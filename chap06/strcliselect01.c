#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

/**
 * @brief 有问题版本
 * 参考  strcliselect02.c 修订版
 *
 * @param fp
 * @param sockfd
 */

void str_cli(FILE *fp, int sockfd)
{
    // 最大描述符数
    int maxfdp1;

    // 描述符集
    fd_set rset;

    char sendline[MAXLINE], recvline[MAXLINE];

    // 使用 FD_ZERO 宏进行初始化
    FD_ZERO(&rset);

    for (;;)
    {
        // 用 FD_SET 打开位
        // 对应标准 I/O 文件 fp（注意，使用 fileno 函数获取 fd）
        FD_SET(fileno(fp), &rset);

        // 用 FD_SET 打开位
        // 对应套接字 sockfd
        FD_SET(sockfd, &rset);

        // 待测试的描述符个数
        // 设置为待测试的最大描述符加 1
        maxfdp1 = max(fileno(fp), sockfd) + 1;

        // 系统调用 select
        // 等待返回
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        // 如果套接字是可读的
        if (FD_ISSET(sockfd, &rset))
        {
            // 用 readline 读入回射文本行
            if (Readline(sockfd, recvline, MAXLINE) == 0)
            {
                err_quit("str_cli: server terminated prematurely");
            }

            // 再用 fputs 输出
            Fputs(recvline, stdout);
        }

        // 如果标准 I/O 是可读的
        if (FD_ISSET(fileno(fp), &rset))
        {
            // 读入行
            if (Fgets(sendline, MAXLINE, fp) == NULL)
            {
                /* all done */
                return;
            }

            // 输出行到 sockfd
            Writen(sockfd, sendline, strlen(sendline));
        }
    }
}
