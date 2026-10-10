CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS ?=
LDLIBS ?=

TARGET = sudoku_solve
TEST_TARGET = test_sudoku
BUILD_DIR = build
APP = $(BUILD_DIR)/$(TARGET)
TEST_APP = $(BUILD_DIR)/$(TEST_TARGET)
LIBRARY_SOURCE = src/sudoku_solve.c
HEADER = include/sudoku_solve.h

.PHONY: all test clean showcase

all: $(APP)

$(APP): $(LIBRARY_SOURCE) src/main.c $(HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) $(LIBRARY_SOURCE) src/main.c $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_APP): tests/test_sudoku.c $(LIBRARY_SOURCE) $(HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) tests/test_sudoku.c $(LIBRARY_SOURCE) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR):
	mkdir -p $@

test: $(APP) $(TEST_APP)
	./$(TEST_APP)
	./tests/test_cli.sh

showcase: $(APP)
	./$(APP) examples/sample_puzzle

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TEST_TARGET) sudoku_solve.o
