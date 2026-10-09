#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 9
#define SQRT_SIZE 3
#define MAX_NUM SIZE
#define BORDER_SIZE (((SQRT_SIZE * 2) + 1) * SQRT_SIZE) + 1
#define HORIZONTAL_DIVIDER '~'
#define VERTICAL_DIVIDER '|'
#define EMPTY_CELL '.'
#define ANCHOR '+'
#define BOARD_FULL -1
#define MIN_ARGS 2

typedef enum RowCol { ROW,
    COL } RowCol;

typedef struct Board {
    unsigned short square[SIZE * SIZE];
} Board;

Board* alloc_board()
{
    Board* b = malloc(sizeof(Board));
    assert(b != NULL);
    return b;
}

Board* board_from_str(char* puzzle)
{
    assert(strlen(puzzle) == (SIZE * SIZE));

    Board* b = alloc_board();

    size_t i = 0;
    while (puzzle[i] != '\0') {
        b->square[i] = (unsigned short)(puzzle[i] - '0');
        i++;
    }

    return b;
}

void print_divider()
{
    for (size_t i = 0; i < BORDER_SIZE; i++) {
        if (i % (((BORDER_SIZE - 1) / SQRT_SIZE)) == 0) {
            putchar(ANCHOR);
        } else {
            putchar(HORIZONTAL_DIVIDER);
        }
    }
    putchar('\n');
}

void board_print(Board* b)
{
    print_divider();
    for (size_t i = 0; i < (SIZE * SIZE); i++) {
        // print the interior borders
        if (i % SQRT_SIZE == 0) {
            putchar(VERTICAL_DIVIDER);
        }

        // handle newlines
        if (i % (SQRT_SIZE * SIZE) == 0 && i != 0) {
            putchar('\n');
            print_divider();
            putchar(VERTICAL_DIVIDER);
        } else if (i % SIZE == 0 && i != 0) {
            putchar('\n');
            putchar(VERTICAL_DIVIDER);
        }

        // print the actual numbers
        if (b->square[i] == 0) {
            putchar(EMPTY_CELL);
            putchar(' ');
        } else {
            printf("%d ", b->square[i]);
        }
    }
    // final divider
    putchar(VERTICAL_DIVIDER);
    putchar('\n');

    print_divider();
}

// returns true for not seen
// return false for seen before
// frees seen if it returns a false value
bool check_unseen(__uint16_t* seen, unsigned short candidate)
{
    assert(candidate <= MAX_NUM);

    // 0 value just means that the square is empty in sudoku
    if (0 == candidate) {
        return true;
    }

    __uint16_t check = 1UL << (candidate - 1);

    if (*seen & check) {
        // free seen here because don't want to write it twice in check_row_col
        free(seen);
        return false;
    } else {
        *seen |= check;
        return true;
    }
}

bool check_row_col(Board* b, size_t index, RowCol row_col)
{
    __uint16_t* seen = malloc(sizeof(__uint16_t));
    // haven't seen anything yet
    *seen = 0;

    if (row_col == ROW) {
        // jump down the rows
		const size_t row_index = floor(index / SIZE) * SIZE;

        for (size_t i = row_index; i < (row_index + SIZE); i++) {
            assert(b->square[i] <= MAX_NUM);
            if (!check_unseen(seen, b->square[i])) {
                return false;
            }
        }
    } else if (row_col == COL) {
        for (size_t i = index % SIZE; i < (SIZE * SIZE); i += SIZE) {
            assert(b->square[i] <= MAX_NUM);
            if (!check_unseen(seen, b->square[i])) {
                return false;
            }
        }
    } else {
        // always fail if row_col is set badly
        assert(false);
    }
    // free seen pointer here because we haven't seen a false value
    // and check_unseen is not doing it for us
    free(seen);
    // true for all good
    return true;
}

size_t get_box_top_left(size_t index) 
{
	return (floor(index / 27) * 27) + (floor((index % 9) / 3) * 3);
}

// true for all good
// false for no good
bool check_square(Board* b, size_t index)
{
    __uint16_t* seen = malloc(sizeof(__uint16_t));
    *seen = 0;

    // jump to top left of sub-square
	size_t top_left = get_box_top_left(index);

    // i is the row selector
    for (size_t i = top_left; i < (top_left + (SIZE * SQRT_SIZE)); i += SIZE) {
        // j is the col selector but it's a simple iterator because we're already
		// indexing into the array using i
        for (size_t j = 0; j < SQRT_SIZE; j++) {
            assert(b->square[i + j] <= MAX_NUM);
            assert(i + j < SIZE * SIZE);
            if (!check_unseen(seen, b->square[i + j])) {
                // tell the board that this square failed
                return false;
            }
        }
    }
    free(seen);

    // tell the board this square is checked
    return true;
}

bool check_board(Board* b, size_t i)
{
	return check_row_col(b, i, COL) 
		&& check_row_col(b, i, ROW) 
		&& check_square(b, i);
}

int get_next_empty_index(Board* b)
{
    for (int i = 0; i < SIZE * SIZE; i++) {
        if (b->square[i] == 0) {
            return i;
        }
    }
    // -1 for BOARD_FULL
    return BOARD_FULL;
}

Board* solve(Board* b)
{
    int next_empty_index = get_next_empty_index(b);
    if (next_empty_index == BOARD_FULL) {
        return b;
    }
    // 1 through 9
    for (size_t i = 1; i <= SIZE; i++) {
        b->square[next_empty_index] = i;
        if (!check_board(b, next_empty_index)) {
            continue;
		} 
		Board *solved_b = solve(b);	
		if (solved_b != NULL) {
			return solved_b;
		}
	}
	b->square[next_empty_index] = 0;
	return NULL;
}

char* output_board_puzzle_str(Board* b)
{
    char* output = malloc(sizeof(char) * (SIZE * SIZE) + 1);

    size_t i = 0;
    for (; i < (SIZE * SIZE); i++) {
        output[i] = (char)(b->square[i] + '0');
    }
    output[i] = '\0';
    return output;
}

void print_board_as_puzzle_str(Board* b)
{
    char* output = output_board_puzzle_str(b);
    printf("%s\n", output);

    free(output);
}

char* get_sudoku_file(char* filename)
{
    FILE* sudoku_file = fopen(filename, "r");

    if (sudoku_file == NULL) {
        printf("Couldn't read sudoku file");
        return NULL;
    }

    fseek(sudoku_file, 0L, SEEK_END);
    size_t file_size = ftell(sudoku_file);

    rewind(sudoku_file);

    // will always be big enough
    char* buffer = malloc(1 + (sizeof(char) * file_size));

    assert(buffer != NULL);

    const size_t bytes_read = fread(buffer, 1, file_size, sudoku_file);

    if (bytes_read < (size_t)file_size && ferror(sudoku_file)) {
        return NULL;
    }
    buffer[bytes_read] = '\0';

    return buffer;
}

int main(int argc, char** argv)
{
	if (argc < MIN_ARGS) {
		printf("USAGE: ./sudoku_sovlve [puzzle_file]");
		return 1;
	}

    char* sudoku_input = get_sudoku_file(argv[MIN_ARGS - 1]);

    if (sudoku_input == NULL) {
        return 1;
    }
	printf("Solving puzzle string from file \"%s\"\n", argv[MIN_ARGS -1]);

    // alloc here
    Board* b = board_from_str(sudoku_input);

    board_print(b);

    solve(b);
    board_print(b);

	printf("Solution as puzzle string: \n");
    print_board_as_puzzle_str(b);

    free(b);
    free(sudoku_input);

    return 0;
}
