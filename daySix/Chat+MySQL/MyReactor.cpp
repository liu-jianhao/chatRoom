#include "MyReactor.h"

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
auto my_logger = spdlog::stdout_color_mt("my_logger");

MyReactor::MyReactor()
{
}

MyReactor::~MyReactor()
{
}

struct ARG
{
    MyReactor* pThis;
};

bool MyReactor::init(const char* ip, short nport)
{
    //连接数据库
    int ret = connect();
    if(ret == -1)
    {
        my_logger->error("mysql connection failed");
    }


    if(!create_server_listener(ip, nport))
    {
        my_logger->error("Unable to bind: {0}:{1}.", ip, nport);
        return false;
    }

    std::cout << "main thread id = " << pthread_self() << std::endl;

    ARG *arg = new ARG();
    arg->pThis = this;

    pthread_create(&m_accept_threadid, NULL, accept_thread_proc, (void*)arg);

    for(int i = 0; i < WORKER_THREAD_NUM; i++)
    {
        pthread_create(&m_threadid[i], NULL, worker_thread_proc, (void*)arg);
    }

    return true;
}


bool MyReactor::uninit()
{
    m_bStop = true;

    /* 将读端和写端都关闭 */
    shutdown(m_listenfd, SHUT_RDWR);
    close(m_listenfd);
    close(m_epollfd);

    mysql_close(mysql);

    return true;
}


void* MyReactor::main_loop(void *p)
{
    my_logger->info("main thread id = {}", pthread_self());

    MyReactor* pReactor = static_cast<MyReactor*>(p);

    while(!pReactor->m_bStop)
    {
        struct epoll_event ev[1024];
        int n = epoll_wait(pReactor->m_epollfd, ev, 1024, 10);
        if(n == 0)
            continue;
        else if(n < 0)
        {
            my_logger->error("epoll_wait error");
            continue;
        }

        int m = std::min(n, 1024);
        for(int i = 0; i < m; i++)
        {
            /* 有新连接 */
            if(ev[i].data.fd == pReactor->m_listenfd)
                pthread_cond_signal(&pReactor->m_accept_cond);
            /* 有数据 */
            else
            {
                pthread_mutex_lock(&pReactor->m_client_mutex);
                pReactor->m_clientlist.push_back(ev[i].data.fd);
                pthread_mutex_unlock(&pReactor->m_client_mutex);
                pthread_cond_signal(&pReactor->m_client_cond);
            }
        }
    }

    my_logger->info("main loop exit ...");
    return NULL;
}


bool MyReactor::close_client(int clientfd)
{
    if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, clientfd, NULL) == -1)
    {
        my_logger->warn("release client socket failed as call epoll_ctl fail");
    }

    close(clientfd);
    return true;
}


bool MyReactor::create_server_listener(const char* ip, short port)
{
    m_listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(m_listenfd == -1)
    {
        my_logger->error("fail to create a socket");
        return false;
    }

    int on = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    if(bind(m_listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1)
        return false;

    if(listen(m_listenfd, 50) == -1)
        return false;

    m_epollfd = epoll_create(1);
    if(m_epollfd == -1)
        return false;

    struct epoll_event e;
    memset(&e, 0, sizeof(e));
    e.events = EPOLLIN | EPOLLRDHUP;
    e.data.fd = m_listenfd;
    if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &e) == -1)
        return false;

    return true;
}


void* MyReactor::accept_thread_proc(void* args)
{
    ARG *arg = (ARG*)args;
    MyReactor* pReactor = arg->pThis;

    while(!pReactor->m_bStop)
    {
        pthread_mutex_lock(&pReactor->m_accept_mutex);
        pthread_cond_wait(&pReactor->m_accept_cond, &pReactor->m_accept_mutex);

        struct sockaddr_in clientaddr;
        socklen_t addrlen;
        int newfd = accept(pReactor->m_listenfd, (struct sockaddr *)&clientaddr, &addrlen);
        pthread_mutex_unlock(&pReactor->m_accept_mutex);
        if(newfd == -1)
            continue;

        my_logger->info("new client connected: ");

        /* 将新socket设置为non-blocking */
        int oldflag = fcntl(newfd, F_GETFL, 0);
        int newflag = oldflag | O_NONBLOCK;
        if(fcntl(newfd, F_SETFL, newflag) == -1)
        {
            my_logger->error("fcntl error, oldflag = {0}, newflag = {1}", oldflag, newflag);
            continue;
        }

        struct epoll_event e;
        memset(&e, 0, sizeof(e));
        e.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        e.data.fd = newfd;
        /* 添加进epoll的兴趣列表 */
        if(epoll_ctl(pReactor->m_epollfd, EPOLL_CTL_ADD, newfd, &e) == -1)
        {
            my_logger->error("epoll_ctl error, fd = {}", newfd);
        }
    }

    return NULL;
}

