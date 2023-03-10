cc = gcc -Wall -Wextra -O3 -fopenmp

main: matrix.o queue.o list.o
	$(cc) main.c matrix.o queue.o list.o -o tsp

queue.o:
	$(cc) nqueue/queue.c nqueue/queue.h -c

list.o:
	$(cc) list.c list.h -c

matrix.o:
	$(cc) matrix.c matrix.h -c

clean:
	rm *.o
	rm *.h.gch
	rm nqueue/queue.h.gch
	rm tsp