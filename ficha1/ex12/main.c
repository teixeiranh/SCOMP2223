#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int spawn_childs(int n) {
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            return i + 1;
        }
    }
    return 0;
}

int main() {
    int n = 6;
    int status;

    int pos = spawn_childs(n);

    if (pos > 0) {
        exit(pos * 2);
    }

    for (int i = 0; i < n; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Child number %d exited successfully.\n", WEXITSTATUS(status));
        }
    }
    
	return 0;
}