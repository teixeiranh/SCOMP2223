all: writer reader


writer.o: writer.c
	gcc -Wall -g -c writer.c -lrt -lm -lpthread

writer: writer.o
	gcc -Wall -g -o writer writer.o -lrt -lm -lpthread


reader.o: reader.c
	gcc -Wall -g -c reader.c -lrt -lm -lpthread

reader: reader.o
	gcc -Wall -g -o reader reader.o -lrt -lm -lpthread


clean:
	rm -f writer reader .*~ *.o *~

run_leitor: reader
	./reader


run_escritor: writer
	./writer

debug_leitor: reader
	gdb -tui reader


debug_escritor: writer
	gdb -tui writer

test_leitor: reader
	valgrind ./reader


test_escritor: writer
	valgrind ./writer


