#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/*
Function to handle SIGUSR1
*/
void handle_USR1(int sigNo)
{
    /*
    Don't use printf, it is not safe!
    */
    printf("SIGUSR1 has been captured and was sent by the process with PID %d\n",
           getpid());
}

int main()
{
    // set the signal handler
    signal(SIGUSR1, handle_USR1);

    for (;;)
    {
        sleep(1); // use sleep here to avoid the capture of CPU resources by the infinite loop
    }
}