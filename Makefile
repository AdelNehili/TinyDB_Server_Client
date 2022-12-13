#CFLAGS += -Wall -Wextra -pedantic -O0 -std=gnu17

all:
	gcc -o Server tcpServer.c
	gcc -o Client tcpClient.c
	

clean:
	-rm tcpServer tcpClient authentificationClient authentificationServer
