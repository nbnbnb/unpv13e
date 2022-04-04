#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    // 等待一个子进程终止
    // 只有调用 wait 或者 waitpid 才不会有僵死进程
    pid = wait(&stat);

    printf("child %d terminated\n", pid);

    return;
}
