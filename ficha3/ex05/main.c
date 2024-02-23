#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ITERATIONS 100
#define SHM_NAME "/shared_memory"

int *shared_value;

// Signal handler to toggle the value of *shared_value
void signal_handler(int signum)
{
    *shared_value = !*shared_value;
}

/* Main function to solve ex05 of PL3. */
int main()
{
    // FIXME:  This function is not working properly.
    // It blocks and we never know the actual final value.
    const int DATA_SIZE = sizeof(int);
    pid_t child_pid;

    // Create and initialize shared memory using POSIX API
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) // Check for errors
    {
        perror("shm_open error!");
        exit(1); // Exit with error
    }

    if (ftruncate(shm_fd, DATA_SIZE) == -1) // Truncate the file to the size of the data
    {
        perror("ftruncate error!");
        exit(1); // Exit with error
    }

    // Map the shared memory to the process
    shared_value = mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_value == MAP_FAILED)
    {
        perror("mmap error!");
        exit(1); // Exit with error
    }

    *shared_value = 100;

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);      // No signals blocked
    sigaction(SIGUSR1, &sa, NULL); // Set signal handler

    printf("-debug line - before fork -\n");
    // Fork a new process
    child_pid = fork();
    if (child_pid == -1) // Check for errors
    {
        perror("fork error!");
        exit(1); // Exit with error
    }

    printf("-debug line - before child -\n");
    if (child_pid == 0)
    { // Child process
        for (int i = 0; i < ITERATIONS; ++i)
        {
            // Wait for parent's signal to continue
            while (*shared_value)
            {
                usleep(1000); // Sleep for 1 ms
            }

            // Perform the required operations
            (*shared_value)++;
            (*shared_value)--;
            // Signal the parent to continue
            kill(getppid(), SIGUSR1);
        }
        exit(0);
    }
    else
    { // Parent process
        printf("-debug line - before parent -\n");
        for (int i = 0; i < ITERATIONS; ++i)
        {
            // Perform the required operations
            (*shared_value)++;
            (*shared_value)--;
            // Signal the child to continue
            kill(child_pid, SIGUSR1);

            // Wait for child's signal to continue
            while (!*shared_value)
            {
                usleep(1000); // Sleep for 1 ms
            }
        }

        wait(NULL); // Wait for the child process to finish
        printf("Final value: %d\n", *shared_value);

        // Unmap the shared memory
        munmap(shared_value, sizeof(int));
    }

    return 0;
}
