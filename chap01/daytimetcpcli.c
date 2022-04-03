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
	// 库函数 inet_pton 去转换 IP 为合适的格式（支持 IPv6）

	// 端口号转换
	servaddr.sin_port = htons(53315);

	// IP 转换
	// 存储到结构 servaddr.sin_addr 中
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
	{
		err_quit("inet_pton error for %s", ip);
	}

	// SA 是一个 "通用套接字结构"
	// 每当一个套接字函数需要一个指向某个套接字地址结构的指针时
	// 这个指针必须强制转换为一个通用套接字地址结构的指针
	// 这是因为套接字函数早于 ANSI C 标准，20 世纪 80 年代早期开发这些函数时，ANSI C 的 void * 指针类型还不可用
	// 问题是 "struct sockaddr" 长达 15 个字符，往往造成源代码行超出屏幕的右边缘，因此我们把它缩减成 SA

	// connect 使用上面返回的描述符
	// 第二个参数指定连接的服务器信息
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
	{
		err_sys("connect error");
	}

	// 使用 read 函数读取服务器的应答
	while ((n = read(sockfd, recvline, MAXLINE)) > 0)
	{
		// 使用 TCP 时必须小心，因为 TCP 是一个没有记录边界的字节流协议

		// 如果服务器应该返回 26 个字节，那么这 26 个字节可以有多种返回方式
		// 既可以是包含所有 26 个字节的单个 TCP 分节，也可以是每个分节只包含 1 个字节的 26 个 TCP 分节，或者其它任意组合

		// 通常服务器返回包含所有 26 个字节的单个分节
		// 但是如果数据量很大，我们就不能确保一次 read 调用能返回服务器的整个应答

		// 因此从 TCP 套接字读取数据时，我们总是需要把 read 编写在某个循环中
		// 当 read 返回 0（表明对端关闭连接）或负值（表明发生错误）时终止循环

		/* null terminate */
		// 结尾终止符
		recvline[n] = 0;

		// 使用标准 I/O 函数 fputs 输出结果
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
