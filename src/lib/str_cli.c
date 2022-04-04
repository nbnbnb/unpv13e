#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	// 当遇到文件结束符或错误时
	// fgets 将返回一个空指针，于是客户端处理循环终止
	while (Fgets(sendline, MAXLINE, fp) != NULL)
	{
		// 回车也会输出

		printf("--- start ---\n");

		// 输出后
		Writen(sockfd, sendline, strlen(sendline));

		// 立马读取响应
		if (Readline(sockfd, recvline, MAXLINE) == 0)
		{
			err_quit("str_cli: server terminated prematurely");
		}

		Fputs(recvline, stdout);

		printf("---- end ----\n");
	}
}
