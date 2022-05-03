#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    char *ip = "127.0.0.1";

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, ip, &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}
