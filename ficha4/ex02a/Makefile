all: writer reader


writer.o: writer.c
	gcc -Wall -g -c writer.c -lrt -lm -lpthread

writer: writer.o
	gcc -Wall -g -o writer writer.o -lrt -lm -lpthread

clean:
	rm -f writer reader .*~ *.o *~

run_escritor: writer
	./writer

debug_escritor: writer
	gdb -tui writer

test_escritor: writer
	valgrind ./writer


