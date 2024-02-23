#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{
    int fd[2]; // file descriptor for the pipe
    pid_t pid; // process ID

    // create the pipe
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE); // returns an error status
    }

    // fork a child
    pid = fork();
    if (pid == -1) // check for any error in the fork
    {
        perror("fork");
        exit(EXIT_FAILURE); // returns an error status
    }
    else if (pid == 0) // child process
    {
        close(fd[1]); // close the write end of the pipe
        int parent_pid;
        read(fd[0], &parent_pid, sizeof(parent_pid)); // read the parent's PID from the pipe
        printf("Child process: Parent's PID is %d\n", parent_pid);
        close(fd[0]); // close the read end of the pipe
    }
    else // parent
    {
        close(fd[0]);              // close the read end of the pipe
        int parent_pid = getpid(); // get the parent's PID
        printf("Parent process: My PID is %d\n", parent_pid);
        write(fd[1], &parent_pid, sizeof(parent_pid)); // write the parent's PID to the pipe
        close(fd[1]);                                  // close the write end of the pipe
    }

    return 0;
}
