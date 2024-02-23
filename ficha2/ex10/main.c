// /* Ex10.
// Write a program that simulates a betting system. Assume that the child process starts the game with a credit of 20
// euros. The game has the following rules:
// a. The parent process generates a random number between 1 and 5;
// b. Then, it writes 1 in the pipe, notifying the child that it can make another bet, or 0, if the child’s credit
// ended;
// c. The child process reads the number sent by the parent process and makes a bet or terminates, accordingly.
// To make a bet, the child should generate a random number between 1 and 5 and send it to the parent
// process;
// d. The parent process waits for the child’s bet or by its termination, accordingly. If the child’s bet is right, its
// credit grows 10 euros. If the bet is wrong, its credit diminishes 5 euros;
// e. The parent process sends the new credit’s value to the child process. The child process prints it. 
// */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

void sigint_handler(int sig)
{
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int parent_to_child_pipe[2];
    int child_to_parent_pipe[2];
    int do_next_bet;
    int bet;
    int child_credit = 20;

    // Create pipes
    if (pipe(parent_to_child_pipe) == -1 || pipe(child_to_parent_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
        srand(12);  // setting seed for rand in child process

        while (do_next_bet != 0) {
            sleep(0.5);
            //write(parent_to_child_pipe[WRITE_END], &child_credit, sizeof(child_credit));
            // Read notification from parent process
            read(parent_to_child_pipe[READ_END], &do_next_bet, sizeof(do_next_bet));
            if (do_next_bet == 0) {
                break;
            } else {
                // Make a bet
                bet = (rand() % 5) + 1;
                printf("Child process: My bet is %d.\n", bet);
                
                // Send bet to parent process
                write(child_to_parent_pipe[WRITE_END], &bet, sizeof(bet));
                
                // Read new credit value from parent process
                read(parent_to_child_pipe[READ_END], &child_credit, sizeof(child_credit));
                printf("Child process: My new credit is %d.\n", child_credit);
                printf("\n");
            }
        }
        
        // Close pipes
        close(parent_to_child_pipe[READ_END]);
        close(child_to_parent_pipe[WRITE_END]);
        printf("Child process: Not enough credit. Game over.\n");
        kill(getppid(), SIGINT);
        exit(EXIT_SUCCESS);

    } else { // parent process
        int parent_number;
        srand(11); // setting seed for rand in parent process (different than that of child)

        while (do_next_bet != 0) {
            sleep(1);

            if (child_credit > 0) {
                do_next_bet = 1;

                // Generate random number
                parent_number = (rand() % 5) + 1;
                printf("Parent process: The number is %d.\n", parent_number);
            } else {
                do_next_bet = 0;
            }
            write(parent_to_child_pipe[WRITE_END], &do_next_bet, sizeof(do_next_bet));
            
            // Read child's bet
            read(child_to_parent_pipe[READ_END], &bet, sizeof(bet));
            
            if (bet == parent_number) { // child's bet is right
                child_credit += 10;
                printf("Parent process: Child wins! Credit increased to %d.\n", child_credit);
            } else { // child's bet is wrong
                child_credit -= 5;
                printf("Parent process: Child loses. Credit decreased to %d.\n", child_credit);
            }
            
            // Send new credit value to child process
            write(parent_to_child_pipe[WRITE_END], &child_credit, sizeof(child_credit));
        }

        // Close pipes
        close(child_to_parent_pipe[READ_END]);
        close(parent_to_child_pipe[WRITE_END]);
    }

    kill(pid, SIGKILL);

    return 0;
}

/*
gcc -Wall -g -c main.c
gcc -Wall -g -o ex10 main.o
./ex10
Parent process: The number is 4.
Child process: My bet is 1.
Parent process: Child loses. Credit decreased to 15.
Child process: My new credit is 15.

Parent process: The number is 5.
Child process: My bet is 5.
Parent process: Child wins! Credit increased to 25.
Child process: My new credit is 25.

Parent process: The number is 3.
Child process: My bet is 5.
Parent process: Child loses. Credit decreased to 20.
Child process: My new credit is 20.

Parent process: The number is 1.
Child process: My bet is 4.
Parent process: Child loses. Credit decreased to 15.
Child process: My new credit is 15.

Parent process: The number is 4.
Child process: My bet is 4.
Parent process: Child wins! Credit increased to 25.
Child process: My new credit is 25.

Parent process: The number is 1.
Child process: My bet is 1.
Parent process: Child wins! Credit increased to 35.
Child process: My new credit is 35.

Parent process: The number is 2.
Child process: My bet is 5.
Parent process: Child loses. Credit decreased to 30.
Child process: My new credit is 30.

Parent process: The number is 2.
Child process: My bet is 4.
Parent process: Child loses. Credit decreased to 25.
Child process: My new credit is 25.

Parent process: The number is 5.
Child process: My bet is 3.
Parent process: Child loses. Credit decreased to 20.
Child process: My new credit is 20.

Parent process: The number is 4.
Child process: My bet is 5.
Parent process: Child loses. Credit decreased to 15.
Child process: My new credit is 15.

Parent process: The number is 1.
Child process: My bet is 4.
Parent process: Child loses. Credit decreased to 10.
Child process: My new credit is 10.

Parent process: The number is 2.
Child process: My bet is 5.
Parent process: Child loses. Credit decreased to 5.
Child process: My new credit is 5.

Parent process: The number is 4.
Child process: My bet is 3.
Parent process: Child loses. Credit decreased to 0.
Child process: My new credit is 0.

Child process: Not enough credit. Game over.
*/