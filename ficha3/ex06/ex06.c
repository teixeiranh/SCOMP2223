#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SHM_NAME "/myshm"
#define SHM_SIZE sizeof(int)*LOCAL_MAX_SIZE
#define ARRAY_SIZE 1000
#define SUBARRAY_SIZE 100
#define LOCAL_MAX_SIZE 10

int main() {
    int global_array[ARRAY_SIZE];
    int i, j, max;
    pid_t pid;
    int *local_max_shm;

    // Initialize array of 1000 integers with random values between 0 and 1000
    srand(getpid());
    for (i = 0; i < ARRAY_SIZE; i++) {
        global_array[i] = rand() % 1000;
    }

    // Create shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        perror("Error creating shared memory object");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("Error setting the size of the shared memory object");
        exit(EXIT_FAILURE);
    }

    // Create shared memory area for local maximums
    local_max_shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (local_max_shm == MAP_FAILED) {
        perror("Error creating shared memory area");
        exit(EXIT_FAILURE);
    }

    // Create child processes
    for (i = 0; i < LOCAL_MAX_SIZE; i++) {
        pid = fork();

        if (pid == -1) {
            perror("Error creating child process");
            exit(EXIT_FAILURE);

        } else if (pid == 0) { // Child process
            // Using ranges of 100 in the 1000 integer array
            int local_max_value = global_array[i * SUBARRAY_SIZE];
            for (j = i * SUBARRAY_SIZE; j < (i + 1) * SUBARRAY_SIZE; j++) {
                // Replacing the local maximum
                if (global_array[j] > local_max_value) {
                    local_max_value = global_array[j];
                }
            }
            // Saving the local maximum in the shared memory object
            local_max_shm[i] = local_max_value;
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for child processes to finish
    for (i = 0; i < LOCAL_MAX_SIZE; i++) {
        wait(NULL);
    }

    // Determine global maximum
    max = local_max_shm[0];
    for (i = 1; i < LOCAL_MAX_SIZE; i++) {
        if (local_max_shm[i] > max) {
            max = local_max_shm[i];
        }
    }

    // Print result
    printf("The maximum element in the array is %d\n", max);

    // Cleanup shared memory area
    if (munmap(local_max_shm, SHM_SIZE) == -1) {
        perror("Error unmapping shared memory area");
        exit(EXIT_FAILURE);
    }

    // Close shared memory object
    if (close(fd) == -1) {
        perror("Error closing shared memory object");
        exit(EXIT_FAILURE);
    }

    // Unlink shared memory object
    if (shm_unlink(SHM_NAME) == -1) {
        perror("Error unlinking shared memory object");
        exit(EXIT_FAILURE);
    }

    return 0;
}

