#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define    MAXLINE        1024

void usage(char *command)
{
    printf("usage :%s portnum filename\n", command);
    exit(0);
}
int main(int argc,char **argv)
{
    struct sockaddr_in     serv_addr;
    struct sockaddr_in     clie_addr;
    char                   buf[MAXLINE];
    int                    sock_id;
    int                    link_id;
    int                    recv_len;
    int                    write_leng;
    int                    clie_addr_len;
    FILE                   *fp;

    if (argc != 3) {
        usage(argv[0]);
    }
    if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create socket failed\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) {
        perror("Bind socket failed\n");
        exit(0);
    }

    if (-1 == listen(sock_id, 10)) {
        perror("Listen socket failed\n");
        exit(0);
    }

    while (1) {
        if ((fp = fopen(argv[2], "w")) == NULL) {
            perror("Open file failed\n");
            exit(0);
        }


        clie_addr_len = sizeof(clie_addr);
        link_id = accept(sock_id, (struct sockaddr *)&clie_addr, &clie_addr_len);
        if (-1 == link_id) {
            perror("Accept socket failed\n");
            exit(0);
        }
        bzero(buf, MAXLINE);
        while (recv_len = recv(link_id, buf, MAXLINE, 0)) {
            if(recv_len < 0) {
                printf("Recieve Data From Server Failed!\n");
                break;
            }
            /*
            else if(recv_len == 0){
                printf("finish\n");
                break;
            }
            */
            printf("#");
            write_leng = fwrite(buf, sizeof(char), recv_len, fp);
            if (write_leng < recv_len) {
                printf("Write file failed\n");
                break;
            }
            bzero(buf,MAXLINE);
        }
        printf("\nFinish Recieve\n");

        char cmd[100];
        sprintf(cmd, "aplay %s", argv[2]);
        system(cmd);

        fclose(fp);
        close(link_id);
    }
    close(sock_id);
    return 0;
}
