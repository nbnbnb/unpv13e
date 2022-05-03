#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int sockfd;
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;

    char *ip = "127.0.0.1";

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, ip, &servaddr.sin_addr);

    // 先调用 connect
    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    len = sizeof(cliaddr);

    // 然后调用 getsockname 获取外出接口信息
    Getsockname(sockfd, (SA *)&cliaddr, &len);

    // local address 127.0.0.1:39776
    printf("local address %s\n", Sock_ntop((SA *)&cliaddr, len));

    exit(0);
}
