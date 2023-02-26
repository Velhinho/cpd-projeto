cc = gcc -Wall -Wextra

main: matrix.o list.o queue.o
	$(cc) main.c list.o matrix.o queue.o -g -o tsp

queue.o:
	$(cc) nqueue/queue.c nqueue/queue.h -c

list.o:
	$(cc) list.c list.h -c

matrix.o:
	$(cc) matrix.c matrix.h -c

clean:
	rm *.o
	rm *.h.gch
	rm tsp