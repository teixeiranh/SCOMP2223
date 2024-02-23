#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SEMAPHORE_BEER "/semaphore_beer"
#define SEMAPHORE_CHIPS "/semaphore_chips"

/* Method to simulate  the process of buying chips. */
void buy_chips()
{
    printf("Process 1 is buying chips...\n");
}

/* Method to simulate  the process of buying beer. */
void buy_beer()
{
    printf("Process 2 is buying beer...\n");
}

/* Method to simulate the process eat and drink. */
void eat_and_drink()
{
    printf("Eating and drinking...\n");
}

/*
    Main function to solve exercise 07a from PL4 on Semaphores.

    We have used 2 semaphores to synchronize the buying of beer and chips.
*/
int main()
{

    srand(time(NULL));

    // Unlink semaphores at start in case they weren't cleaned up properly
    sem_unlink(SEMAPHORE_CHIPS);
    sem_unlink(SEMAPHORE_BEER);

    // Create the semaphores
    sem_t *chipsSemaphore = sem_open(SEMAPHORE_CHIPS, O_CREAT, 0666, 0); // initial value = 0
    sem_t *beerSemaphore = sem_open(SEMAPHORE_BEER, O_CREAT, 0666, 0);   // initial value = 0

    if (chipsSemaphore == SEM_FAILED || beerSemaphore == SEM_FAILED)
    {
        printf("Error creating the semaphores.\n");
        exit(EXIT_FAILURE);
    }

    int pid = fork();
    if (pid < 0)
    {
        perror("fork error!");
        exit(EXIT_FAILURE); // returns an error status
    }

    if (pid == 0) // Child process
    {
        sleep(rand() % 5);
        buy_chips();
        sem_post(chipsSemaphore);
        sem_wait(beerSemaphore); // waits for the beer to be available
        eat_and_drink();
    }
    else // Parent process
    {
        sleep(rand() % 5);
        buy_beer();
        sem_post(beerSemaphore);
        sem_wait(chipsSemaphore); // Waits for the chips to be available
        eat_and_drink();
        // Wait for child process to finish
        wait(NULL);
    }

    // Clean semaphores
    sem_unlink(SEMAPHORE_CHIPS);
    sem_unlink(SEMAPHORE_BEER);

    return 0;
}
