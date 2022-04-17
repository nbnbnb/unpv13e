#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

void str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;

    // 标准输入上的 EOF 标志
    stdineof = 0;

    FD_ZERO(&rset);

    for (;;)
    {
        if (stdineof == 0)
        {
            FD_SET(fileno(fp), &rset);
        }

        FD_SET(sockfd, &rset);

        // 待测试的描述符个数
        // 设置为待测试的最大描述符加 1
        maxfdp1 = max(fileno(fp), sockfd) + 1;

        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset))
        {
            // 使用 read 和 write 对缓冲区而不是文本行进行操作
            // 使得 select 能够如期的工作

            // 在套接字上读到 EOF 时
            if ((n = Read(sockfd, buf, MAXLINE)) == 0)
            {
                // 如果已在标准输入上遇到 EOF
                // 那就是正常的终止，于是函数返回
                if (stdineof == 1)
                {
                    /* normal termination */
                    return;
                }
                else
                {
                    // 如果没有在标准输入上遇到 EOF
                    // 那么服务器进程已过早终止
                    err_quit("str_cli: server terminated prematurely");
                }
            }

            Write(fileno(stdout), buf, n);
        }

        if (FD_ISSET(fileno(fp), &rset))
        {

            // 使用 read 和 write 对缓冲区而不是文本行进行操作
            // 使得 select 能够如期的工作

            // 当在标准输入上碰到 EOF 时
            if ((n = Read(fileno(fp), buf, MAXLINE)) == 0)
            {
                // 设置 EOF 标志
                stdineof = 1;

                // 把第二个参数设置为 SHUT_WR 来调用 shutdown 以发送 FIN
                // 关闭连续的写这一半
                /* send FIN */
                Shutdown(sockfd, SHUT_WR);

                FD_CLR(fileno(fp), &rset);

                continue;
            }

            // 发送到网路 sockfd 中
            Writen(sockfd, buf, n);
        }
    }
}
