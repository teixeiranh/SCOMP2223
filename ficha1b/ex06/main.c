/* Ex06

Recall the meaning of global variables and static variables. 
Is it safe to use global or static variables
inside a signal handler? Justify your answer

Global variables are variables that are defined outside of a function and can 
be accessed and modified by any function in the program. Static variables are 
variables that are declared inside a function but retain their value between 
calls to the function.

It is not safe to use global or static variables inside a signal handler because 
signal handlers are executed asynchronously and can interrupt the normal flow of 
the program. If a signal handler modifies a global or static variable, it can 
create race conditions or other unexpected behavior in the program. Signal 
handling race conditions can occur whenever a function installed as a signal 
handler is non-reentrant, which means it maintains some internal state or calls 
another function that does so. Such race conditions are even more likely when 
the same function is installed to handle multiple signals.

In general, it is recommended to avoid using global or static variables in signal 
handlers and instead use local variables or pass variables as arguments to the 
signal handler function. If you must use global or static variables in a signal 
handler, it is important to ensure that they are accessed in a thread-safe manner, 
such as through the use of mutexes or other synchronization mechanisms.

*/