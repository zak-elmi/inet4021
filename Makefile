CC     = gcc
CFLAGS = -Wall -c -g -ggdb -pthread
LDFLAGS = -Wall -g -ggdb -pthread

server: server.o http.o
	$(CC) $(LDFLAGS) -o server server.o http.o

server.o: src/server.c src/server.h
	$(CC) $(CFLAGS) -c src/server.c

http.o: src/http.c src/server.h
	$(CC) $(CFLAGS) -c src/http.c

clean:
	rm -f *.o server


