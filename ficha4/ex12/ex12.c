#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define BUFFER_SIZE 10
#define NUM_VALUES 30

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} CircularBuffer;

int main() {
    // Create shared memory for the circular buffer
    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(CircularBuffer));
    CircularBuffer *buffer = (CircularBuffer *)mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Create semaphore for mutual exclusion
    sem_t *mutex = sem_open("/mutex", O_CREAT | O_RDWR, 0666, 1);
    
    // Create semaphore for empty slots
    sem_t *empty = sem_open("/empty", O_CREAT | O_RDWR, 0666, 0);
    
    // Create semaphore for full slots
    sem_t *full = sem_open("/full", O_CREAT | O_RDWR, 0666, BUFFER_SIZE);

    sleep(1);

    // Fork the first producer
    pid_t producer1_pid = fork();
    if (producer1_pid == 0) {
        for (int i = 0; i < NUM_VALUES; i++) {
            // one slot filled;
            sem_wait(full);

            // a producer only writes after guaranteeing mutual exclusion;
            sem_wait(mutex);
            
            // Produce value
            buffer->buffer[buffer->in] = i;
            buffer->in = (buffer->in + 1) % BUFFER_SIZE;

            printf("Producer 1 produced: %d\n", i);

            // a producer only writes after guaranteeing mutual exclusion;
            sem_post(mutex);

            // one slot available for reading;
            sem_post(empty);
            
            sleep(1);
        }
        exit(0);
    }

    // Fork the second producer
    pid_t producer2_pid = fork();
    if (producer2_pid == 0) {
        for (int i = NUM_VALUES; i < NUM_VALUES * 2; i++) {
            // one slot filled;
            sem_wait(full);
            // a producer only writes after guaranteeing mutual exclusion;
            sem_wait(mutex);
            
            // Produce value
            buffer->buffer[buffer->in] = i;
            buffer->in = (buffer->in + 1) % BUFFER_SIZE;

            printf("Producer 2 produced: %d\n", i);

            // a producer only writes after guaranteeing mutual exclusion;
            sem_post(mutex);

            // one slot available for reading;
            sem_post(empty);
            
            sleep(1);
        }
        exit(0);
    }

    // Consumer process
    for (int i = 0; i < NUM_VALUES * 2; i++) {
        // gets one slot to print
        sem_wait(empty);
        // sem_wait(mutex);
        
        // Consume value
        int value = buffer->buffer[buffer->out];
        buffer->out = (buffer->out + 1) % BUFFER_SIZE;

        printf("Consumer consumed: %d\n", value);
        
        // sem_post(mutex);
        // releases one slot
        sem_post(full);
        
        sleep(2);
    }

    // Clean up resources
    waitpid(producer1_pid, NULL, 0);
    waitpid(producer2_pid, NULL, 0);
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);
    sem_unlink("/mutex");
    sem_unlink("/empty");
    sem_unlink("/full");
    shm_unlink("/shared_memory");

    return 0;
}
