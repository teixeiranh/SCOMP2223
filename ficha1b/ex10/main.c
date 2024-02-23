/* Ex10
Write a program that executes an infinite loop. For each iteration of the loop, the program displays
the message “I’m working!” and calls the sleep() function to pause for one second.

(a) Add to your program the handle_USR1() function to handle the SIGUSR1 signal. This function
increments the global USR1˙counter global variable and displays the message “SIGUSR1 signal
captured: USR1 counter = XX”.
Note: The variable should be declared as volatile sig_atomic_t USR1_counter.
• The volatile keyword indicates the compiler that the variable may be changed asynchronously,
so any code optimisation on this variable must be avoided.
• The sig_atomic_t type is an integer that is loaded/stored from/to memory in a single instruction.
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <string.h>

// volatile sig_atomic_t USR1_counter = 0;

// void handle_USR1(int signo) {
//     USR1_counter++;
//     char message[44];
//     sprintf(message, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
//     write(STDOUT_FILENO, message, sizeof(message));
// }

// int main() {
//     struct sigaction act;
//     memset(&act, 0, sizeof(struct sigaction));
//     act.sa_handler = handle_USR1;
//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;

//     if (sigaction(SIGUSR1, &act, NULL) == -1) {
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }

//     printf("My PID is %d\n", getpid());

//     while(1) {
//         write(STDOUT_FILENO, "I'm working!\n", 13);
//         kill(getpid(), SIGUSR1);
//         sleep(1);
//     }
    
//     return 0;
// }

/*
(b) Set your program to block all signals whenever the SIGUSR1 is handled, i.e. no other signal (not
even the SIGUSR1 signal) can interrupt handle˙USR1() during its execution.
Compile and test your program, sending signals from another terminal.
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <string.h>

// volatile sig_atomic_t USR1_counter = 0;

// void handle_USR1(int signo) {
//     USR1_counter++;
//     char message[44];
//     sprintf(message, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
//     write(STDOUT_FILENO, message, sizeof(message));
//     sleep(5); // Added the longer sleep to make sure the process was not interrupted while handling the signal
// }

// int main() {
//     struct sigaction act;
//     memset(&act, 0, sizeof(struct sigaction));
//     act.sa_handler = handle_USR1;
//     sigfillset(&act.sa_mask); //Todos os sinais são bloqueados durante a execução do sigaction
//     act.sa_flags = SA_SIGINFO;

//     if (sigaction(SIGUSR1, &act, NULL) == -1) {
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }

//     printf("My PID is %d\n", getpid());

//     while(1) {
//         write(STDOUT_FILENO, "I'm working!\n", 13);
//         kill(getpid(), SIGUSR1);
//         sleep(1);
//     }
    
//     return 0;
// }

/*
(c) Modify your program to create a child process before entering the infinite loop. The child sends
to his parent a burst of 12 SIGUSR1s with a 10 ms interval between consecutive signals; finally,
the child sends a SIGINT.
Test this version of your program.
(Suggestion: Check the nanosleep() function manual page.)
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <string.h>
// #include <time.h>

// volatile sig_atomic_t USR1_counter = 0;

// void handle_USR1(int signo) {
//     USR1_counter++;
//     char message[44];
//     sprintf(message, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
//     write(STDOUT_FILENO, message, sizeof(message));
// }

// int main() {
//     struct sigaction act;
//     memset(&act, 0, sizeof(struct sigaction));
//     act.sa_handler = handle_USR1;
//     sigemptyset(&act.sa_mask); //Todos os sinais são bloqueados durante a execução do sigaction
//     act.sa_flags = SA_SIGINFO;

//     if (sigaction(SIGUSR1, &act, NULL) == -1) {
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }

//     printf("My PID is %d\n", getpid());

//     // Create child process
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         exit(EXIT_FAILURE);
//     } 
//     if (pid == 0) { // child process
//         for (int i = 0; i < 12; i++) {
//             kill(getppid(), SIGUSR1); // send signal to parent
//             struct timespec ts;
//             ts.tv_sec = 0;
//             ts.tv_nsec = 10000000; // 10 ms
//             nanosleep(&ts, NULL); // sleep for 10 ms
//         }
//         kill(getppid(), SIGINT); // send SIGINT to parent
//         exit(EXIT_SUCCESS);
//     } else { // parent process
//         while (1) {
//             printf("I'm working!\n");
//             sleep(1);
//         }
//     }
//     return 0;
// }

/*
make run
gcc -Wall -g -c main.c
gcc -Wall -g -o ex10 main.o
./ex10
My PID is 25907
I'm working!
SIGUSR1 signal captured: USR1 counter = 1
�I'm working!
SIGUSR1 signal captured: USR1 counter = 2
�I'm working!
SIGUSR1 signal captured: USR1 counter = 3
�I'm working!
SIGUSR1 signal captured: USR1 counter = 4
�I'm working!
SIGUSR1 signal captured: USR1 counter = 5
�I'm working!
SIGUSR1 signal captured: USR1 counter = 6
�I'm working!
SIGUSR1 signal captured: USR1 counter = 7
�I'm working!
SIGUSR1 signal captured: USR1 counter = 8
�I'm working!
SIGUSR1 signal captured: USR1 counter = 9
�I'm working!
SIGUSR1 signal captured: USR1 counter = 10
I'm working!
SIGUSR1 signal captured: USR1 counter = 11
I'm working!
SIGUSR1 signal captured: USR1 counter = 12
I'm working!
make: *** [makefile:22: run] Interrupt
*/


/*
(d) Change the SIGUSR1 handler to take more than 1 second to be executed.
Recompile, run the program and check for differences.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo) {
    USR1_counter++;
    char message[44];
    sprintf(message, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
    write(STDOUT_FILENO, message, sizeof(message));
}

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_USR1;
    sigemptyset(&act.sa_mask); //Todos os sinais são bloqueados durante a execução do sigaction
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("My PID is %d\n", getpid());

    // Create child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    if (pid == 0) { // child process
        for (int i = 0; i < 12; i++) {
            kill(getppid(), SIGUSR1); // send signal to parent
            sleep(2);
        }
        kill(getppid(), SIGINT); // send SIGINT to parent
        exit(EXIT_SUCCESS);
    } else { // parent process
        while (1) {
            printf("I'm working!\n");
            sleep(1);
        }
    }
    return 0;
}

/*
The parent process body runs twice while the child process body runs once
because of the sleep(1) and sleep(2), respectively.

gcc -Wall -g -c main.c
gcc -Wall -g -o ex10 main.o
./ex10
My PID is 26672
I'm working!
SIGUSR1 signal captured: USR1 counter = 1
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 2
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 3
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 4
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 5
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 6
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 7
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 8
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 9
�I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 10
I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 11
I'm working!
I'm working!
SIGUSR1 signal captured: USR1 counter = 12
I'm working!
I'm working!
make: *** [makefile:22: run] Interrupt
*/