#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
#include "sigchldwait.c"

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    void sig_chld(int);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    // 注册信号
    // 等待子进程终止
    Signal(SIGCHLD, sig_chld);

    printf("Server starting at 127.0.0.1:9877\n");

    for (;;)
    {
        clilen = sizeof(cliaddr);

        // 如果在 accept 阻塞时，捕获到一个 SIGCHLD 信号
        // 内核就会使 accept 返回一个 EINTR 错误

        // 有些系统，不会使内核自动重启被中断的系统调用，于是服务终止

        // Linux 系统自动重启被中断的应用，所以此处不会终止（accept 不会返回错误）
        connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

        if (connfd < 0)
        {
            // 有些系统
            // 需要处理中断的 accept
            if (errno == EINTR)
            {
                continue;
            }
        }

        if ((childpid = Fork()) == 0)
        {
            /* child process */

            /* close listening socket */
            Close(listenfd);

            /* process the request */
            str_echo(connfd);

            exit(0);
        }

        Close(connfd); /* parent closes connected socket */
    }
}
