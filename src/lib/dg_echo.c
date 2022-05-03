#include "unp.h"

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];

	for (;;)
	{
		len = clilen;

		// 使用 recvfrom 读入下一个到达服务器端口的数据报
		// 客户端地址存储在 pcliaddr 指针中
		n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

		// 然后使用 sendto 把它发送给发送者
		Sendto(sockfd, mesg, n, 0, pcliaddr, len);
	}
}
