#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"
/* for OPEN_MAX */
#include <limits.h>

#define OPEN_MAX 100

int main(int argc, char **argv)
{
    int i, maxi, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;

    // 使用 pollfd 结构
    struct pollfd client[OPEN_MAX];

    struct sockaddr_in cliaddr, servaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;

    for (i = 1; i < OPEN_MAX; i++)
    {
        /* -1 indicates available entry */
        client[i].fd = -1;
    }

    /* max index into client[] array */
    maxi = 0;

    for (;;)
    {
        // 调用 poll 函数
        // 第一个参数是 pollfd
        // 第二个参数是结构数组中元素的个数
        // 第三个参数指定 poll 函数返回前等待多长 时间（毫秒）
        nready = Poll(client, maxi + 1, INFTIM);

        if (client[0].revents & POLLRDNORM)
        {
            /* new client connection */
            clilen = sizeof(cliaddr);

            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

#ifdef NOTDEF
            printf("new client: %s\n", Sock_ntop((SA *)&cliaddr, clilen));
#endif

            for (i = 1; i < OPEN_MAX; i++)
            {
                if (client[i].fd < 0)
                {
                    /* save descriptor */
                    client[i].fd = connfd;

                    break;
                }
            }

            if (i == OPEN_MAX)
            {
                err_quit("too many clients");
            }

            client[i].events = POLLRDNORM;

            if (i > maxi)
            {
                /* max index in client[] array */
                maxi = i;
            }

            if (--nready <= 0)
            {
                /* no more readable descriptors */
                continue;
            }
        }

        for (i = 1; i <= maxi; i++)
        {
            /* check all clients for data */
            if ((sockfd = client[i].fd) < 0)
            {
                continue;
            }

            if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                if ((n = read(sockfd, buf, MAXLINE)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        /*4connection reset by client */
#ifdef NOTDEF
                        printf("client[%d] aborted connection\n", i);
#endif
                        Close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                    {
                        err_sys("read error");
                    }
                }
                else if (n == 0)
                {
                    /*connection closed by client */
#ifdef NOTDEF
                    printf("client[%d] closed connection\n", i);
#endif
                    Close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    Writen(sockfd, buf, n);
                }

                if (--nready <= 0)
                {
                    /* no more readable descriptors */
                    break;
                }
            }
        }
    }
}
