#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

// 验证服务器地址
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr *preply_addr;

    // 使用 malloc 来分配另一个套接字地址结构
    preply_addr = Malloc(servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL)
    {

        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        len = servlen;

        // 传递 5,6 参数，得到服务端的地址
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);

        // 如果 sendto 和 recvfrom 中返回的服务器地址不同
        if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0)
        {
            printf("reply from %s (ignored)\n", Sock_ntop(preply_addr, len));
            continue;
        }

        /* null terminate */
        recvline[n] = 0;

        Fputs(recvline, stdout);
    }
}
