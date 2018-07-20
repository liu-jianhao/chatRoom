#include "MyReactor.h"


#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
auto main_logger = spdlog::stdout_color_mt("main_logger");


MyReactor g_reactor;


void prog_exit(int signo)
{
    main_logger->info("program recv signal {} to exit.", signo);
    g_reactor.uninit();
}

void daemon_run()
{
    int pid;
    signal(SIGCHLD, SIG_IGN);
    pid = fork();
    if (pid < 0)
    {
        main_logger->error("fork error");
        exit(-1);
    }
    else if (pid > 0) {
        exit(0);
    }
    //之前parent和child运行在同一个session里,parent是会话（session）的领头进程,
    //parent进程作为会话的领头进程，如果exit结束执行的话，那么子进程会成为孤儿进程，并被init收养。
    //执行setsid()之后,child将重新获得一个新的会话(session)id。
    //这时parent退出之后,将不会影响到child了。
    setsid();
    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if (fd > 2)
        close(fd);
}


int main(int argc, char* argv[])
{
    //设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGKILL, prog_exit);
    signal(SIGTERM, prog_exit);

    short port = 0;
    int ch;
    bool bdaemon = false;
    while ((ch = getopt(argc, argv, "p:d")) != -1)
    {
        switch (ch)
        {
            case 'd':
                bdaemon = true;
                break;
            case 'p':
                port = atol(optarg);
                break;
        }
    }

    if (bdaemon)
        daemon_run();


    if (port == 0)
        port = 12345;

    if (!g_reactor.init("0.0.0.0", 12345))
        return -1;


    g_reactor.main_loop(&g_reactor);

    main_logger->info("main exit");

    return 0;
}


