/* Ex04

Check the documentation to determine the 
differences between the signal() and 
sigaction() functions

a) Which of the two is more suitable for handling signals? Justify your choice

From signal() documentation:
The signal() function and the sigaction() function 
are both used for handling signals in Unix-based operating 
systems. The main differences between these two functions are:

signal() function:
    It is an older function and is not as flexible as sigaction().
    It only allows for a single handler function to be registered for a given signal.
    It has a simple interface that is easy to use.
    
sigaction() function:
    It provides more control over the handling of signals.
    It allows for multiple handler functions to be registered for a given signal.
    It provides additional options for specifying how signals should be handled, 
    such as the ability to block signals while the handler is running.

In general, the sigaction() function is more suitable for handling signals in 
modern Unix-based operating systems. This is because it provides more control 
and flexibility over the handling of signals, which can be important for writing 
robust and reliable applications. Additionally, the sigaction() function provides 
more information about the signal, such as the process ID of the sender and the 
signal delivery time, which can be useful for debugging and logging purposes. 
However, the signal() function can still be useful for simple signal handling 
tasks that do not require the additional functionality provided by sigaction().


b) Document with detail all the parameters of sigaction(), as well as its possible values.

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

signum: This is the signal number for which the behavior is being specified. 
For example, SIGINT is the signal that is sent to a process when the user presses Ctrl-C.

act: This is a pointer to a struct sigaction that describes the new behavior for the 
specified signal. It can be NULL if you just want to get the current behavior for the signal 
(in this case, the oldact argument must not be NULL).

oldact: This is a pointer to a struct sigaction that will be used to return the current 
behavior for the signal. It can be NULL if you don't care about the current behavior 
(in this case, the act argument must not be NULL).

*/