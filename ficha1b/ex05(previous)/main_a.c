#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main()
{
    for (;;)
    {
        printf("I Like Signal\n");
        sleep(3); // Loop in a timely manner, don't blow the CPU!
    }
}