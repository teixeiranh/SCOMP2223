#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void handle_sigusr1(int sig)
{
    printf("Child received SIGUSR1 signal\n");
}

void execute_task_a()
{
    printf("Executing A...\n");
    sleep(3);
    printf("A: done!\n");
}

void execute_task_b()
{
    srand(time(NULL)); // time used to always get different numbers
    int duration = rand() % 5 + 1; // random number between 1 and five
    printf("Executing B for %d seconds...\n", duration);
    sleep(duration);
    printf("Task B: done!\n");
}

void execute_task_c()
{
    printf("Executing C...\n");
    sleep(1);
    printf("C: done!\n");
}

/* Main function to test the code. */
int main()
{
    pid_t pid;
    struct sigaction sa;

    // Create child process
    pid = fork();

    if (pid == -1) // check any error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child
    {
        sa.sa_handler = handle_sigusr1;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        execute_task_b();
        kill(getppid(), SIGUSR1);
        execute_task_c();
        printf("Job is complete!\n");
        exit(EXIT_SUCCESS);
    }
    else // parent
    {
        execute_task_a();
        printf("Sending SIGUSR1 signal...\n");
        kill(pid, SIGUSR1);
        wait(NULL); // wait for the child to complete
        printf("Job is complete!\n");
        exit(EXIT_SUCCESS);
    }

    return (0);
}
