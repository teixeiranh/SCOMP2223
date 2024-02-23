# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = ex07a.c
OBJFILES = ex07a.o
EXEC = ex07a

# Suffix rules
.SUFFIXES : .c .o

# How to build an object .o from a code file .c ; $< -- file name
.c.o:
	gcc -Wall -pthread -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -pthread -g -o ${EXEC} ${OBJFILES} -lrt -lm

${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
	./${EXEC}

vg: ${EXEC}
	 valgrind ./${EXEC}

clean:
	rm -f ${OBJFILES} ${EXEC}