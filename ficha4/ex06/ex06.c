#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SEM_C "sem_c"
#define SEM_S "sem_s"

int main() {
    sem_t *semS, *semC;
    semS = sem_open(SEM_S, O_CREAT | O_EXCL, 0644, 1);
    semC = sem_open(SEM_C, O_CREAT | O_EXCL, 0644, 1);

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    }
    
    if (pid == 0) {
        // Child process (printing 'S')
        int n=0;
        while (n < 10000) {
            int c_value;
            int s_value;
            sem_getvalue(semC, &c_value);
            sem_getvalue(semS, &s_value);
            if ((c_value - s_value) > 2) {
                sem_wait(semC);
            }
            if ((s_value - c_value) > 2) {
                sem_wait(semS);
            }
            sem_wait(semS);
            printf("S");
            fflush(stdout);
            sem_post(semC);
            
            sleep(.2);
            n++;    
        }
        exit(0);
    } else {
        
        // Parent process (printing 'C')
        int n=0;
        while (n < 10000) {
            int c_value;
            int s_value;
            sem_getvalue(semC, &c_value);
            sem_getvalue(semS, &s_value);
            if ((c_value - s_value) > 2) {
                sem_wait(semC);
            }
            if ((s_value - c_value) > 2) {
                sem_wait(semS);
            }
            sem_wait(semC);
            printf("C");
            sem_post(semS);
            fflush(stdout);

            sleep(.2);
            n++;
        }
        wait(NULL);
        

        // Clean up semaphore
        if(sem_close(semS) == -1) {
            perror("sem_close() failed");
            exit(EXIT_FAILURE);
        }

        if(sem_close(semC) == -1) {
            perror("sem_close() failed");
            exit(EXIT_FAILURE);
        }
        
        if (sem_unlink(SEM_S) < 0) {
            perror("sem_unlink() failed");
            exit(EXIT_FAILURE);
        }

        if (sem_unlink(SEM_C) < 0) {
            perror("sem_unlink() failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
