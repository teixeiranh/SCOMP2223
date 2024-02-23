#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

#define FD_WRITE 1 // constant to hold the write file descriptor
#define FD_READ 0  // constant to hold the read file descriptor

int main(void)
{

	int numberOfMachines = 4;	 // number of machines
	int numberOfFlux = 4;		 // number of different flux of pieces used in this arrangement
	int totalNumOfPieces = 1000; // number of pieces to be stored
	int storageA1 = 0;
	int inventory = 0;
	int piecesPassed[4] = {5, 5, 10, 100};				// auxiliary array to store the maximum value per flux/pipe
	int notificationSend = 1, notificationFinished = 0; // boolean variables to know if it already sent the piece
	int i;

	int fd[numberOfFlux][2];			   // pipes for the flux
	int fd_notifications[numberOfFlux][2]; // pipes for the notification

	// create all the pipes necessary to "flux" the products
	for (i = 0; i < numberOfFlux; i++)
	{
		if (pipe(fd[i]) == -1)
		{
			perror("Pipe failed!");
			return (1);
		}
	}

	// create all the pipes necessary to allow the other processes to pass to the next one
	for (i = 0; i < numberOfFlux; i++)
	{
		if (pipe(fd_notifications[i]) == -1)
		{
			perror("Pipe failed");
			return (1);
		}
	}

	printf("Number of pieces to produce: %d\n", totalNumOfPieces);

	pid_t pid = fork();

	// for the process
	if (pid == -1)
	{
		perror("Fork erro!");
		return (1);
	}

	// process "M1" comes here
	if (pid == 0)
	{

		close(fd[0][FD_READ]);				 // close the file descriptor
		close(fd_notifications[0][FD_READ]); // close the file descriptor

		while (totalNumOfPieces != 0) // this process will be sending until we hit the final mark
		{
			write(fd[0][FD_WRITE], &piecesPassed[0], sizeof(piecesPassed[0]));				   // write in the flux
			totalNumOfPieces -= 5;															   // number of pieces to be used in the first pipe
			write(fd_notifications[0][FD_WRITE], &notificationSend, sizeof(notificationSend)); // write in the notification
			printf("%d pieces sent from M1 to M2!\n", piecesPassed[0]);
		}

		// send the notification to warn the next process that we have finished
		write(fd_notifications[0][FD_WRITE], &notificationFinished, sizeof(notificationFinished));

		close(fd[0][FD_WRITE]);				  // close the file descriptor
		close(fd_notifications[0][FD_WRITE]); // close the file descriptor

		exit(0); // exit process M1
	}

	// parent comes here
	for (i = 1; i < numberOfMachines; i++)
	{
		pid_t machinePID = fork();

		// for the process
		if (machinePID == -1)
		{
			perror("Fork erro!");
			return (1);
		}

		// child comes here
		if (machinePID == 0)
		{

			close(fd[i][FD_READ]);				 // close the file descriptor
			close(fd_notifications[i][FD_READ]); // close the file descriptor

			int piecesReceived, notificationReceived, piecesToSend = 0; // auxiliary variables

			read(fd_notifications[i - 1][FD_READ], &notificationReceived, sizeof(int)); // read from the previous notification (check if sent)

			while (notificationReceived != 0) // while items are being sent
			{
				if (notificationReceived == 1)
				{
					read(fd[i - 1][FD_READ], &piecesReceived, sizeof(int)); // read the received pieces
					piecesToSend += piecesReceived;							// accumulate pieces for this process
					if (piecesToSend == piecesPassed[i])					// if we already have all the necessary quantities
					{
						write(fd[i][1], &piecesPassed[i], sizeof(piecesPassed[i]));					// send the pieces to the next stop
						write(fd_notifications[i][1], &notificationSend, sizeof(notificationSend)); // send the notification warning next process
						if (i < numberOfMachines - 1)
						{
							printf("%d pieces sent from machine M%d to machine M%d!\n", piecesPassed[i], i + 1, i + 2);
						}
						else
						{
							printf("%d pieces sent from machine M%d to storage A1!\n", piecesPassed[i], i + 1);
						}

						piecesToSend = 0; // restart the counter
					}
				}
				read(fd_notifications[i - 1][FD_READ], &notificationReceived, sizeof(int)); // read the notification received
			}

			write(fd_notifications[i][FD_WRITE], &notificationFinished, sizeof(notificationFinished)); // notify finished

			close(fd[i][FD_WRITE]);				  // close the file descriptor
			close(fd_notifications[i][FD_WRITE]); // close the file descriptor

			exit(0); // exit the process
		}
	}

	// parent

	int piecesReceived, notificationReceived;

	read(fd_notifications[3][0], &notificationReceived, sizeof(int)); // read the last pipe

	while (notificationReceived != 0) // while receiving pieces
	{
		if (notificationReceived == 1)
		{
			read(fd[3][0], &piecesReceived, sizeof(int));
			if (piecesReceived == piecesPassed[3])
			{
				storageA1 += piecesReceived;
				inventory += piecesReceived;
			}
		}
		read(fd_notifications[3][0], &notificationReceived, sizeof(int)); // read and update the notification
	}

	close(fd[3][1]);// close the file descriptor
	close(fd_notifications[3][1]);// close the file descriptor

	printf("Pieces in storage A1 is %d, and the number of  parts in the inventory is %d!\n", storageA1, inventory);

	return (0);
}