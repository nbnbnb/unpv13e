#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

// 设置最大客户端数量
// 有了 poll 就不再有数量限制的问题了
// 因为分配一个 pollfd 结构数组并把该数组元素的数目通知内核成了调用者的责任
// 内核不再需要知道类似 fd_set 的固定大小的数据类型
#define OPEN_MAX 1024

#define NOTDEF

int main(int argc, char **argv)
{
    int i, maxi, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 9877 端口
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    // 第一项设置为 listenfd
    client[0].fd = listenfd;
    // 第一项设置 POLLRDNORM 事件，这样当有新的连接准备好被接受时
    // poll 将通知我们
    client[0].events = POLLRDNORM;

    // client 数组，记录 "已连接描述符"
    // 将其初始化为 -1
    for (i = 1; i < OPEN_MAX; i++)
    {
        /* -1 indicates available entry */
        client[i].fd = -1;
    }

    // maxi 变量含有 client 数组当前正在使用的最大下标值
    /* max index into client[] array */
    maxi = 0;

    for (;;)
    {
        // 调用 poll 函数
        // 返回就绪的个数
        nready = Poll(client, maxi + 1, INFTIM);

        // 如果从 poll 返回
        // 则表示有事件响应

        // 首先判断是否为 listenfd 的事件
        // 检测条件是判断 POLLRDNORM 位是否被设置
        if (client[0].revents & POLLRDNORM)
        {
            /* new client connection */
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

#ifdef NOTDEF           
            printf("new client: %s, port %d\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL), ntohs(cliaddr.sin_port));
#endif

            // 客户端数组的大小就是 OPEN_MAX
            for (i = 1; i < OPEN_MAX; i++)
            {
                // 如果有一个槽位（-1）
                if (client[i].fd < 0)
                {
                    /* save descriptor */
                    // 将 "已连接描述符" 保存到客户端数组中
                    client[i].fd = connfd;
                    break;
                }
            }

            // 如果没有槽位（退出）
            if (i == OPEN_MAX)
            {
                err_quit("too many clients");
            }

            // 设置需要监听的事件
            client[i].events = POLLRDNORM;

            // 计算 client 中的最大索引
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
        for (i = 1; i <= maxi; i++)
        {
            /* check all clients for data */
            // 如果是 -1，则继续迭代
            if ((sockfd = client[i].fd) < 0)
            {
                continue;
            }

            // 此处检查两个返回事件 POLLRDNORM | POLLERR

            // 我们检查 POLLERR 的原因在于：
            // 有些实现在一个连接上收到 RST 时返回的是 POLLERR 事件，而其他实现返回的只是 POLLRDNORM 事件
            // 无论哪种情形，我们都调用 read
            if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                // 将 sockfd 中的数据读入 buf 中
                if ((n = read(sockfd, buf, MAXLINE)) < 0)
                {
                    // 当一个现有的连接，由它的客户端终止时
                    // 我们就把它的 fd 成员设置为 -1
                    if (errno == ECONNRESET)
                    {
                        /*4connection reset by client */
#ifdef NOTDEF
                        printf("client[%d] aborted connection\n", i);
#endif
                        // 服务器端关闭
                        Close(sockfd);
                        // 从 client 中清除
                        client[i].fd = -1;
                    }
                    else
                    {
                        // 其它未知错误
                        // 关闭服务器
                        err_sys("read error");
                    }
                }
                else if (n == 0)
                {
                    // 如果 EOF

                    /*4connection closed by client */
#ifdef NOTDEF
                    printf("client[%d] closed connection\n", i);
#endif
                    // 服务器端关闭
                    Close(sockfd);
                    // 从 client 中清除
                    client[i].fd = -1;
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
