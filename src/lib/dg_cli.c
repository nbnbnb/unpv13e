#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int n;

	char sendline[MAXLINE], recvline[MAXLINE + 1];

	// fgets 从标准输入读入一个文本行
	while (Fgets(sendline, MAXLINE, fp) != NULL)
	{
		// sendto 函数将文本行发送给服务器
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		// recvfrom 读回服务器的回射
		// 第五，第六个参数为 NULL，表示我们不关系应答数据报由谁发送
		// 此处可能回一直阻塞（一般设置一个超时时间）
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

		// 设置 EOF
		/* null terminate */
		recvline[n] = 0;

		// 把回射的文本行显示到标准输出
		Fputs(recvline, stdout);
	}
}
