#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

/* datagrams to send */
#define NDG 2000

/* length of each datagram */
#define DGLEN 1400

void dg_cli_loop(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int i;
    char sendline[DGLEN];

    for (i = 0; i < NDG; i++)
    {
        Sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
    }
}
