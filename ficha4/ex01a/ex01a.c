#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define NUM_CHILDS 8
#define NUMBER_COUNT 200
#define SEMAPHORE "semaphore"
#define FILE_OPEN "Numbers.txt"
#define FILE_OUT "Output.txt"

int main(int argc, char* argv[]) {
	 
	//delete any previous version of the file Output.txt
	remove(FILE_OUT);

	// Initializing semaphore with mutual exclusion accessing shared resources
	sem_t* sem = sem_open(SEMAPHORE, O_CREAT | O_EXCL, 0644, 1);
	if (sem == SEM_FAILED) {
		perror("sem_open() failed");
		exit(EXIT_FAILURE);
	}

	// Create 8 new processes
    for (int i = 0; i < NUM_CHILDS; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Failed to create child process");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            FILE* input_file = fopen(FILE_OPEN, "r");
			FILE* output_file = fopen(FILE_OUT, "a");

			if (input_file == NULL || output_file == NULL) {
				perror("fopen() failed");
				exit(EXIT_FAILURE);
			}

			int number;
			
			sem_wait(sem);

			//reading from input
			fscanf(input_file, "%d", &number);
			
			//writing to output
			fprintf(output_file, "[%d] %d\n", getpid(), number);
			
			//End critical section
			sem_post(sem);
			
			
			//close files
			fclose(input_file);
			fclose(output_file);
            exit(EXIT_SUCCESS);
        }
    }

	for (int i = 0; i < NUM_CHILDS; i++) {
        wait(NULL);
    }

	// Clean up semaphore
	if(sem_close(sem) == -1) {
		perror("sem_close() failed");
		exit(EXIT_FAILURE);
	}
	
	if (sem_unlink(SEMAPHORE) < 0) {
		perror("sem_unlink() failed");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

