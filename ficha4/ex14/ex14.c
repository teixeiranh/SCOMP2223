#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SHARED_MEMORY_SIZE 10
#define NUM_AX_PROCESSES 3
#define NUM_BX_PROCESSES 2

sem_t *semaphore_ax;
sem_t *semaphore_bx;
int shared_memory[SHARED_MEMORY_SIZE];

void print_shared_memory() {
    printf("Shared Memory: ");
    for (int i = 0; i < SHARED_MEMORY_SIZE; i++) {
        printf("%d ", shared_memory[i]);
    }
    printf("\n");
}

void ax_process(int process_id) {
    int n = 0;
    while (n<3) {
        sleep(5); // Wait for 5 seconds

        sem_wait(semaphore_ax);

        // Place data on the shared memory
        printf("AX%d is placing data on the shared memory.\n", process_id);
        for (int i = 0; i < SHARED_MEMORY_SIZE; i++) {
            shared_memory[i] = process_id * 100 + i;
        }

        print_shared_memory();

        sleep(1); 

        sem_post(semaphore_ax); // Signal other AX processes to access shared memory
        sem_post(semaphore_bx); // Release access to shared memory to BX processes
        n++;
    }
}

void bx_process(int process_id) {
    int n = 0;
    while (n<3) {
        sleep(6); // Wait for 6 seconds

        sem_wait(semaphore_bx);

        // Place data on the shared memory
        printf("BX%d is placing data on the shared memory.\n", process_id);
        for (int i = 0; i < SHARED_MEMORY_SIZE; i++) {
            shared_memory[i] = -process_id * 100 - i;
        }

        print_shared_memory();
        
        sleep(1); 

        sem_post(semaphore_bx); // Signal other BX processes to access shared memory
        sem_post(semaphore_ax); // Release access to shared memory to AX processes
        n++;
    }
}

int main() {
    semaphore_ax = sem_open("/semaphore_ax", O_CREAT, 0644, 1);
    semaphore_bx = sem_open("/semaphore_bx", O_CREAT, 0644, 0);

    // Create AX processes
    for (int i = 0; i < NUM_AX_PROCESSES; i++) {
        if (fork() == 0) {
            ax_process(i);
            exit(0);
        }
    }

    // Create BX processes
    for (int i = 0; i < NUM_BX_PROCESSES; i++) {
        if (fork() == 0) {
            bx_process(i);
            exit(0);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_AX_PROCESSES + NUM_BX_PROCESSES; i++) {
        wait(NULL);
    }

    // Close and unlink semaphores
    sem_close(semaphore_ax);
    sem_unlink("/semaphore_ax");
    sem_close(semaphore_bx);
    sem_unlink("/semaphore_bx");

    return 0;
}
