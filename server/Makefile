CC = gcc
CFLAGS = -Wall -Wextra
OBJ = main.o server.o
OUT = servidor

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

main.o: main.c server.h protocol.h
	$(CC) $(CFLAGS) -c main.c

server.o: server.c server.h protocol.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o $(OUT)