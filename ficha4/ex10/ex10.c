#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TRAIN_CAPACITY 20
#define NUM_DOORS 3

void findAvailableDoor(sem_t** doors, int* door_number) {
    *door_number = -1;

    while (*door_number < 0) {
        for (int k = 0; k < NUM_DOORS; k++) {
            if (sem_trywait(doors[k]) == 0) {
                usleep(1000000);  // Simulate passenger entering or exiting the train
                sem_post(doors[k]); // Release the door    
                *door_number = k;
                break;
            }
        }
    }
}

int main() {

    sem_t* train_capacity;
    sem_t* doors[NUM_DOORS];

    // Create the semaphores
    train_capacity = sem_open("/train_capacity", O_CREAT, 0644, TRAIN_CAPACITY);
    if (train_capacity == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    for (int i = 0; i < NUM_DOORS; i++) {
        char door_name[20];
        sprintf(door_name, "/door_%d", i);
        doors[i] = sem_open(door_name, O_CREAT, 0644, 1);
        if (doors[i] == SEM_FAILED) {
            perror("sem_open");
            exit(1);
        }
    }

    int initial_passengers_in_train = 15;
    
    // Decrement train available capacity
    for (int i = 0; i < initial_passengers_in_train; i++) {
        sem_wait(train_capacity);
    }

    // Simulate the train's operation at station A
    printf("Train arrives at station A. Passengers in train: %d\n", initial_passengers_in_train);

    int passengers_leaving_at_station_A = 5;

    printf("Train arrives at station A. Passengers leaving train: %d\n", passengers_leaving_at_station_A);

    // Passengers leaving the train
    for (int i = 0; i < passengers_leaving_at_station_A; i++) {
        if (fork() == 0) {
            // Passenger process
            int passenger_id = getpid();

            int door_number;
            findAvailableDoor(doors, &door_number);

            sem_post(train_capacity);  // Increment train capacity

            printf("Passenger %d left the train through door %d.\n", passenger_id, door_number);

            exit(0);
        }
    }
    
    // Wait for all passenger processes to complete
    for (int i = 0; i < passengers_leaving_at_station_A; i++) {
        wait(NULL);
    }

    int passengers_waiting_at_station_A = 20;

    printf("Train arrives at station A. Passengers waiting train: %d\n", passengers_waiting_at_station_A);

    // Passengers waiting the train
    for (int i = 0; i < passengers_waiting_at_station_A; i++) {
        if (fork() == 0) {
            // Passenger process
            int passenger_id = getpid();
            
            if (sem_trywait(train_capacity) == 0) {
                int door_number;
                findAvailableDoor(doors, &door_number);

                printf("Passenger %d is inside the train through door %d \n", passenger_id, door_number);
                exit(0);
            }
            exit(0); 
        }
    }

    
    // Wait for all passenger processes to complete
    for (int i = 0; i < passengers_waiting_at_station_A; i++) {
        wait(NULL);
    }
    
    int passenger_id = getpid();
    // int free_capacity = sem_getvalue(train_capacity, &free_capacity);
    printf("Train departs from station A. Passengers in train: %d\n %d", TRAIN_CAPACITY, passenger_id);


    // Close and unlink the semaphores
    sem_close(train_capacity);
    sem_unlink("/train_capacity");

    for (int i = 0; i < NUM_DOORS; i++) {
        char door_name[20];
        sprintf(door_name, "/door_%d", i);
        sem_close(doors[i]);
        sem_unlink(door_name);
    }

    return 0;
}
