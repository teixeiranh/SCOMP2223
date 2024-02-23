#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main(){
	pid_t p;
	
	if (fork() == 0) {
		printf("First child PID = %d\n", getpid());
		exit(0);
	}
 
	if ((p=fork()) == 0) {
		printf("Second child PID = %d\n", getpid());
		exit(0);
	} 

	printf("Parent PID = %d\n", getpid());
 
	printf("Waiting... (for PID=%d)\n",p); 
	waitpid(p, NULL, 0);
 
	printf("Enter Loop...\n"); 
	while (1); /* Infinite loop */
	
	return 0;
}

/*
a) In the shell, list the running processes with ps (look for the PIDs printed). Are all parent and child processes
listed? Why? 

Running main, the terminal prints:
gcc -Wall -g -c main.c
gcc -Wall -g -o ex08 main.o
./ex08
Parent PID = 2910
PID = 2911
Waiting... (for PID=2912)
PID = 2912
Enter Loop...

After running ps -aux | less in a second terminal, among others, the following processes are present:
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
vitorca+  2910 96.8  0.0   2496   580 pts/7    R+   12:15   0:05 ./ex08
vitorca+  2911  0.0  0.0      0     0 pts/7    Z+   12:15   0:00 [ex08] <defunct>

Only the parent process (2910) and the first child (2911) appear. 
The second child does not appear because of the waitpid function, which waits for the affected child process (the second child) to die.

b) Is there anything particular about the child process? Explain. 
The first child process appears as defunct, also referred to as zombie, since it has completed execution.

c) Kill the parent process: kill -KILL <PID>
In terminal: 
kill -KILL 2910
In standing by terminal:
gcc -Wall -g -c main.c
gcc -Wall -g -o ex08 main.o
./ex08
Parent PID = 2910
PID = 2911
Waiting... (for PID=2912)
PID = 2912
Enter Loop...
make: *** [makefile:19: run] Killed

Both 2910 parent process and 2911 child process are killed.
*/