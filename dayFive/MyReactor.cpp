#include "MyReactor.h"

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
/* static auto my_logger = spdlog::my_logger_mt("my_logger", "logs/daily.txt", 18, 53); */
/* auto my_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt"); */
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
    if(!create_server_listener(ip, nport))
    {
        my_logger->error("Unable to bind: {0} : {1}.", ip, nport);
        return false;
    }

    ARG *arg = new ARG();
    arg->pThis = this;

    pthread_create(&m_accept_threadid, NULL, accept_thread_proc, (void*)arg);

    pthread_create(&m_send_threadid, NULL, send_thread_proc, (void*)arg);

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

    return true;
}


void* MyReactor::main_loop(void *p)
{
    my_logger->info("main thread id = {}", pthread_self());

    MyReactor* pReactor = static_cast<MyReactor*>(p);

    while(!pReactor->m_bStop)
    {
        /* std::cout << "main loop" << std::endl; */
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


        pthread_mutex_lock(&pReactor->m_cli_mutex);
        pReactor->m_fds.insert(newfd);
        pthread_mutex_unlock(&pReactor->m_cli_mutex);

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
                    my_logger->error("recv error, client disconnected, fd = {}", clientfd);
                    pReactor->close_client(clientfd);
                    bError = true;
                    break;
                }
            }
            /* 对端关闭了socket，这端也关闭 */
            else if(nRecv == 0)
            {
                /* 将该客户从客户列表中删除 */
                pthread_mutex_lock(&pReactor->m_cli_mutex);
                pReactor->m_fds.erase(clientfd);
                pthread_mutex_unlock(&pReactor->m_cli_mutex);

                my_logger->info("peer closed, client disconnected, fd = {}", clientfd);
                pReactor->close_client(clientfd);
                bError = true;
                break;
            }

            strclientmsg += buff;
        }

        /* 如果出错了就不必往下执行了 */
        if(bError)
        {
            continue;
        }

        my_logger->info("client msg: {}", strclientmsg);

        /* 将消息加上时间戳 */
        time_t now = time(NULL);
        struct tm* nowstr = localtime(&now);
        std::ostringstream ostimestr;
        ostimestr << "[" << nowstr->tm_year + 1900 << "-"
            << std::setw(2) << std::setfill('0') << nowstr->tm_mon + 1 << "-"
            << std::setw(2) << std::setfill('0') << nowstr->tm_mday << " "
            << std::setw(2) << std::setfill('0') << nowstr->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << nowstr->tm_min << ":"
            << std::setw(2) << std::setfill('0') << nowstr->tm_sec << " :";

        strclientmsg.insert(0, ostimestr.str());

        /* 将消息交给发送消息的线程 */
        pReactor->m_msgs.push_back(strclientmsg);
        pthread_cond_signal(&pReactor->m_send_cond);
    }
    return NULL;
}


void* MyReactor::send_thread_proc(void *args)
{
    ARG *arg = (ARG*)args;
    MyReactor* pReactor = arg->pThis;

    while(!pReactor->m_bStop)
    {
        std::string strclientmsg;

        pthread_mutex_lock(&pReactor->m_send_mutex);
        /* 注意！要用while循环等待 */
        while(pReactor->m_msgs.empty())
            pthread_cond_wait(&pReactor->m_send_cond, &pReactor->m_send_mutex);

        strclientmsg = pReactor->m_msgs.front();
        pReactor->m_msgs.pop_front();
        pthread_mutex_unlock(&pReactor->m_send_mutex);

        std::cout << std::endl;


        while(1)
        {
            int nSend;
            int clientfd;
            for(auto it = pReactor->m_fds.begin(); it != pReactor->m_fds.end(); it++)
            {
                clientfd = *it;
                nSend = send(clientfd, strclientmsg.c_str(), strclientmsg.length(), 0);
                if(nSend == -1)
                {
                    if(errno == EWOULDBLOCK)
                    {
                        sleep(10);
                        continue;
                    }
                    else
                    {
                        my_logger->error("send error, fd = {}", clientfd);
                        pReactor->close_client(clientfd);
                        break;
                    }
                }
            }

            my_logger->info("send: {}", strclientmsg);
            /* 发送完把缓冲区清干净 */
            strclientmsg.clear();

            if(strclientmsg.empty())
                break;
        }
    }

    return NULL;
}
