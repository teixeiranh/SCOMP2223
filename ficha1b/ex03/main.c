#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main() {
    
    int pid, signal;
    
    printf("Enter PID of the target process: "); // ask the user for the PID
    scanf("%d", &pid); // save the PID
    printf("Enter signal number to send: "); // ask the user for the signal
    scanf("%d", &signal); // save the signal to be sent

    // check for an error
    if (kill(pid, signal) == -1) {
        perror("Error sending signal");
        exit(1);
    }
    
    // print the result
    printf("Signal %d sent to process %d\n", signal, pid);
    
    return 0;
}

