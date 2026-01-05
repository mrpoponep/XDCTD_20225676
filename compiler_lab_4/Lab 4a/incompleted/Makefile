CC = gcc
CFLAGS = -Wall -c
LIBS = -lm

OBJS = main.o parser.o scanner.o reader.o charcode.o token.o error.o symtab.o semantics.o debug.o
TARGET = kplc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
