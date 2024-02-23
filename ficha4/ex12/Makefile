# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = ex12.c
OBJFILES = ex12.o
EXEC = ex12

# Suffix rules
.SUFFIXES : .c .o

# How to build an object .o from a code file .c ; $< -- file name
.c.o:
	gcc -Wall -pthread -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -pthread -g -o ${EXEC} ${OBJFILES} -lrt -lm -lpthread

${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
	./${EXEC}

vg: ${EXEC}
	 valgrind ./${EXEC}

debug: ${EXEC}
	gdb -tui ${EXEC}
	
clean:
	rm -f ${OBJFILES} ${EXEC}