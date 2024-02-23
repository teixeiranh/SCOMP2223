#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>

/*
    Main function to solve exercise 05 from PL4 on Semaphores.

    In this function we have used fflush (from stdio.h), so that the output
    is flushed to avoid problems with the output.

    We have used 3 semaphores to synchronize the processes of these 3 children.
*/
int main()
{

    sem_t *semaphore1 = sem_open("/semaphore1", O_CREAT, 0644, 0); // initial value = 0
    if (semaphore1 == SEM_FAILED)
    {
        perror("sem_open failure!! --'");
        exit(EXIT_FAILURE);
    }

    sem_t *semaphore2 = sem_open("/semaphore2", O_CREAT, 0644, 0); // initial value = 0
    if (semaphore2 == SEM_FAILED)
    {
        perror("sem_open failure!! --'");
        exit(EXIT_FAILURE);
    }

    sem_t *semaphore3 = sem_open("/semaphore3", O_CREAT, 0644, 0); // initial value = 0
    if (semaphore3 == SEM_FAILED)
    {
        perror("sem_open failure!! --'");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) // Child 1
    {

        printf("Sistemas ");
        fflush(stdout);
        sem_post(semaphore1); // Unlocks semaphore1 (=1)

        sem_wait(semaphore3); // Waits for "Computadores -"
        printf("a ");
        fflush(stdout);
        sem_post(semaphore1); // Increment the semaphore1 (=2)

        _exit(EXIT_SUCCESS);
    }

    if (fork() == 0) // Child 2
    {
        sem_wait(semaphore1); // Waits for "Sistemas "
        printf("de ");
        fflush(stdout);
        sem_post(semaphore2); // Unlocks semaphore2 (=1)

        sem_wait(semaphore1); // Waits for "a "
        printf("melhor ");
        fflush(stdout);
        sem_post(semaphore2);

        exit(EXIT_SUCCESS);
    }

    if (fork() == 0) // Child 3
    {
        sem_wait(semaphore2); // Waits for "de"
        printf("Computadores - ");
        fflush(stdout);
        sem_post(semaphore3); // Unlocks semaphore3 (=1)

        sem_wait(semaphore2); // Waits for "melhor"
        printf("disciplina do mundo <3 (juntamente com ARQCP heheh)!\n ");
        fflush(stdout);

        exit(EXIT_SUCCESS);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    // Cleanup semaphores
    if (sem_close(semaphore1) == -1) // Close the semaphore
    {
        perror("sem_close problem1! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphore2) == -1) // Close the semaphore
    {
        perror("sem_close problem! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphore3) == -1) // Close the semaphore
    {
        perror("sem_close problem! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink("/semaphore1") == -1) // Unlink the semaphore
    {
        perror("sem_unlink error! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink("/semaphore2") == -1)
    {
        perror("sem_unlink error! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink("/semaphore3") == -1)
    {
        perror("sem_unlink error! :/");
        exit(EXIT_FAILURE);
    }

    return 0;
}
