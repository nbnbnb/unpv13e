#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
#include "dgcliconnect.c"
#include "dgcliloop1.c"

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

    // 通过使用 sendto 进行发送
    // dg_cli(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    // 通过使用 connect 进行发送
    // dg_cli_by_connect(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    // 客户端批量发送行数据
    // 验证流量控制
    dg_cli_loop(sockfd, (SA *)&servaddr, sizeof(servaddr));

    // 服务端的计数达不到指定的数量，程序就退出了
    exit(0);
}
