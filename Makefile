CC       := gcc
CFLAGS   := -Iinclude -Ilib

TARGET   := server

.PHONY: all
all: $(TARGET)

$(TARGET): src/server.o src/commands.o src/utils.o src/rooms.o src/history.o
	$(CC) $(CFLAGS) src/server.o src/commands.o src/utils.o src/rooms.o src/history.o -o $(TARGET)

src/server.o: src/server.c
	$(CC) $(CFLAGS) -c src/server.c -o src/server.o

src/commands.o: src/commands.c
	$(CC) $(CFLAGS) -c src/commands.c -o src/commands.o

src/utils.o: src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

src/rooms.o: src/rooms.c
	$(CC) $(CFLAGS) -c src/rooms.c -o src/rooms.o

src/history.o: src/history.c
	$(CC) $(CFLAGS) -c src/history.c -o src/history.o

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) src/*.o
