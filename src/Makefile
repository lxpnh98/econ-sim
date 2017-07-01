CFLAGS=-Wall -Wextra -pedantic -O2

main: main.o display.o market.o market_init.o
	cc -o main main.o display.o market.o market_init.o

clean:
	rm *.o main

display.o: display.c display.h market.h
main.o: main.c display.h market.h market_init.o
market.o: market.c market.h display.h
market_init.o: market_init.c market.h
