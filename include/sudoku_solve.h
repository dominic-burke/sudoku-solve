#ifndef SUDOKU_SOLVE_H
#define SUDOKU_SOLVE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define SUDOKU_SIZE 9
#define SUDOKU_CELL_COUNT (SUDOKU_SIZE * SUDOKU_SIZE)
#define SUDOKU_STRING_SIZE (SUDOKU_CELL_COUNT + 1)

typedef struct Board {
    unsigned short square[SUDOKU_CELL_COUNT];
} Board;

bool board_parse(const char *input, Board *board, char *error, size_t error_size);
bool board_is_valid(const Board *board);
bool solve(Board *board);
void board_print(FILE *stream, const Board *board);
void board_to_string(const Board *board, char output[SUDOKU_STRING_SIZE]);

#endif
