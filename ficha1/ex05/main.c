#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pidProcess1;
    pid_t pidProcess2;
    int statusProcess1;
    int statusProcess2;

    pidProcess1 = fork(); // fork the first child process

    // check if process was properly created
    if (pidProcess1 < 0)
    {
        perror("Error creating the processes!\n");
        exit(-1);
    }

    if (pidProcess1 == 0)
    {
        sleep(1); // set process1 to sleep
        exit(1);
    }
    else // parent code is running now
    {
        pidProcess2 = fork(); // fork the second child process

        if (pidProcess2 < 0)
        {
            perror("Error creating the processes!\n");
            exit(1);
        }

        if (pidProcess2 == 0)
        {
            sleep(2); // set process2 to sleep
            exit(2);
        }
        else // finally the parent code gets to process the children code
        {
            waitpid(pidProcess1, &statusProcess1, 0);
            waitpid(pidProcess2, &statusProcess2, 0);
            // here we will print the exit status  set in  process1 and process2
            printf("Child 1 exited, status: %d\n", WEXITSTATUS(statusProcess1));
            printf("Child 2 exited, status: %d\n", WEXITSTATUS(statusProcess2));
        }
    }

    return 0;
};
