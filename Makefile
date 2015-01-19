CF = -c -Wall

all: dns

debug: CF+=-g
debug: dns

dns: dns.o manager.o misc.o resolve.o label.o
	gcc -o dns dns.o manager.o misc.o resolve.o label.o
	
dns.o: dns.c dns.h
	gcc $(CF) dns.c
manager.o: manager.c manager.h dns.h
	gcc $(CF) manager.c
misc.o: misc.c misc.h
	gcc $(CF) misc.c
resolve.o: resolve.c resolve.h dns.h manager.h
	gcc $(CF) resolve.c
label.o: label.c label.h
	gcc $(CF) label.c

.PHONY=clean
clean:
	rm *.o dns
