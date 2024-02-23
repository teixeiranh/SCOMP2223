#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void error(pid_t p) {
	if (p < 0) {
        perror("Error creating!\n");
        exit(-1);
    }
}

int main(void) {
	
	printf("I'm..\n");								// initial printf by parent process
	pid_t p1 = fork(); 								// p1 parent creates 1st child
	error(p1);

	if (p1 > 0) { 									// p1 parent waits 1st child printf
		p1 = fork(); 								// p1 parent creates 2nd child	
		error(p1);

		if (p1 > 0) { 								// p1 parent waits 2nd child printf
			printf("the..\n");						// p1 parent printf after 2nd child printf
			
			p1 = fork(); 							// p1 parent creates 3rd child	
			error(p1);

			if (p1 > 0) { 							// p1 parent waits 3rd child printf		
				printf("father!\n");				// p1 parent printf after 3rd child printf

			} else {
				printf("I'll never join you!\n"); 	// 3rd child printf
			}
		} else {
			printf("I'll never join you!\n"); 		// 2nd child printf
		}
	} else {
		printf("I'll never join you!\n"); 			// 1st child printf
	}
	return 0;
} 