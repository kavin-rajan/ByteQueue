#variables
CC = gcc
CFLAGS = -Iinclude -Wall -g
SOURCE = src/Main.c src/ByteQueue.c
OBJECT = $(SOURCE:.c=.o)
TARGET = bin/ByteQueue

all: $(TARGET)

$(TARGET): $(SOURCE)
		$(CC) $(CFLAGS) $(SOURCE) -o$(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
		rm -f $(TARGET) $(OBJECTS)

