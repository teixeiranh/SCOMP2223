#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	int a = 0, b, c, d;
	b = (int) fork();
	c = (int) getpid(); /* getpid(), getppid(): unistd.h*/
	d = (int) getppid();
	a = a + 5;
	if (b == 0) {
		printf("\nChild prints a=%d, b=%d, c=%d, d=%d\n", a, b, c, d);
	} else {
		printf("\nParent prints a=%d, b=%d, c=%d, d=%d\n", a, b, c, d);
	}
	return 0;
}

/*
a) Which of the variables a, b, c and d will have the same value in both processes? 
b) Draw a process tree that describes the processes created.

Only variable a has the same value in both processes.

P	
|	   C	
|------|       
|      |   
|      |

In parent process:
a = 5
b = child pid
c = own pid 
d = parent pid

In child process:
a = 5
b = 0
c = own pid
d = parent pid
*/