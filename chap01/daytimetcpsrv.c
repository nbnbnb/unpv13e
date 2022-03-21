#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
#include <time.h>

int main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;

	//	1 Socket
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// 指定 IP 地址为 INADDR_ANY
	// 这样要是服务器主机有多个网络接口，服务器进程就可以额在任意网路接口上接受客户端连接
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 服务端端口 53315
	servaddr.sin_port = htons(53315);

	// 2. Bind
	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	// 3. Listen
	// 调用 listen 将套接字转换为一个监听套接字
	// 这样来自客户端的外来连接就可以在改套接字上由内核接受
	// LISTENQ 表示在这个监听描述符上排队的组大客户端连接数
	Listen(listenfd, LISTENQ);

	// 1 Socket，2. Bind，3. Listen
	// TCP 服务器准备所谓监听描述符的正常步骤

	for (;;)
	{
		// 通常情况下，服务器进程在 accept 调用中投入睡眠
		// 等待某个客户连接的到达并被内核接受

		// TCP 连接使用所谓的 "三路握手" 来建立连接
		// 握手完毕时 accept 返回，其返回值是一个称为 "已连接描述符" 的新描述符

		// 已连接描述符用于与新近连接的那个客户端通信

		// accept 为每个连接到本服务器的客户端返回一个新描述符
		connfd = Accept(listenfd, (SA *)NULL, NULL);

		// time 返回 Unix 纪元
		// 自 1970.1.1 毫秒数
		ticks = time(NULL);

		// ctime 把整数转换为直观可读的时间格式
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

		// 将结果输出到客户端
		Write(connfd, buff, strlen(buff));

		printf("server response ...\n");

		// 引发正常的 TCP 连接终止序列
		// 每个方向上发送一个 FIN，每个 FIN 又由各自的对端确认
		Close(connfd);
	}

	exit(0);
}
