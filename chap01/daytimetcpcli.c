#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

// 注意：先启动服务端
// sudo ./daytimetcpsrv

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	char *ip = "127.0.0.1";

	// 建立一个 TCP 套接字（AF_INT：网际，SOCK_STREAM：字节流）
	// 返回一个小整数描述符
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_sys("socket error");
	}

	// bzero 把 servaddr 结构清零
	bzero(&servaddr, sizeof(servaddr));

	// 设置地址族为 AF_INET
	servaddr.sin_family = AF_INET;

	// 由于 TCP 套接字中 IP 和 端口号需要用特定的格式
	// 所以调用
	// 库函数 htons 去转换二进制端口号
	// 库函数 inet_pton 去转换 IP 为合适的格式

	// 端口号转换
	servaddr.sin_port = htons(53315);

	// IP 转换
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
	{
		err_quit("inet_pton error for %s", ip);
	}

	// connect 使用上面返回的描述符
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
