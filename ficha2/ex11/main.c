#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

#define FD_WRITE 1 // constant to hold the write file descriptor
#define FD_READ 0  // constant to hold the read file descriptor

/*
When using pipes, one only needs to properly close the used pipes' file descriptor, not all of them!
*/
int main(void)
{

	time_t t; // used to obtain different random numbers
	int num_pipes = 6;
	int num_child = 5;
	int i;

	int fd[num_pipes][2]; // array to store the file descriptors of all the pipes created

	// create all the 6 pipes necessary
	for (i = 0; i < num_pipes; i++)
	{
		if (pipe(fd[i]) == -1)
		{
			perror("Pipe error!");
			return (1);
		}
	}

	int previousNumber, generatedNumber; // variables to hold the previous and generated random number

	// for loop to traverse all the child processes and obtain the pretended result
	for (i = 0; i < num_child; i++)
	{
		pid_t pid = fork();

		if (pid == -1)
		{
			perror("Fork error!");
			return (1);
		}

		if (pid == 0) // child go here
		{
			close(fd[i][FD_WRITE]); // close the write file descriptor

			srand((unsigned)time(&t) ^ getpid()); // plant the seed

			generatedNumber = rand() % 500 + 1; // generate the random number

			read(fd[i][0], &previousNumber, sizeof(int)); // read from the ith pipe

			printf("Process: %d, generated number %d\n", getpid(), generatedNumber);

			close(fd[i][FD_READ]);	   // close the read file descriptor
			close(fd[i + 1][FD_READ]); // as we are going to write to the next pipe, close the read file descriptor

			if (generatedNumber > previousNumber)
			{
				write(fd[i + 1][FD_WRITE], &generatedNumber, sizeof(int));
			}
			else
			{
				write(fd[i + 1][FD_WRITE], &previousNumber, sizeof(int));
			}

			close(fd[i + 1][FD_WRITE]); // close the next pipe write file descriptor
			exit(0);					// exit the child process
		}
	}

	// parent process goes here
	close(fd[0][FD_READ]); // close the read file descriptor of the first pipe

	srand((unsigned)time(&t) ^ getpid()); // generate seed

	generatedNumber = rand() % 500 + 1; // generate random number

	printf("Process: %d, generated number  %d\n", getpid(), generatedNumber);

	write(fd[0][FD_WRITE], &generatedNumber, sizeof(int)); // send stream to the first pipe
	close(fd[0][FD_WRITE]);								   // close the write file descriptor

	close(fd[5][FD_WRITE]);								 // close the write file descriptor (using the read, so we need to close the write)
	read(fd[5][FD_READ], &generatedNumber, sizeof(int)); // read the last value
	close(fd[5][FD_READ]);								 // close the read file descriptor
	printf("The greatest random number is %d\n", generatedNumber);

	return 0;
}