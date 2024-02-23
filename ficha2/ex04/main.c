/* Ex4.
Write a program that creates a child process and establishes with it a communicating channel through a pipe. The
parent process should send the contents of a text file to its child through the pipe. The child should print all the
received data. The parent must wait for its child to end.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) { 
        // informs the user of the usage procedure if it fails to provide the correct number of arguments    
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]); 
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
        close(fd[1]); // close write end

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
            fwrite(buffer, sizeof(char), bytes_read, stdout);
        }

        close(fd[0]); // close read end
        exit(EXIT_SUCCESS);

    } else { // parent process
        close(fd[0]); // close read end

        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
            ssize_t bytes_written = write(fd[1], buffer, bytes_read);
            if (bytes_written != bytes_read) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        close(fd[1]); // close write end
        wait(NULL);

        exit(EXIT_SUCCESS);

        return 0;
    }
}

/*
after running "make" in termminal to build the executable file,
and after running "./ex04 problemas.txt" the terminal prints:
Exercícios não resolvidos:
==========================
-ex16


Exercícios com problemas:
=========================
-ex08- cant make main() reach the return 0, need to kill the execution in the terminal with CTRL-C
     - problem solved with adding kill(getpid(), SIGUSR1); after capturing sigusr1_handler
     - could have used exit(0) also

-ex12- The SA_NOCLDSTOP and SA_NOCLDWAIT flags are used to prevent the kernel 
       from automatically reaping the child processes when they exit or stop. 
       This allows the parent process to explicitly wait for them using wait().
       Is this information correct?

*/
