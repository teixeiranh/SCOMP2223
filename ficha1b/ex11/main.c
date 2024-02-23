#include <stdio.h>
#include <signal.h>

void list_blocked_signals_on_sigusr1()
{
    sigset_t block_set;
    sigfillset(&block_set);
    sigdelset(&block_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block_set, NULL);
    printf("Signals blocked on receipt of SIGUSR1:\n");
    for (int i = 1; i <= NSIG; i++)
    {
        if (sigismember(&block_set, i))
        {
            printf("%d\n", i);
        }
    }
}

int main(){
    return (0);
}

