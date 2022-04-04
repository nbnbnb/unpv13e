#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    // -1 表示等待所有的子进程
    // 任意一个终止了，就返回

    // WNOHANG 表示，如果没有子进程了
    // 则不阻塞
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("child %d terminated\n", pid);
    }

    // 约定，void 也设置 return 语句
    // 这样我们就可以得知该系统调用具体是被哪个信号处理函数的哪个 return 语句中断的
    return;
}
