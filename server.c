//-------------------------服务器端server.c-------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //数据类型定义
#include <sys/stat.h> //文件属性
#include <netinet/in.h> //定义数据结构sockaddr_in
#include <sys/socket.h> //提供socket函数和数据结构
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define PERM S_IRUSR | S_IWUSR //用户读写
#define MAXLINE 4096
#define MYPORT  12345 //通信端口
#define BACKLOG 2 //定义服务器段可以连接的最大客户数
#define WELCOME "|---------------Welcome to the chat room!----------------|"//当客户端连接服务端时，向客户端发送此字符串



//将int类型转换成char*类型
void itoa(int i, char *string)
{
    int mask = 1;
    while (i / mask >= 10)
        mask *= 10;
    while (mask > 0)
    {
        *string++ = i / mask + '0';
        i %= mask;
        mask /= 10;
    }
    *string = '\0';
}

//得到当前系统的时间
void get_cur_time(char *time_str)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    strcpy(time_str, ctime(&now.tv_sec));
}


//端口绑定函数。创建套接字，并绑定到指定端口
int bindPort(unsigned short int port)
{
    int sockfd;
    struct sockaddr_in my_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //创建基于流套接字
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET; //IPV4协议族
    my_addr.sin_port = htons(port); //转换端口为网络字节序
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("fail to bind");
        exit(1);
    }

    printf("bind success!\n");
    return sockfd;
}

int main(int argc, char *argv[])
{
    int sockfd, clientfd[2]; //监听套接字、客户套接字
    int sin_size, recvbytes;

    pid_t pid, ppid; //定义父子进程标记
    char buf[MAXLINE], temp[MAXLINE], time_str[MAXLINE]; //需要用到的缓冲区
    struct sockaddr_in their_addr; //定义地址结构

    sockfd = bindPort(MYPORT); //绑定端口

    get_cur_time(time_str);
    printf("Time is : %s\n", time_str);

    if (listen(sockfd, BACKLOG) == -1)
    {
        //在指定端口上监听
        perror("fail to listen");
        exit(1);
    }

    printf("listen....\n");
    /* 两个客户聊天 */
    int i = 0;
    for(; i < 2; i++){
        sin_size = sizeof(their_addr);
        if ((clientfd[i] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("fail to accept");
            exit(1);
        }
    }


    send(clientfd[0], WELCOME, strlen(WELCOME), 0); //发送问候信息
    send(clientfd[1], WELCOME, strlen(WELCOME), 0); //发送问候信息

    ppid = fork(); //创建子进程
    if (ppid == 0) //子进程
    {
        pid = fork(); //子进程创建子进程
        while (1)
        {
            if (pid > 0)
            {
                bzero(&buf, sizeof(buf));
                printf("OK\n");
                if ((recvbytes = recv(clientfd[1], buf, 255, 0)) <= 0)
                {
                    perror("fail to recv");
                    close(clientfd[0]);
                    raise(SIGKILL);
                    exit(1);
                }

                get_cur_time(time_str);
                strcat(buf, time_str);
                if (send(clientfd[0], buf, strlen(buf), 0) == -1)
                {
                    perror("fail to send");
                    exit(1);
                }
            }

            else if (pid == 0)
            {
                bzero(&buf, sizeof(buf));
                printf("OK\n");
                if ((recvbytes = recv(clientfd[0], buf, 255, 0)) <= 0)
                {
                    perror("fail to recv");
                    close(clientfd[1]);
                    raise(SIGKILL);
                    exit(1);
                }

                get_cur_time(time_str);
                strcat(buf, time_str);
                if (send(clientfd[1], buf, strlen(buf), 0) == -1)
                {
                    perror("fail to send");
                    exit(1);
                }
                /* memset(buf, '\0', 1024); */
            }
            else
                perror("fail to fork");
        }
        waitpid(pid, NULL, 0);
    }
    printf("------------------------------------\n");
    waitpid(ppid, NULL, 0);
    close(sockfd);
    close(clientfd[0]);
    close(clientfd[1]);
    return 0;
}
