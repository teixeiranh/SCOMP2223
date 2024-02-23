#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

int main()
{
    int up_pipe[2], down_pipe[2];
    pid_t pid;
    char message[256];

    // create the pipes
    if (pipe(up_pipe) == -1 || pipe(down_pipe) == -1)
    {
        perror("pipes error!");
        exit(EXIT_FAILURE); // returns an error status
    }

    // fork a child
    pid = fork();
    if (pid == -1)
    {
        perror("fork error!");
        exit(EXIT_FAILURE); // returns an error status
    }

    if (pid == 0) // child process (client)
    {
        // close unused pipe ends
        close(up_pipe[0]);   // client writes from up_pipe[1]
        close(down_pipe[1]); // client reads from down_pipe[0]

        // read message from stdin and send it to the parent via "up_pipe"
        printf("Enter a message: ");
        fgets(message, 256, stdin);                  // read the user input (stdin) and store in the buffer
        write(up_pipe[1], message, strlen(message)); // write the message up

        // wait for server's response and print it out
        read(down_pipe[0], message, 256);
        printf("Server response: %s", message);

        // close the pipes and exit
        close(up_pipe[1]);
        close(down_pipe[0]);
    }
    else // server process (parent)
    {
        // close unused pipe ends
        close(up_pipe[1]);   // server reads from up_pipe[0]
        close(down_pipe[0]); // server writes to down_pipe[1]

        // read message from client via up_pipe and convert it
        // "Attempts to read from a pipe that is currently empty block until
        // at least one byte has been written to the pipe."
        read(up_pipe[0], message, 256); 
        for (int i = 0; i < strlen(message); i++)
        {
            if (islower(message[i]))
            {
                message[i] = toupper(message[i]);
            }
            else if (isupper(message[i]))
            {
                message[i] = tolower(message[i]);
            }
        }

        // send the converted message to client via down_pipe
        write(down_pipe[1], message, strlen(message));

        // close the pipes
        close(up_pipe[0]);
        close(down_pipe[1]);
        wait(NULL); // to avoid zombies
        printf("Child process %d has exited\n", pid);
    }

    return 0;
}
