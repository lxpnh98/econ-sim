CFLAGS=-Wall -Wextra -pedantic -O2 -g

main: main.o display.o market.o market_init.o interface.o
	cc -o main main.o display.o market.o market_init.o interface.o

clean:
	rm *.o main

main.o: main.c
display.o: display.c
market.o: market.c
market_init.o: market_init.c
interface.o: interface.c
