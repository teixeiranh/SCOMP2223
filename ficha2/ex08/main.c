/* Ex8.
Write a program that creates 10 child processes to play the game “Win the pipe!”. There must be only one pipe,
shared by all processes. The game’s rules are as follow:

a. The parent process fills, each 2 seconds, a structure with a message “Win” and the round’s number (1 to
10) and writes this data in the pipe;
b. Each of child processes is trying to read data from the pipe. The child that succeeds should print the
winning message and the round’s number, ending its execution with an exit value equal to the winning
round’s number;
c. The remaining child processes continue to try to read data from the pipe;
d. After all child processes terminate, the parent process should print the PID and the winning round of each
child.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 10
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

typedef struct {
    char message[4];
    int round;
} GameData;

int main() {
    GameData data;

    // Create pipe
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(fd[PIPE_WRITE_END]); // Close unused write end of pipe
            
            // Read from pipe
            while (read(fd[PIPE_READ_END], &data, sizeof(GameData)) == sizeof(GameData)) {
                printf("Child process %d: %s round %d\n", getpid(), data.message, data.round);
                exit(data.round); // Exit with winning round number
            }
            close(fd[PIPE_READ_END]);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process
    close(fd[PIPE_READ_END]); // Close unused read end of pipe

    for (int i = 1; i <= 10; i++) {
        sprintf(data.message, "Win");
        data.round = i;

        // Write to pipe
        if (write(fd[PIPE_WRITE_END], &data, sizeof(GameData)) != sizeof(GameData)) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        sleep(2); // Wait 2 seconds
    }
    close(fd[PIPE_WRITE_END]);

    // Wait for all child processes to terminate and print winning round
    int status;
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = wait(&status);
        printf("Child process with PID %d won round %d\n", pid, WEXITSTATUS(status));
    }
    
    exit(EXIT_SUCCESS);
    return 0;
}

/*
make run
gcc -Wall -g -c main.c
gcc -Wall -g -o ex08 main.o
./ex08
Child process 3136: Win round 1
Child process 3128: Win round 2
Child process 3130: Win round 3
Child process 3129: Win round 4
Child process 3131: Win round 5
Child process 3132: Win round 6
Child process 3133: Win round 7
Child process 3134: Win round 8
Child process 3135: Win round 9
Child process 3127: Win round 10
Child process with PID 3127 won round 10
Child process with PID 3128 won round 2
Child process with PID 3129 won round 4
Child process with PID 3130 won round 3
Child process with PID 3131 won round 5
Child process with PID 3132 won round 6
Child process with PID 3133 won round 7
Child process with PID 3134 won round 8
Child process with PID 3135 won round 9
Child process with PID 3136 won round 1
*/