CC=gcc
CFLAGS=-Wall -Werror


all: cmp copy encode decode stshell
cmp: cmp.c
	$(CC) $(CFLAGS) cmp.c -o cmp

copy: copy.c
	$(CC) $(CFLAGS) copy.c -o copy

encode: libcodecA.so libcodecB.so codech.h
	$(CC) $(CFLAGS) -o encode encode.c ./libcodecA.so ./libcodecB.so

decode: libcodecA.so libcodecB.so codech.h
	$(CC) $(CFLAGS) -o decode decode.c ./libcodecA.so ./libcodecB.so

libcodecA.so: codecA.c
	$(CC) -shared -fPIC -o libcodecA.so codecA.c

libcodecB.so: codecB.c
	$(CC) -shared -fPIC -o libcodecB.so codecB.c

stshell: stshell.c
	$(CC) $(CFLAGS) stshell.c -o stshell

clean:
	rm -f cmp copy encode decode *.o *.so stshell