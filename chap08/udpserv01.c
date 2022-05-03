#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
#include "dgecholoop1.c"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    // SOCK_DGRAM 表示使用 UDP 协议
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 9877 端口
    servaddr.sin_port = htons(SERV_PORT);

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

    // dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    // 对接收到的数据报进行计数
    dg_echo_loop(sockfd, (SA *)&cliaddr, sizeof(cliaddr));
}
