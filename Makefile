cc = gcc -Wall -Wextra

main: matrix.o queue.o tour.o
	$(cc) main.c matrix.o queue.o tour.o -g -o tsp

queue.o:
	$(cc) nqueue/queue.c nqueue/queue.h -c

tour.o:
	$(cc) tour.c tour.h -c

matrix.o:
	$(cc) matrix.c matrix.h -c

clean:
	rm *.o
	rm *.h.gch
	rm tsp