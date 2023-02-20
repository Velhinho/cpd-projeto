cc = gcc -Wall -Wextra

main: matrix.o list.o
	$(cc) main.c list.o matrix.o -g -o tsp

list.o:
	$(cc) list.c list.h -c

matrix.o:
	$(cc) matrix.c matrix.h -c

clean:
	rm *.o
	rm *.h.gch
	rm tsp