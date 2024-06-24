CC = gcc
CFLAGS = -Wall -g -fPIC

all: st_reactor.so react_server
	clear

react_server: server.o
	$(CC) $(CFLAGS) -o react_server server.o -L. -l:st_reactor.so -lpthread

server.o: server.c reactor.h
	$(CC) $(CFLAGS) -c server.c

st_reactor.so: reactor.o
	$(CC) $(CFLAGS) -shared -o st_reactor.so reactor.o

reactor.o: reactor.c reactor.h
	$(CC) $(CFLAGS) -c reactor.c

clean:
	rm -f react_server server.o st_reactor.so reactor.o
