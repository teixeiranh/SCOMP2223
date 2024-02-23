#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void handle_sigint(int sig)
{
    printf("I won't let the process end with CTRL-C!\n");
}

void handle_sigquit(int sig)
{
    printf("I won't let the process end by pressing CTRL-\\!\n");
}

int main()
{
    signal(SIGINT, handle_sigint);   // Set up signal handler for SIGINT
    signal(SIGQUIT, handle_sigquit); // Set up signal handler for SIGQUIT

    for (;;)
    {
        printf("I Like Signal\n");
        sleep(1); // Loop in a timely manner, don't blow the CPU!
    }
}
