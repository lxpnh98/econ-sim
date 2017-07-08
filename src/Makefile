CFLAGS=-Wall -Wextra -pedantic -O2

main: main.o display.o market.o market_init.o interface.o
	cc -o main main.o display.o market.o market_init.o interface.o

clean:
	rm *.o main

main.o: main.c interface.h
display.o: display.c display.h market.h
market.o: market.c market.h display.h
market_init.o: market_init.c market_init.h market.h
interface.o: interface.c interface.h market.h market_init.h display.h
