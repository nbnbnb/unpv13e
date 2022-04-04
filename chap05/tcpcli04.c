#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int i, sockfd[5];
    struct sockaddr_in servaddr;
    char *ip = "127.0.0.1";

    // 同时创建 10 个客户端
    for (i = 0; i < 10; i++)
    {
        sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        Inet_pton(AF_INET, ip, &servaddr.sin_addr);

        Connect(sockfd[i], (SA *)&servaddr, sizeof(servaddr));
    }

    /* do it all */
    str_cli(stdin, sockfd[0]);

    // 当客户端终止的时候
    // 服务端子进程同时也会收到多个 SIGCHLD 信号
    // 需要都每个都正确处理
    // 要不就会导致僵死子进程
    exit(0);
}
