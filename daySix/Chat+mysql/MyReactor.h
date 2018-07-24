#ifndef __MYREACTOR_H
#define __MYREACTOR_H


#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  //for htonl() and htons()
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>     //for signal()
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <list>
#include <set>
#include <time.h>
#include <sstream>
#include <deque>
#include <vector>
#include <iomanip> //for std::setw()/setfill()
#include <stdlib.h>

#include <sys/stat.h>

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "command.h"

#include <mysql.h>

#define WORKER_THREAD_NUM 5


class MyReactor{
    public:
        MyReactor();
        ~MyReactor();

        /* 初始化socket和线程，供应用程序调用 */
        bool init(const char *ip, short nport);

        bool uninit();

        bool close_client(int clientfd);

        static void* main_loop(void* loop);

        //数据库操作
        int connect();
        int sqlQuery(const char*);

    private:
        /* 见条款6，阻止有人调用复制构造函数和赋值运算符构造函数 */
        MyReactor(const MyReactor& rhs);
        MyReactor& operator = (const MyReactor& rhs);

        static void *accept_thread_proc(void* args);
        static void *worker_thread_proc(void* args);

        static void *send_thread_proc(void* args);

        bool create_server_listener(const char* ip, short port);


    private:
        /* 服务器端的socket */
        int m_listenfd = 0;
        /* 让线程可以修改它 */
        int m_epollfd = 0;
        /* 线程ID */
        pthread_t m_accept_threadid;
        pthread_t m_threadid[WORKER_THREAD_NUM];

        pthread_t m_send_threadid;

        /* 接受客户的信号量 */
        pthread_mutex_t m_accept_mutex = PTHREAD_MUTEX_INITIALIZER;
        /* 有新连接的条件变量 */
        pthread_cond_t m_accept_cond = PTHREAD_COND_INITIALIZER;
        /* 添加、取出客户链表的信号量 */
        pthread_mutex_t m_client_mutex = PTHREAD_MUTEX_INITIALIZER;
        /* 通知工作线程有客户消息的条件变量 */
        pthread_cond_t m_client_cond = PTHREAD_COND_INITIALIZER;

        pthread_mutex_t m_send_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t m_send_cond = PTHREAD_COND_INITIALIZER;
        std::deque<std::string> m_msgs;

        pthread_mutex_t m_cli_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t m_cli_cond = PTHREAD_COND_INITIALIZER;

        std::set<int> m_fds;

        /* 存储连接客户的链表，用队列也一样*/
        std::list<int> m_clientlist;


        /* 决定主线程、accept线程、工作线程是否继续迭代 */
        bool m_bStop = false;

        /* MySQL数据库 */
        MYSQL *mysql;
        MYSQL_RES *res_ptr;
        MYSQL_ROW sqlrow;
};

#endif
