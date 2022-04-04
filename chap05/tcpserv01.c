#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    // 通配地址 INADDR_ANY
    // 通配地址告诉系统：要是系统是多宿主机，将接受目的地址为任何本地接口的连接
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 9877 端口
    // 比 1023 大（我们不需要一个保留端口）
    // 比 5000 大（以免许多源自 Berkeley 的实现分配临时端口的范围冲突）
    // 比 49152 小（以免与临时端口号的 “正确” 范围冲突）
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    // backlog = 1024
    // 将套接字转换为一个监听套接字
    Listen(listenfd, LISTENQ);

    printf("Server starting at 127.0.0.1:9877\n");

    for (;;)
    {
        clilen = sizeof(cliaddr);

        // 服务器阻塞于 accept 调用
        // 等待客户连接的完成
        connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

        if ((childpid = Fork()) == 0)
        {
            /* child process */

            /* close listening socket */
            Close(listenfd);

            /* process the request */
            str_echo(connfd);

            /* closes connected socket */
            // 可选，子进程终止时，也会自动关闭
            Close(connfd);
            exit(0);
        }

        /* parent closes connected socket */
        Close(connfd);
    }
}
