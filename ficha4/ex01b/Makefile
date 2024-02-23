# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = ex01b.c
OBJFILES = ex01b.o
EXEC = ex01b

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

clean:
	rm -f ${OBJFILES} ${EXEC}