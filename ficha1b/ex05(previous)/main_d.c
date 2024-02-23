#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sigHandler(int sigNo)
{
    /*
    Don't use printf, it is not safe!
    */
    printf("MUAHAHAHAH I won't let the process end with CTRL-\\! MUAHAHAHAHAH\n");
}

int main(int argc, char *argv[])
{

    // in case of error
    if (signal(SIGQUIT, sigHandler) == SIG_ERR)
    {
        errExit("signal");
    }

    
    for (;;)
    {
        printf("I Like Signal yoo\n");
        sleep(3); // Loop in a timely manner, don't blow the CPU!
    }
}