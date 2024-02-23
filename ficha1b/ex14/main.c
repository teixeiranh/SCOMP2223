/* Ex14. 
Write a program that (1) reads a string from the keyboard and (2) displays the size of the string on
the screen. However, the user has 10 seconds to input the string. After this time limit, the program 
displays the message “You were too slow and this program will end!” and exits.

(a) Implement a solution that solves the problem with a single process.
Suggestion: Use the alarm() function.
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <string.h>

// volatile sig_atomic_t timeout = 0;

// void handle_ALRM(int signo) {
//     timeout = 1;
// }

// int main() {
//     char str[100];
//     int len;

//     struct sigaction act;
//     memset(&act, 0, sizeof(struct sigaction));
//     sigemptyset(&act.sa_mask);
//     act.sa_handler = handle_ALRM;

//     if (sigaction(SIGALRM, &act, NULL) == -1) {
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }

//     alarm(10);

//     // Read the string from the keyboard
//     printf("Enter a string (you have 10 seconds):\n");
//     fgets(str, 100, stdin);

//     if (timeout) {
//         printf("You were too slow and this program will end!\n");
//     }

//     len = strlen(str) - 1;  // Exclude the newline character

//     printf("The size of the string is: %d\n", len);
    
//     return 0;
// }

/*
make run
./ex14
Enter a string (you have 10 seconds):
You were too slow and this program will end!

make run
./ex14
Enter a string (you have 10 seconds):
ASASASAS
The size of the string is: 8
*/


/*
(b) Implement a solution that uses two processes to manage the time limit
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define TIMEOUT 10

volatile sig_atomic_t timeout = 0;

void handle_ALRM(int signo) {
    timeout = 1;
}

int main() {
    char str[100];
    int len;

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_handler = handle_ALRM;

    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // Create child process to manage timeout
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        alarm(TIMEOUT);

        pause();

        if (timeout) {
            printf("You were too slow and this program will end!\n");
            kill(getppid(), SIGALRM);
        }
        exit(EXIT_SUCCESS);
    }

    // Read the string from the keyboard
    printf("Enter a string (you have 10 seconds):\n");
    fgets(str, 100, stdin);

    len = strlen(str) - 1;  // Exclude the newline character
    if (len >= 0) {
        printf("The size of the string is: %d\n", len);
    }
    
    kill(pid, SIGKILL);

    return 0;
}