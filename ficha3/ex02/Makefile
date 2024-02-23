# all: ex02

# ex02.o: ex02.c
# 	gcc -Wall -g -c ex02.c -lrt -lm

# ex02: ex02.o
# 	gcc -Wall -g -o ex02.o -lrt -lm

# clean:
# 	rm -f ex02 .*~ *.o *~

# run: ex02
# 	./ex02

# debug: ex02
# 	gdb -tui ex02

# test: ex02
# 	valgrind ./ex02

# A Makefile with variables and suffix rules

# Variables
INCLUDES = 
SOURCES = ex02.c
OBJFILES = ex02.o
EXEC = ex02

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