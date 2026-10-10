CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Iinclude
TARGET = ls

OBJS = main.o options.o entry.o sort.o display.o traverse.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o $@

options.o: src/options.c
	$(CC) $(CFLAGS) -c src/options.c -o $@

entry.o: src/entry.c
	$(CC) $(CFLAGS) -c src/entry.c -o $@

sort.o: src/sort.c
	$(CC) $(CFLAGS) -c src/sort.c -o $@

display.o: src/display.c
	$(CC) $(CFLAGS) -c src/display.c -o $@

traverse.o: src/traverse.c
	$(CC) $(CFLAGS) -c src/traverse.c -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean