#include "/home/zhangjin/projects/unpv13e/src/lib/unp.h"

void temp(void);

int main(int argc, char *argv[])
{
    printf("Compiled on: %s %s\n", __DATE__, __TIME__);
    printf("------ Start ------\n");
    temp();
    printf("------  End  ------\n");

    return 0;
}

void temp(void)
{
}
