CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
OBJ = main.o src/server.o src/auth.o src/parser.o src/client_manager.o src/session.o src/word_eval.o src/round_manager.o src/game_logic.o
OUT = servidor

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

main.o: main.c include/server.h include/protocol.h include/config.h include/auth.h include/parser.h include/client_manager.h include/session.h include/game_logic.h
	$(CC) $(CFLAGS) -c main.c -o main.o

src/server.o: src/server.c include/server.h include/protocol.h include/config.h
	$(CC) $(CFLAGS) -c src/server.c -o src/server.o

src/auth.o: src/auth.c include/auth.h
	$(CC) $(CFLAGS) -c src/auth.c -o src/auth.o

src/parser.o: src/parser.c include/parser.h
	$(CC) $(CFLAGS) -c src/parser.c -o src/parser.o

src/client_manager.o: src/client_manager.c include/client_manager.h
	$(CC) $(CFLAGS) -c src/client_manager.c -o src/client_manager.o

src/session.o: src/session.c include/session.h include/client_manager.h
	$(CC) $(CFLAGS) -c src/session.c -o src/session.o

src/word_eval.o: src/word_eval.c include/word_eval.h
	$(CC) $(CFLAGS) -c src/word_eval.c -o src/word_eval.o

src/round_manager.o: src/round_manager.c include/round_manager.h
	$(CC) $(CFLAGS) -c src/round_manager.c -o src/round_manager.o

src/game_logic.o: src/game_logic.c include/game_logic.h include/word_eval.h include/round_manager.h
	$(CC) $(CFLAGS) -c src/game_logic.c -o src/game_logic.o

clean:
	rm -f main.o src/*.o $(OUT)
