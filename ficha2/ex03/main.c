#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd[2];      // file descriptors for the pipe
    pid_t pid;      // process pid
    char buf[1024]; // buffer to save the message

    // create the pipe
    if (pipe(fd) == -1)
    {
        perror("pipe error!");
        exit(EXIT_FAILURE); // returns an error status
    }

    // fork a child
    pid = fork();
    if (pid < 0)
    {
        perror("fork error!");
        exit(EXIT_FAILURE); // returns an error status
    }

    if (pid == 0) // child process code
    {
        close(fd[1]); // close the write end of the pipe

        // while loop to print the message received from parent process
        while (read(fd[0], buf, sizeof(buf)) > 0)
        {
            printf("%s", buf);
        }

        close(fd[0]); // close the read end of the pipe
    }
    else // parent process
    {
        close(fd[0]); // close the read end of the pipe

        char *message1 = "Hello World\n"; // message 1 to the child
        char *message2 = "Goodbye!\n";    // message 2 to the child
        write(fd[1], message1, strlen(message1));
        write(fd[1], message2, strlen(message2));

        close(fd[1]); // close the write end of the pipe

        int status;
        waitpid(pid, &status, 0); // wait for child to end
        printf("Child process with PID %d exited with status %d\n", pid, status);
    }

    return 0;
}
