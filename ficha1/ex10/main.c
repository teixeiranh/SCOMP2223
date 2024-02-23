#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define ARRAY_SIZE 2000
#define NUMBER_RANGE 200
#define NUMBER_CHILDS 10
#define DEFAULT_RETURN 255

int main() {

    time_t t;                     
    pid_t pid[NUMBER_CHILDS];  
	int numbers[ARRAY_SIZE]; 
	int status;                

	/* initializes RNG(srand(): stdlib.h;time(): time.h) */
    srand((unsigned)time(&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
    for (int i = 0; i < ARRAY_SIZE; i++)
        numbers[i] = rand() % NUMBER_RANGE;

	/* initialize n */
    int n = rand() % NUMBER_RANGE;


    for (int i = 0; i < NUMBER_CHILDS; i++) {
        pid[i] = fork();

		if (pid[i] < 0) {
			printf("Error creating the processes!\n");
			exit(1);
		}
        if (pid[i] == 0) {
            for (int j = i * NUMBER_RANGE; j < (i + 1) * NUMBER_RANGE; j++) {
                if (numbers[j] == n) {
                    exit(j - i * NUMBER_RANGE);
                }
            }
            exit(255);
        }
		waitpid(pid[i], &status, 0);
        if (WIFEXITED(status) && (WEXITSTATUS(status) == DEFAULT_RETURN)) {
			printf("Child process number %d does not register any occurrence of number %d.\n", i + 1, n);
		} else {
			printf("Child process number %d registers an occurrence of number %d at position %d.\n", i + 1, n, WEXITSTATUS(status));
		}
    }
	return 0;
}