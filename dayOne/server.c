//-------------------------服务器端server.c-------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //数据类型定义
#include <sys/stat.h> //文件属性
#include <netinet/in.h> //定义数据结构sockaddr_in
#include <sys/socket.h> //提供socket函数和数据结构
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h> //共享内存
#include <time.h>
#include <sys/time.h>

#define PERM S_IRUSR | S_IWUSR //用户读写
#define MYPORT  12345 //通信端口
#define BACKLOG 10 //定义服务器段可以连接的最大客户数
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

//创建共享存储区
int shm_create()
{
    int shmid;
    if ((shmid = shmget(IPC_PRIVATE, 1024, PERM)) == -1)
    {
        fprintf(stderr, "Create Share Memory Error:%s\n\a", strerror(errno));
        exit(1);
    }
    return shmid;
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
    int sockfd, clientfd; //监听套接字、客户套接字
    int sin_size, recvbytes;

    pid_t pid, ppid; //定义父子进程标记
    char *buf, *read_addr, *write_addr, *temp, *time_str; //需要用到的缓冲区
    struct sockaddr_in their_addr; //定义地址结构
    int shmid;

    shmid = shm_create(); //创建共享存储区

    temp = (char *)malloc(255);
    time_str = (char *)malloc(50);
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
    while (1)
    {
        //接受一个客户端的连接请求,相当于不限客户数，群发消息
        if ((clientfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("fail to accept");
            exit(1);
        }
        printf("%d \n", clientfd);
        //得到客户端的IP地址输出
        char address[20];
        inet_ntop(AF_INET, &their_addr.sin_addr, address, sizeof(address));

        //printf("accept from %s\n", address);
        send(clientfd, WELCOME, strlen(WELCOME), 0); //发送问候信息
        buf = (char *)malloc(255);

        ppid = fork(); //创建子进程
        if (ppid == 0) //子进程
        {
            pid = fork(); //子进程创建子进程
            while (1)
            {
                if (pid > 0)
                {
                    //buf = (char *)malloc(255);
                    //父进程用于接收信息
                    memset(buf, 0, 255);
                    printf("OK\n");
                    if ((recvbytes = recv(clientfd, buf, 255, 0)) <= 0)
                    {
                        perror("fail to recv");
                        close(clientfd);
                        raise(SIGKILL);
                        exit(1);
                    }
                    write_addr = shmat(shmid, 0, 0); //shmat将shmid所代表的全局的共享存储区关联到本进程的进程空间
                    memset(write_addr, '\0', 1024);

                    //把接收到的消息存入共享存储区中
                    strncpy(write_addr, buf, 1024);

                    //把接收到的消息连接此刻的时间字符串输出到标准输出
                    get_cur_time(time_str);
                    strcat(buf, time_str);
                    printf("%s\n", buf);
                }
                else if (pid == 0)
                {
                    //子进程的子进程用于发送消息
                    sleep(1); //子进程先等待父进程把接收到的信息存入共享存储区
                    read_addr = shmat(shmid, 0, 0); //读取共享存储区的内容

                    //temp存储上次读取过的内容,每次先判断是否已经读取过该消息
                    if (strcmp(temp, read_addr) != 0)
                    {
                        strcpy(temp, read_addr); //更新temp，表示已经读取过该消息

                        get_cur_time(time_str);
                        strcat(read_addr, time_str);
                        if (send(clientfd, read_addr, strlen(read_addr), 0) == -1)
                        {
                            perror("fail to send");
                            exit(1);
                        }
                        memset(read_addr, '\0', 1024);
                        strcpy(read_addr, temp);
                    }
                }
                else
                    perror("fail to fork");
            }
        }
    }
    printf("------------------------------------\n");
    free(buf);
    close(sockfd);
    close(clientfd);
    return 0;
}
