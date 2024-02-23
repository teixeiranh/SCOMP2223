#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SEMAPHORE_CHIPS "/semaphore_chips"
#define SEMAPHORE_BEER "/semaphore_beer"
#define SEMAPHORE_ALL_BOUGHT "/semaphore_all_bought"

/* Method to simulate  the process of buying chips. */
void buy_chips()
{
    printf("Process is buying chips...\n");
}

/* Method to simulate  the process of buying beer. */
void buy_beer()
{
    printf("Process is buying beer...\n");
}

/* Method to simulate the process eat and drink. */
void eat_and_drink()
{
    printf("Eating and drinking...\n");
}

/*
    Main function to solve exercise 07b from PL4 on Semaphores.

    We have used 3 semaphores. 2 semaphores like the ones used in 07a and an
    additional to ensure that all processes have completed their purchases.
*/
int main()
{
    srand(time(NULL));

    // Unlink semaphores at start in case they weren't cleaned up properly
    sem_unlink(SEMAPHORE_CHIPS);
    sem_unlink(SEMAPHORE_BEER);
    sem_unlink(SEMAPHORE_ALL_BOUGHT);

    // Create the semaphores
    sem_t *chipsSemaphore = sem_open(SEMAPHORE_CHIPS, O_CREAT, 0666, 0);
    sem_t *beerSemaphore = sem_open(SEMAPHORE_BEER, O_CREAT, 0666, 0);
    sem_t *allBoughtSemaphore = sem_open(SEMAPHORE_ALL_BOUGHT, O_CREAT, 0666, 0);

    if (chipsSemaphore == SEM_FAILED || beerSemaphore == SEM_FAILED || allBoughtSemaphore == SEM_FAILED)
    {
        printf("Error creating the semaphores.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 6; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            perror("fork error!");
            exit(EXIT_FAILURE); // returns an error status
        }

        if (pid == 0) // Child process
        {
            sleep(rand() % 5);

            if (i < 3)
            {
                buy_chips();
                sem_post(chipsSemaphore);
            }
            else
            {
                buy_beer();
                sem_post(beerSemaphore);
            }
            // Wait until all processes have completed their purchases
            sem_wait(allBoughtSemaphore);
            eat_and_drink();
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for all purchases to complete
    for (int i = 0; i < 3; i++)
    {
        sem_wait(chipsSemaphore);
        sem_wait(beerSemaphore);
    }

    // Once all purchases are complete, allow all child processes to eat and drink
    for (int i = 0; i < 6; i++)
    {
        sem_post(allBoughtSemaphore);
    }

    // Wait for all child processes to exit
    for (int i = 0; i < 6; i++)
    {
        wait(NULL);
    }

    // Unlink semaphores after use
    if (sem_unlink(SEMAPHORE_CHIPS) == -1)
    {
        perror("sem_unlink error for SEMAPHORE_CHIPS :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEMAPHORE_BEER) == -1)
    {
        perror("sem_unlink error for SEMAPHORE_BEER :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEMAPHORE_ALL_BOUGHT) == -1)
    {
        perror("sem_unlink error for SEMAPHORE_ALL_BOUGHT :/");
        exit(EXIT_FAILURE);
    }

    return 0;
}
