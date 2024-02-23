#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void error(pid_t p) {
	if (p < 0) {
        perror("Error dealing with process!\n");
        exit(-1);
    }
}

int main() {
	int status;
	pid_t pids[4];
	//int x = 0;
	for (int i = 0; i < 4; i++) {
		pids[i] = fork();
		error(pids[i]);
		if (pids[i] == 0) { 
			printf("child %d from parent %d created in order %d.\n", getpid(), getppid(), i);
            exit(0);
		}
		if(pids[i] % 2 == 0){
			waitpid(pids[i], &status, 0);
			if(WIFEXITED(status)){
				printf("Even child process with PID %d exited.\n", pids[i]);
			}
		}
	}
	printf("This is the end.\n");
	return 0;
} 

/*

a) How many processes will be created by this code? Justify by drawing a process tree that describes the
processes created.

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	int i;
	int status;
	int x = 0;
	for (int i = 0; i < 4; i++) {
		if (fork() == 0) { 
            sleep(1);
		}
	}
	printf("This is the end.\n");
	return 0;
} 

16 processes will be created in total.
The fork is executed 4 times.
The first fork results in 2 processes.
The second fork results in 2 + 2 = 4 processes.
The third fork results in 4 + 4 = 8 processes.
The fourth fork results in 8 + 8 = 16 processes.

-------------------------------------------------
	|		|		|			|	
	|		|		|			|----------------
	|		|		|
	|		|		|----------------------------
	|		|					|
	|		|					|----------------
	|		|
	|		|------------------------------------	
	|				|			|
	|				|			|----------------
	|  				|
	|				|----------------------------
	|							|
	| 							|----------------
	|
	|--------------------------------------------
			|		|			|		
			|		|			|----------------
			|		|
			|		|----------------------------
			|					|
			|					|----------------
			|
			|------------------------------------
					|			|	
					|			|----------------
					|	
					|----------------------------
								|
								|----------------


b) What change, if any, would you do to this code so that exactly 4 child processes are created?

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	for (int i = 0; i < 4; i++) {
		if (fork() == 0) { 
			printf("child %d - parent %d.\n", getpid(), getppid());
            exit(0);
		}
	}
	printf("This is the end.\n");
	return 0;
} 

Replacing "sleep(1)" with "exit(0)". 
That way inside the if-statement whenever a child is created it is killed immediately before the 
next fork, so that only the first process creates a child in each fork inside the for-loop. 

c) Assuming the changes in b), change the code so that the parent process waits for child processes with an
even PID.

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	int status;
	pid_t pids[4];
	for (int i = 0; i < 4; i++) {
		if ((pids[i] = fork()) == 0) { 
			printf("child %d - parent %d.\n", getpid(), getppid());
            exit(0);
		}
		if(pids[i] % 2 == 0){
			waitpid(pids[i], &status, 0);
			if(WIFEXITED(status)){
				printf("Even child process with PID %d exited.\n", pids[i]);
			}
		}
	}
	printf("This is the end.\n");
	return 0;
} 

d) Assuming the changes in b) and c), change the code so that the child processes return a number that reflects
their creation order (that is, the first child process returns 1, the second returns 2, …). 

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void error(pid_t p) {
	if (p < 0) {
        perror("Error dealing with process!\n");
        exit(-1);
    }
}

int main() {
	int status;
	pid_t pids[4];
	//int x = 0;
	for (int i = 0; i < 4; i++) {
		pids[i] = fork();
		error(pids[i]);
		if (pids[i] == 0) { 
			printf("child %d from parent %d created in order %d.\n", getpid(), getppid(), i);
            exit(0);
		}
		if(pids[i] % 2 == 0){
			waitpid(pids[i], &status, 0);
			if(WIFEXITED(status)){
				printf("Even child process with PID %d exited.\n", pids[i]);
			}
		}
	}
	printf("This is the end.\n");
	return 0;
} 

*/