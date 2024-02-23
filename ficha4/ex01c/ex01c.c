#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define NUM_CHILDREN 8
#define NUM_NUMBERS 200
#define SEMAPHORE "semaphore"

void child_process(int child_num, sem_t *semaphores[NUM_CHILDREN]) {
    FILE *input = fopen("Numbers.txt", "r");
    FILE *output = fopen("Output.txt", "a");

    int number;

    for (int i = 0; i < NUM_NUMBERS; i++) {
        // Wait for current child process semaphore
        sem_wait(semaphores[child_num]);

        // Read number from input
        fscanf(input, "%d", &number);

        // Write number and PID to output
        fprintf(output, "[%d] %d\n", getpid(), number);

		// forcing any unwritten data in the output file stream's 
		// buffer to be written to the file immediately
		fflush(output);

        // Signal next child process to continue
        sem_post(semaphores[(child_num + 1) % NUM_CHILDREN]);
    }

    fclose(input);
    fclose(output);
}

int main() {
    sem_t *semaphores[NUM_CHILDREN];

    // Initialize semaphores
    for (int i = 0; i < NUM_CHILDREN; i++) {
        semaphores[i] = sem_open(SEMAPHORE+i, O_CREAT, 0664, (i == 0) ? 1 : 0);
    }

    // Remove previous version of Output.txt
    remove("Output.txt");

    // Create child processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            child_process(i, semaphores);
            exit(0);
        }
    }

    // Wait for child processes to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Close and unlink semaphores
    for (int i = 0; i < NUM_CHILDREN; i++) {
        sem_close(semaphores[i]);
        sem_unlink(SEMAPHORE+i);
    }

    return 0;
}
