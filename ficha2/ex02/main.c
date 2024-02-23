/* Ex2.
Write a program that creates a child process and establishes with it a communication channel through a pipe. The
parent process reads an integer and a string from stdin and sends both through the pipe. The child process should
read those values from the pipe and print them.

a. Solve the exercise by first sending an integer and then a string in two separate write operations;
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>

// #define BUFFER_SIZE 80

// int main() {
//     int fd[2]; // file descriptor array for pipe
//     pid_t pid; 

//     if (pipe(fd) == -1) { // error creating pipe
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     }

//     pid = fork(); // create child process

//     if (pid == -1) { // error creating child process
//         perror("fork");
//         exit(EXIT_FAILURE);
//     }

//     if (pid == 0) { // child process
//         close(fd[1]); // close unused write end of pipe

//         int num;
//         char str[BUFFER_SIZE];

//         read(fd[0], &num, sizeof(num)); // read integer from pipe
//         read(fd[0], str, BUFFER_SIZE); // read string from pipe

//         printf("Child process: Integer = %d, String = %s\n", num, str);

//         close(fd[0]); // close read end of pipe

//         exit(EXIT_SUCCESS);
//     }
//     else { // parent process
//         close(fd[0]); // close unused read end of pipe

//         int num;
//         char str[BUFFER_SIZE];

//         printf("Enter an integer: ");
//         scanf("%d", &num);

//         printf("Enter a string: ");
//         scanf("%s", str);

//         write(fd[1], &num, sizeof(num)); // write integer to pipe
//         write(fd[1], str, strlen(str) + 1); // write string to pipe

//         close(fd[1]); // close write end of pipe
//         wait(NULL);
//     }

//     return 0;
// }

/*
b. Solve the exercise by using a structure to send both values in one write operation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 80

typedef struct {
    int num;
    char str[BUFFER_SIZE];
} Values;

int main() {
    int fd[2]; // file descriptor array for pipe
    pid_t pid; 
    Values values;

    if (pipe(fd) == -1) { // error creating pipe
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork(); // create child process

    if (pid == -1) { // error creating child process
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process
        close(fd[1]); // close unused write end of pipe

        read(fd[0], &values, sizeof(values)); // read from pipe

        printf("Child process: Integer = %d, String = %s\n", values.num, values.str);

        close(fd[0]); // close read end of pipe

        exit(EXIT_SUCCESS);
    }
    else { // parent process
        close(fd[0]); // close unused read end of pipe

        printf("Enter an integer: ");
        scanf("%d", &values.num);

        printf("Enter a string: ");
        scanf("%s", values.str);

        write(fd[1], &values, sizeof(values)); // write to pipe

        close(fd[1]); // close write end of pipe
        wait(NULL);
    }

    return 0;
}