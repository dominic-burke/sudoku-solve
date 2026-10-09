CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = sudoku_solve

OBJS = sudoku_solve.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

sudoku_solve.o: sudoku_solve.c 
	$(CC) $(CFLAGS) -c sudoku_solve.c

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
