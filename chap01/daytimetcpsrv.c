#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
#include <time.h>

// 注意
// 需要通过管理员权限运行

int main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* daytime server */
	servaddr.sin_port = htons(13);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for (;;)
	{
		connfd = Accept(listenfd, (SA *)NULL, NULL);

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		// printf("server response ...");

		Close(connfd);
	}
}
