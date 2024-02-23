#include <stdio.h>
#include <unistd.h>

int main() {
    for( ; ; ) {
        printf("I Love SCOMP!\n");
        sleep(1);
    }
}

/* a) On one terminal, compile and run the program.

After running in terminal:
gcc -Wall -g -c main.c
gcc -Wall -g -o ex02 main.o
./ex02
I Love SCOMP!
I Love SCOMP!
I Love SCOMP!
I Love SCOMP!
I Love SCOMP!
^Cmake: *** [makefile:22: run] Interrupt
*/

/* b) Open another terminal, and type the command ps to discover the PID of the process in which
the program displaying the “I love SCOMP!” message is executing.

After running in terminal:
Process with PID 1367
gcc -Wall -g -o ex02 main.o
./ex02
I Love SCOMP!
I Love SCOMP!
ps
  PID TTY          TIME CMD
 1111 pts/2    00:00:00 bash
 2675 pts/2    00:00:00 make
 2683 pts/2    00:00:00 ex02
 2987 pts/2    00:00:00 ps
 */

/* c) Type the command kill -s STOP target˙PID to send the STOP signal and suspend that process.
Note: target˙PID is the PID of the process that will receive the signal.

After running in terminal:
I Love SCOMP!
I Love SCOMP!
kiI Love SCOMP!
ll I Love SCOMP!
-I Love SCOMP!
s STI Love SCOMP!
OP I Love SCOMP!
268I Love SCOMP!
3
*/

/* d) Type the command ps -l to check that the process became “stopped” (status should be “T”)

After running in terminal:
F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  1111    54  0  80   0 -  2670 do_wai pts/2    00:00:00 bash
0 S  1000  2675  1111  0  80   0 -  1901 do_wai pts/2    00:00:00 make
0 T  1000  2683  2675  0  80   0 -   624 do_sig pts/2    00:00:00 ex02
0 R  1000  3320  1111  0  80   0 -  2635 -      pts/2    00:00:00 ps
*/

/* e) Send the signal CONT such the process resumes its normal execution. Check that the process left
the “stopped” state.

After running kill -18 2683 in terminal:
I Love SCOMP!
I Love SCOMP!
*/