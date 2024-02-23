#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    sem_t *sem_child1, *sem_child2;

    // Create the semaphores
    sem_child1 = sem_open("/child1_semaphore", O_CREAT | O_EXCL, 0644, 0);
    sem_child2 = sem_open("/child2_semaphore", O_CREAT | O_EXCL, 0644, 0);

    if (sem_child1 == SEM_FAILED || sem_child2 == SEM_FAILED) {
        perror("Failed to create semaphores");
        exit(1);
    }
    
    
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        // Child process 1
        sem_wait(sem_child1);  // Wait for permission to proceed

        printf("1st child\n");

          
        exit(0);
    } else if (pid1 > 0) {
        pid2 = fork();
        if (pid2 == 0) {
            // Child process 2
            sem_wait(sem_child2);  // Wait for permission to proceed

            printf("2nd child\n");

            exit(0);
        } else if (pid2 > 0) {
            // Parent process
            sem_post(sem_child1);  // Notify child process 1
            wait(NULL);
            printf("Father\n");
            sem_post(sem_child2); // Notify child process 2
            wait(NULL);

            // Clean up semaphores
            sem_unlink("/child1_semaphore");
            sem_unlink("/child2_semaphore");
        } else {
            perror("Failed to create child process 2");
            exit(1);
        }
    } else {
        perror("Failed to create child process 1");
        exit(1);
    }

    return 0;
}
