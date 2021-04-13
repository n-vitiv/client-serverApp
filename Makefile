CC=g++

ifeq ($(OS), Windows_NT)
	LIBS=-lws2_32
else
	LIBS=-pthread
	LIBS+=-std=c++11
endif

.PHONY: all clean

all: server client

server: server.cpp
		$(CC) server.cpp functions.cpp -g -o server $(LIBS)

client: client.cpp
		$(CC) client.cpp functions.cpp -g -o client $(LIBS)

clean:
ifeq ($(OS), Windows_NT)
		del server.exe
		del client.exe
else
		rm server
		rm client
endif 