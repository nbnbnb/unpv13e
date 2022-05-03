#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

static void recvfrom_int_inner(int);
static int count;

void dg_echo_loop_with_rcvbuf(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n;
    int max_buf;
    socklen_t len;
    char mesg[MAXLINE];

    // 当用终端键终止服务器时，服务器会显示所接收到数据报的数目并终止
    Signal(SIGINT, recvfrom_int_inner);

    // 设置 RCVBUF
    Setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &max_buf, sizeof(max_buf));

    for (;;)
    {
        len = clilen;

        // 使用 recvfrom 读入下一个到达服务器端口的数据报
        // 客户端地址存储在 pcliaddr 指针中
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        // 然后使用 sendto 把它发送给发送者
        Sendto(sockfd, mesg, n, 0, pcliaddr, len);

        // 计数
        count++;
    }
}

static void recvfrom_int_inner(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}
