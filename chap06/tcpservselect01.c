#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
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

    // 将 "监听描述符" 添加到 select
    FD_SET(listenfd, &allset);

    for (;;)
    {
        /* structure assignment */
        rset = allset;

        // 开始 select
        // 返回继续的个数
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        // 如果从 "监听描述符" 返回
        // 表示有客户端接入
        if (FD_ISSET(listenfd, &rset))
        {
            /* new client connection */
            // 表示新接入的客户端信息
            clilen = sizeof(cliaddr);

            // 调用 accept
            // 返回一个 "已连接描述符"
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

#ifdef NOTDEF
            printf("new client: %s, port %d\n",
                   Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
                   ntohs(cliaddr.sin_port));
#endif

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

            // 如果没有槽位
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
                // 开始读取

                // 如果 EOF
                if ((n = Read(sockfd, buf, MAXLINE)) == 0)
                {
                    /*connection closed by client */
                    // 服务器端关闭
                    Close(sockfd);

                    // 从 select 中清除
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
