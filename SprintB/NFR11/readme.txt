========================================= readme.txt =========================================

NFR11 - Explore Shared Board Synchronization:

Pattern : Producer-Consumer

Pseudo-code:

	1. Declare shared memory object and semaphore
	2. Open shared memory object
	3. Set size of shared memory object
	4. Map shared memory object into memory
	5. Initialize board with 0
	6. Create named semaphore
	7. While write_count is less than WRITE_LIMIT
	   a. Try to get the semaphore
	   b. Increment the board value and write it back
	   c. Print the process ID and the value written to the board
	   d. Release the semaphore
	   e. Increment write_count
	   f. Sleep for 1 second
	8. Exit the program

How to run : 

	1. Use "makerun" on the command line - this will compile participant.c
	Alternatively use "gcc -o participant participant.c -lrt -lpthread"
	2. In the command line write "./participant &" for each concurrent participant. For example:
		2 participants - "./participant & ./participant"
		3 participants - "./participant & ./participant & ./participant"
		etc...
	Each participant will write 10 times on the shared board