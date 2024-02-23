# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = ex14.c
OBJFILES = ex14.o
EXEC = ex14

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