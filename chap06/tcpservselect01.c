#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

#define NOTDEF

/**
 * @brief 测试
 *
 * 先启动服务端
 *
 * 然后执行客户端
 * /home/zhangjin/projects/unpv13e/chap05/tcpcli01.run
 *
 */

int main(int argc, char **argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    // FD_SETSIZE 是内核编译时也使用的一个常量，默认是 1024
    int nready, client[FD_SETSIZE];
    ssize_t n;
    // fd_set 结构是 select 需要的
    fd_set rset, allset;
    char buf[MAXLINE];
    char addr[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 9877 端口
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    /* initialize */
    maxfd = listenfd;

    /* index into client[] array */
    maxi = -1;

    // client 数组，记录 "已连接描述符"
    // 将其初始化为 -1
    for (i = 0; i < FD_SETSIZE; i++)
    {
        /* -1 indicates available entry */
        client[i] = -1;
    }

    // 初始化 fd_set
    FD_ZERO(&allset);

    // 将 listenfd 添加到 allset
    FD_SET(listenfd, &allset);

    for (;;)
    {
        /* structure assignment */
        // 由于传入 select 的 fd_set 会修改
        // 所以，此处复制一个
        rset = allset;

        // 开始 select
        // 返回就绪的个数
        // 同时 rset 也会被修改，标识那些位被设置了
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        // 如果从 select 返回
        // 则表示有事件响应

        // 首先判断是否为 listenfd 的事件
        // 通过 FD_ISSET 宏进行判断
        if (FD_ISSET(listenfd, &rset))
        {
            /* new client connection */
            // 表示新接入的客户端信息
            clilen = sizeof(cliaddr);

            // 调用 accept
            // 返回一个 "已连接描述符"
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

            // Inet_ntop 函数将套接字结构中的 32 位 IP 地址转换为一个点分十进制数 ASCII 字符串
            // ntohs 函数将 16 位的端口号从网络字节序转换为主机字节序
            printf("connection from %s, port %d\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, addr, sizeof(addr)), ntohs(cliaddr.sin_port));

            // 客户端数组的大小就是 FD_SETSIZE
            for (i = 0; i < FD_SETSIZE; i++)
            {
                // 如果有一个槽位（-1）
                if (client[i] < 0)
                {
                    /* save descriptor */
                    // 将 "已连接描述符" 保存到客户端数组中
                    client[i] = connfd;
                    break;
                }
            }

            // 如果没有槽位（退出）
            if (i == FD_SETSIZE)
            {
                err_quit("too many clients");
            }

            /* add new descriptor to set */
            // 将 "已连接描述符" 添加到 select
            FD_SET(connfd, &allset);

            // 计算最大描述符数
            if (connfd > maxfd)
            {
                /* for select */
                maxfd = connfd;
            }

            // 计算最大描述符对应在 client 中的索引
            if (i > maxi)
            {
                /* max index in client[] array */
                maxi = i;
            }

            // 这个地方是个关键点
            // 和代码位置有关系

            // 如果 listenfd 返回了，则表示使用了一个 nready
            // nready 减去 1 之后，如果位为 0 了，就表示没有 sockfd 返回了
            // 所以不用执行下面的 sockfd 迭代代码
            if (--nready <= 0)
            {
                /* no more readable descriptors */
                continue;
            }
        }

        // 迭代 client 中的每一项 sockfd
        for (i = 0; i <= maxi; i++)
        {
            /* check all clients for data */
            // 如果是 -1，则继续迭代
            if ((sockfd = client[i]) < 0)
            {
                continue;
            }

            // 如果 select 中返回的与 client 匹配
            if (FD_ISSET(sockfd, &rset))
            {
                // 将 sockfd 中的数据读入 buf 中
                if ((n = Read(sockfd, buf, MAXLINE)) == 0)
                {
                    // 如果 EOF

                    /*connection closed by client */
                    // 服务器端关闭
                    Close(sockfd);

                    // 从 allset 中清除
                    FD_CLR(sockfd, &allset);

                    // 从 client 中清除
                    client[i] = -1;
                }
                else
                {
                    // 从 sockfd 中读到的数据再次写到 sockfd
                    Writen(sockfd, buf, n);
                }

                // 没有可用 sockfd 了
                // client 停止迭代
                if (--nready <= 0)
                {
                    /* no more readable descriptors */
                    break;
                }
            }
        }
    }
}
