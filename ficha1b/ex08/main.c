/* Ex08 
Write a program that when the SIGUSR1 signal is received, the message “I captured a SIGUSR1 sent
by the process with PID XX”, where XX is the PID of the process which sent the SIGUSR1 signal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigusr1_handler(int signo) {
    char message[63];
    sprintf(message, "I captured a SIGUSR1 sent by the process with PID %d\n", getpid());
    write(STDOUT_FILENO, message, sizeof(message));
}

int main() {
    struct sigaction act;
    act.sa_handler = sigusr1_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("My PID is %d\n", getpid());
    
    printf("Waiting for SIGUSR1...\n");
    pause();
    kill(getpid(), SIGUSR1);
    
    return 0;
}

/*
First terminal after make run:
./ex08
My PID is 28066
Waiting for SIGUSR1...

In another terminal:
kill -SIGUSR1 28066

First terminal prints:
I captured a SIGUSR1 sent by the process with PID 28066
*/