void* MyReactor::worker_thread_proc(void* args)
{
    ARG *arg = (ARG*)args;
    MyReactor* pReactor = arg->pThis;

    while(!pReactor->m_bStop)
    {
        int clientfd;
        pthread_mutex_lock(&pReactor->m_client_mutex);
        /* 注意！要用while循环等待 */
        while(pReactor->m_clientlist.empty())
            pthread_cond_wait(&pReactor->m_client_cond, &pReactor->m_client_mutex);

        /* 取出客户套接字 */
        clientfd = pReactor->m_clientlist.front();
        pReactor->m_clientlist.pop_front();
        pthread_mutex_unlock(&pReactor->m_client_mutex);

        std::cout << std::endl;


        std::string strclientmsg;
        char buff[256];
        bool bError = false;
        while(1)
        {
            memset(buff, 0, sizeof(buff));
            int nRecv = recv(clientfd, buff, 256, 0);
            if(nRecv == -1)
            {
                if(errno == EWOULDBLOCK)
                    break;
                else
                {
                    std::cout << "recv error, client disconnected, fd = " << clientfd << std::endl;
                    pReactor->close_client(clientfd);
                    bError = true;
                    break;
                }
            }
            /* 对端关闭了socket，这端也关闭 */
            else if(nRecv == 0)
            {
                std::cout << "peer clised, client disconnected, fd = " << clientfd << std::endl;
                pReactor->close_client(clientfd);
                bError = true;
                break;
            }


            //如果是注册或登录命令，则要另外处理
            if(buff[0] == '1')
            {
                std::string rec(buff);
                auto it = rec.find('\t');
                const char *name = rec.substr(1, it-1).c_str();
                const char *password = rec.substr(it+1, rec.size()-1).c_str();

                std::cout << name << '\t' << password << std::endl;
                char query[100];
                sprintf(query, "insert into UserInfo values('%s', '%s')", name, password);
                int ret = pReactor->sqlQuery(query);
                if(ret == -1)
                {
                    my_logger->error("INSERT error {}", mysql_error(pReactor->mysql));
                }
                else
                {
                    int nSend = send(clientfd, "REGISTER SUCCESS", sizeof("REGISTER SUCCESS"), 0);
                    if(nSend == -1)
                    {
                        if(errno == EWOULDBLOCK)
                        {
                            sleep(10);
                        }
                        else
                        {
                            my_logger->error("send error, fd = {}", clientfd);
                            pReactor->close_client(clientfd);
                            break;
                        }
                    }
                    //注册成功
                    continue;
                    if(mysql_errno(pReactor->mysql))
                    {
                        my_logger->error("Retrive error {}", mysql_error(pReactor->mysql));
                    }
                }
                mysql_free_result(pReactor->res_ptr);
            }
            else if(buff[0] == '2')
            {
                std::string rec(buff);
                auto it = rec.find('\t');
                const char *name = rec.substr(1, it-1).c_str();
                const char *password = rec.substr(it+1, rec.size()-1).c_str();

                std::cout << name << '\t' << password << std::endl;
                char query[100];
                sprintf(query, "select password from UserInfo where username = '%s'", name);
                int ret = pReactor->sqlQuery(query);
                if(ret == -1)
                {
                    my_logger->error("SELECT error {}", mysql_error(pReactor->mysql));
                }
                else
                {
                    pReactor->res_ptr = mysql_store_result(pReactor->mysql);
                    if(pReactor->res_ptr)
                    {
                        pReactor->sqlrow = mysql_fetch_row(pReactor->res_ptr);
                        if(strcmp(pReactor->sqlrow[0], password) == 0)
                        {
                            int nSend = send(clientfd, "LOG IN SUCCESS", sizeof("LOG IN SUCCESS"), 0);
                            if(nSend == -1)
                            {
                                if(errno == EWOULDBLOCK)
                                {
                                    sleep(10);
                                }
                                else
                                {
                                    my_logger->error("send error, fd = {}", clientfd);
                                    pReactor->close_client(clientfd);
                                    break;
                                }
                            }
                            //登录成功
                            continue;
                        }
                        else
                        {
                            int nSend = send(clientfd, "password is wrong", sizeof("password is wrong"), 0);
                            if(nSend == -1)
                            {
                                if(errno == EWOULDBLOCK)
                                {
                                    sleep(10);
                                }
                                else
                                {
                                    my_logger->error("send error, fd = {}", clientfd);
                                    pReactor->close_client(clientfd);
                                    break;
                                }
                            }
                        }
                        if(mysql_errno(pReactor->mysql))
                        {
                            my_logger->error("Retrive error {}", mysql_error(pReactor->mysql));
                        }
                    }
                    mysql_free_result(pReactor->res_ptr);
                }
            }

            strclientmsg += buff;
        }

        /* 如果出错了就不必往下执行了 */
        if(bError)
        {
            continue;
        }

        my_logger->info("client msg: {}", strclientmsg);
    }
    return NULL;
}


int MyReactor::connect()
{
    mysql = mysql_init(NULL);
    if(!mysql)
    {
        my_logger->error("mysql init falied");
        return -1;
    }

    /* root为用户名，liujianhao为密码，test为要连接的database */
    mysql = mysql_real_connect(mysql, "localhost", "root", "liujianhao", "ChatRoom", 0, NULL, 0);

    if(mysql)
    {
        my_logger->info("MySQL connection success");
    }
    else
    {
        my_logger->warn(" MySQL connection failed");
    }
    return 0;
}


int MyReactor::sqlQuery(const char* query)
{
    int res = mysql_query(mysql, query);
    if(res)
    {
        return -1;
    }
    return 0;
}
