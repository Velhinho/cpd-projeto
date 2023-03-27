cc = gcc -Wall -Wextra -O3 -g -fopenmp

main: matrix.o queue.o list.o linked.o
	$(cc) tsp-omp.c matrix.o queue.o list.o linked.o -o tsp

queue.o:
	$(cc) nqueue/queue.c nqueue/queue.h -c

list.o:
	$(cc) list.c list.h -c

matrix.o:
	$(cc) matrix.c matrix.h -c

linked.o:
	$(cc) linked.c linked.h -c

clean:
	rm *.o
	rm *.h.gch
	rm nqueue/queue.h.gch
	rm tsp
