#include "sudoku_solve.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_PUZZLE_FILE_SIZE 4096

static bool read_puzzle_file(const char *path, char buffer[MAX_PUZZLE_FILE_SIZE + 1])
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        (void)fprintf(stderr, "Unable to open '%s': %s\n", path, strerror(errno));
        return false;
    }

    const size_t bytes_read = fread(buffer, 1, MAX_PUZZLE_FILE_SIZE + 1, file);
    const bool read_failed = ferror(file) != 0;
    const bool close_failed = fclose(file) != 0;

    if (read_failed || close_failed)
    {
        (void)fprintf(stderr, "Unable to read '%s'\n", path);
        return false;
    }
    if (bytes_read > MAX_PUZZLE_FILE_SIZE)
    {
        (void)fprintf(stderr, "Puzzle file '%s' exceeds the %d-byte limit\n",
                      path, MAX_PUZZLE_FILE_SIZE);
        return false;
    }

    buffer[bytes_read] = '\0';
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        (void)fprintf(stderr, "Usage: %s <puzzle-file>\n", argv[0]);
        return 2;
    }

    char input[MAX_PUZZLE_FILE_SIZE + 1];
    if (!read_puzzle_file(argv[1], input))
    {
        return 1;
    }

    Board board;
    char error[128];
    if (!board_parse(input, &board, error, sizeof(error)))
    {
        (void)fprintf(stderr, "Invalid puzzle: %s\n", error);
        return 1;
    }
    if (!board_is_valid(&board))
    {
        (void)fprintf(stderr, "Invalid puzzle: duplicate or out-of-range givens\n");
        return 1;
    }

    (void)printf("Puzzle from '%s':\n", argv[1]);
    board_print(stdout, &board);
    if (!solve(&board))
    {
        (void)fprintf(stderr, "Puzzle has no solution\n");
        return 1;
    }

    (void)puts("Solution:");
    board_print(stdout, &board);
    char solution[SUDOKU_STRING_SIZE];
    board_to_string(&board, solution);
    (void)printf("Solution string: %s\n", solution);
    return 0;
}
