#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

int main(int argc, char **argv)
{
    union
    {
        short s;
        char c[sizeof(short)];
    } un;

    // 存储 2 个字节
    un.s = 0x0102;

    printf("%s: ", CPU_VENDOR_OS);

    if (sizeof(short) == 2)
    {

        if (un.c[0] == 1 && un.c[1] == 2)
        {
            // 起始是高位
            // 结束是低位

            // 与声明时一致
            // 大端
            printf("big-endian\n");
        }
        else if (un.c[0] == 2 && un.c[1] == 1)
        {
            // Linxu 使用

            // 起始是低位
            // 结束是高位

            // 与声明时相反
            // 小端
            printf("little-endian\n");
        }
        else
        {
            printf("unknown\n");
        }
    }
    else
    {
        printf("sizeof(short) = %d\n", sizeof(short));
    }

    exit(0);
}