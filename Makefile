#CFLAGS += -Wall -Wextra -pedantic -O0 -std=gnu17

all:
	gcc -o Server.o tcpServer.c
	gcc -o Client.o tcpClient.c
	

clean:
	-rm *.o
