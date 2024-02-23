/* Ex12
Write a program that meets the following requirements.
• The program starts by generating five children processes.
Each child i ∈ {0, 1, 2, 3, 4} prints on the screen the sequence of all integer values in the interval
[i × 200, (i + 5) × 200[.
Once the job is complete, the child notifies the parent with a SIGUSR1 before exiting.
• The parent process enters a loop executing the pause() function.
The parent updates a variable – of type volatile sig_atomic_t – that counts how many children
are still executing.
The parent exits the loop when no more children are executing. Only then, the parent calls the
wait()/waitpid() function five times.
Suggestion: The SA_NOCLDWAIT and SA_NOCLDSTOP flags will be useful for solving this problem
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

volatile sig_atomic_t num_children = 5;

void handle_child_exit(int signo) {
    char message[53];
    sprintf(message, "SIGUSR1 signal %d captured from child with pid %d.\n", num_children, getpid());
    write(STDOUT_FILENO, message, sizeof(message));

    num_children--;
}

void child_process(int i) {
    int start =  i * 200;
    int end = (i + 5) * 200;

    printf("\n");
    for (int i = start; i < end; i++) {
        printf("%d ", i);
    }
    printf("\n");
    
    printf("Child number %d with pid = %d\n", num_children, getpid());
    
    // Notify parent process that child is done
    kill(getppid(), SIGUSR1);
   
    exit(0);
}

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_child_exit;

    sigfillset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Generate five children processes
    for (int i = 0; i < 5; i++) {
        sleep(1);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            child_process(i);
        }
    }

    // Wait for all children to finish
    while (num_children > 0) {
        pause();
    } 

    // Wait for all children to terminate
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }
    
    return 0;
}