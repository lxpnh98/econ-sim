CFLAGS=-Wall -Wextra -pedantic -O2

main: main.o display.o market.o
	cc -o main main.o display.o market.o

clean:
	rm *.o main

display.o: display.c display.h market.h
main.o: main.c display.h market.h
market.o: market.c market.h
