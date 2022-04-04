#include "unp.h"

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	// read 函数从套接字读入数据
	// writen 函数把其中内容回射给客户

	// 如果客户端关闭连接，那么接收到客户端的 FIN 将导致服务器子进程的 read 函数返回 0
	// 最终导致此函数（str_echo）返回
	while ((n = read(sockfd, buf, MAXLINE)) > 0)
	{
		Writen(sockfd, buf, n);
	}

	/* Interrupted system call */
	// 被中断了，再次读取
	if (n < 0 && errno == EINTR)
	{
		goto again;
	}
	else if (n < 0)
	{
		err_sys("str_echo: read error");
	}
}
