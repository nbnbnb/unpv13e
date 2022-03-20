#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

// 注意：先启动服务端
// sudo ./daytimetcpsrv

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	char *ip = "127.0.0.1";

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	printf("ggg response ...\n");

	/* daytime server */
	servaddr.sin_port = htons(13);

	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
	{
		err_quit("inet_pton error for %s", ip);
	}

	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
	{
		err_sys("connect error");
	}

	while ((n = read(sockfd, recvline, MAXLINE)) > 0)
	{
		/* null terminate */
		// 结尾终止符
		recvline[n] = 0;

		if (fputs(recvline, stdout) == EOF)
		{
			err_sys("fputs error");
		}
	}

	if (n < 0)
	{
		err_sys("read error");
	}

	exit(0);
}
