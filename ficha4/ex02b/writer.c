#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void write_to_shared_memory(SharedMemory* shared_memory) {
    pid_t pid = getpid();
    char line[MAX_STRING_LENGTH];
    snprintf(line, MAX_STRING_LENGTH, "I'm the Father - with PID %d", pid);

    while (1) {
        sem_wait(&shared_memory->sem_writer); // Wait for writer semaphore

        if (shared_memory->current_lines == SHARED_MEM_SIZE) {
            sem_post(&shared_memory->sem_reader); // Signal reader semaphore and exit if shared memory is full
            break;
        }

        strncpy(shared_memory->lines[shared_memory->current_lines], line, MAX_STRING_LENGTH);
        shared_memory->current_lines++;

        sem_post(&shared_memory->sem_reader); // Signal reader semaphore

        int sleep_time = rand() % 5 + 1;
        sleep(sleep_time);
    }
}

int main() {
    int fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedMemory));
    SharedMemory* shared_memory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // Initialize semaphores
    sem_init(&shared_memory->sem_writer, 1, 1);
    sem_init(&shared_memory->sem_reader, 1, 0);

    shared_memory->current_lines = 0;

    write_to_shared_memory(shared_memory);

    munmap(shared_memory, sizeof(SharedMemory));
    shm_unlink("/shared_memory");
    sem_destroy(&shared_memory->sem_writer);
    sem_destroy(&shared_memory->sem_reader);

    return 0;
}
