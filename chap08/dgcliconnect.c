#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

void dg_cli_by_connect(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;

    char sendline[MAXLINE], recvline[MAXLINE + 1];

    // 调用 connect
    Connect(sockfd, (SA *)pservaddr, servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL)
    {

        // 使用 write 而不是 sendto
        Write(sockfd, sendline, strlen(sendline));

        // 使用 read 而不是 recvfrom
        n = Read(sockfd, recvline, MAXLINE);

        recvline[n] = 0; /* null terminate */
        Fputs(recvline, stdout);
    }
}
