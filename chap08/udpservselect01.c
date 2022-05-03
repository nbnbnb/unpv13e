#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int listenfd, connfd, udpfd, nready, maxfdp1;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);

    /* 4 create listening TCP socket */
    // tcp 监听描述符
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 9877 端口
    servaddr.sin_port = htons(SERV_PORT);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    /* 4 create UDP socket */
    // udp 描述符
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 9877 端口
    servaddr.sin_port = htons(SERV_PORT);

    Bind(udpfd, (SA *)&servaddr, sizeof(servaddr));

    // ---------------------------

    // 当子进程退出的时候执行（回收子进程，避免子进程僵死）
    /* must call waitpid() */
    Signal(SIGCHLD, sig_chld);

    FD_ZERO(&rset);

    // 计算最大描述符（+1）
    maxfdp1 = max(listenfd, udpfd) + 1;

    for (;;)
    {
        // 设置 select 中的位
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
        {
            // 如果是被线程中断的，则继续
            if (errno == EINTR)
            {
                /* back to for() */
                continue;
            }
            else
            {
                err_sys("select error");
            }
        }

        // TCP 到达（并发模式）
        if (FD_ISSET(listenfd, &rset))
        {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &len);

            // 创建一个子进程
            if ((childpid = Fork()) == 0)
            {
                /* child process */

                /* close listening socket */
                Close(listenfd);

                /* process the request */
                str_echo(connfd);

                exit(0);
            }

            /* parent closes connected socket */
            Close(connfd);
        }

        // UDP 到达（迭代模式）
        if (FD_ISSET(udpfd, &rset))
        {
            len = sizeof(cliaddr);

            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);

            Sendto(udpfd, mesg, n, 0, (SA *)&cliaddr, len);
        }
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    // 等待一个子进程终止
    // 只有调用 wait 或者 waitpid 才不会有僵死进程
    pid = wait(&stat);

    printf("child %d terminated\n", pid);

    // 约定，void 也设置 return 语句
    // 这样我们就可以得知该系统调用具体是被哪个信号处理函数的哪个 return 语句中断的
    return;
}