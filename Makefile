CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS ?=
LDLIBS ?=

TARGET = sudoku_solve
TEST_TARGET = test_sudoku
GUI_TARGET = sudoku_gui
GUI_TEST_TARGET = test_gui
BUILD_DIR = build
APP = $(BUILD_DIR)/$(TARGET)
TEST_APP = $(BUILD_DIR)/$(TEST_TARGET)
GUI_APP = $(BUILD_DIR)/$(GUI_TARGET)
GUI_TEST_APP = $(BUILD_DIR)/$(GUI_TEST_TARGET)
LIBRARY_SOURCE = src/sudoku_solve.c
HEADER = include/sudoku_solve.h
GUI_HEADER = src/gui.h
GTK4_CFLAGS ?= $(shell pkg-config --cflags gtk4 2>/dev/null)
GTK4_LIBS ?= $(shell pkg-config --libs gtk4 2>/dev/null)
GUI_CFLAGS = $(filter-out -Wpedantic,$(CFLAGS))

.PHONY: all test gui gui-test clean showcase

all: $(APP)

$(APP): $(LIBRARY_SOURCE) src/main.c $(HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) $(LIBRARY_SOURCE) src/main.c $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_APP): tests/test_sudoku.c $(LIBRARY_SOURCE) $(HEADER) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) tests/test_sudoku.c $(LIBRARY_SOURCE) $(LDFLAGS) $(LDLIBS) -o $@

GUI_SOURCES = src/gui_main.c src/gui.c src/gui_actions.c

$(GUI_APP): $(GUI_SOURCES) $(HEADER) $(GUI_HEADER) | $(BUILD_DIR)
	@if [ -z "$(GTK4_CFLAGS)" ] || [ -z "$(GTK4_LIBS)" ]; then \
		echo "GTK 4 development files are required; install gtk4 and pkg-config support first." >&2; \
		exit 1; \
	fi
	$(CC) $(CPPFLAGS) -Iinclude -Isrc $(GUI_CFLAGS) $(GTK4_CFLAGS) $(GUI_SOURCES) \
		$(LIBRARY_SOURCE) $(LDFLAGS) $(LDLIBS) $(GTK4_LIBS) -o $@

$(GUI_TEST_APP): tests/test_gui.c $(LIBRARY_SOURCE) $(GUI_SOURCES) $(HEADER) $(GUI_HEADER) | $(BUILD_DIR)
	@if [ -z "$(GTK4_CFLAGS)" ] || [ -z "$(GTK4_LIBS)" ]; then \
		echo "GTK 4 development files are required; install gtk4 and pkg-config support first." >&2; \
		exit 1; \
	fi
	$(CC) $(CPPFLAGS) -Iinclude -Isrc $(GUI_CFLAGS) $(GTK4_CFLAGS) tests/test_gui.c \
		$(LIBRARY_SOURCE) src/gui.c src/gui_actions.c $(LDFLAGS) $(LDLIBS) $(GTK4_LIBS) -o $@

gui: $(GUI_APP)

gui-test: $(GUI_TEST_APP)
	@if command -v xvfb-run >/dev/null 2>&1; then \
		xvfb-run -a ./$(GUI_TEST_APP); \
	else \
		./$(GUI_TEST_APP); \
	fi

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
