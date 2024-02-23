#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHARED_MEM_SIZE 50
#define MAX_STRING_LENGTH 80

typedef struct {
    char lines[SHARED_MEM_SIZE][MAX_STRING_LENGTH];
    int current_lines;
    sem_t sem_writer; // Semaphore for writer
    sem_t sem_reader; // Semaphore for reader
} SharedMemory;

void read_shared_memory(SharedMemory* shared_memory) {
    while (1) {
        sem_wait(&shared_memory->sem_reader); // Wait for reader semaphore

        if (shared_memory->current_lines == 0) {
            sem_post(&shared_memory->sem_writer); // Signal writer semaphore and exit if shared memory is empty
            break;
        }

        printf("Current lines: %d\n", shared_memory->current_lines);
        for (int i = 0; i < shared_memory->current_lines; i++) {
            printf("%s\n", shared_memory->lines[i]);
        }

        sem_post(&shared_memory->sem_writer); // Signal writer semaphore

        sleep(1);
    }
}

int main() {
    int fd = shm_open("/shared_memory", O_RDWR, 0666);
    SharedMemory* shared_memory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    read_shared_memory(shared_memory);

    munmap(shared_memory, sizeof(SharedMemory));

    return 0;
}
