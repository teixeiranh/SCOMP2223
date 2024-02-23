# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = participant.c
OBJFILES = participant.o
EXEC = participant

# Suffix rules
.SUFFIXES : .c .o

# How to build an object .o from a code file .c ; $< -- file name
.c.o:
	gcc -Wall -pthread -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -pthread -g -o ${EXEC} ${OBJFILES} -lrt -lm

${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
#	./${EXEC}

#vg: ${EXEC}
#	 valgrind ./${EXEC}

clean:
	rm -f ${OBJFILES} ${EXEC}