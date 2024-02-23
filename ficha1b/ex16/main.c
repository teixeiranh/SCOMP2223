/* Ex 16. A software house wants to evaluate the potential of a new algorithm to process data in parallel to
extract valuable information in a faster way than the solution in production. For this purpose, this
algorithm will be tested in a simulated application that uses parallel/concurrent programming.
You should develop a program where the master process (i.e. the parent) creates 50 worker processes
(i.e. children).
• Workflow of the worker processes.
Each worker process starts executing of the simulate1() function. This function returns 1 (one)
if it finds relevant data or 0 (zero) if not.
When this function completes, the worker process sends the parent process a SIGUSR1 signal in
the case of sucess or a SIGUSR2 signal otherwise.
After that, the process will wait for a signal from the master process before starting to execute
simulate2() function.
• Workflow of the master process.
The master process waits for the notifications sent by the workers. After 25 processes have
finished, the master process will take one of two possible actions:
(a) If none of the reported searches are successful, the parent process will display the message
“Inefficient algorithm!” and terminate all children.
(b) Otherwise, the master process will send the SIGUSR1 signal to all the worker processes.
Every worker process will immediately start the execution of the simulate2() function, even
if it means interrupting the simulate1() function.
Use randomly generated values to simulate the execution time and the result of both simulate1() and
simulate2() functions.
Set a very low probability of success for the simulate1() function to ensure both scenarios will occur.
*/
