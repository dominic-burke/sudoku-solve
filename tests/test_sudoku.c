#include "sudoku_solve.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                                         \
    do {                                                                                         \
        if (!(condition)) {                                                                      \
            (void)fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
            return 1;                                                                            \
        }                                                                                        \
    } while (0)

static const char sample_puzzle[] =
    "016400000"
    "200009000"
    "400000062"
    "070230100"
    "100000003"
    "003087040"
    "960000005"
    "000800007"
    "000006820";

static const char sample_solution[] =
    "316452978"
    "285679314"
    "497318562"
    "879234156"
    "142965783"
    "653187249"
    "968721435"
    "521843697"
    "734596821";

static int test_parser(void)
{
    Board board;
    char error[128];
    CHECK(board_parse(sample_puzzle, &board, error, sizeof(error)));
    CHECK(error[0] == '\0');
    CHECK(board.square[0] == 0);
    CHECK(board.square[1] == 1);

    CHECK(board_parse(
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "123456789\n"
        "12345678.\n",
        &board, error, sizeof(error)));
    CHECK(board.square[SUDOKU_CELL_COUNT - 1] == 0);

    CHECK(!board_parse("123", &board, error, sizeof(error)));
    CHECK(strstr(error, "expected 81") != NULL);
    CHECK(!board_parse(
        "12345678912345678912345678912345678912345678912345678912345678912345678912345678x",
        &board, error, sizeof(error)));
    CHECK(strstr(error, "invalid puzzle character") != NULL);
    CHECK(!board_parse(NULL, &board, error, sizeof(error)));
    return 0;
}

static int test_sample_solver(void)
{
    Board board;
    char error[128];
    char output[SUDOKU_STRING_SIZE];

    CHECK(board_parse(sample_puzzle, &board, error, sizeof(error)));
    CHECK(board_is_valid(&board));
    CHECK(solve(&board));
    CHECK(board_is_valid(&board));
    board_to_string(&board, output);
    CHECK(strcmp(output, sample_solution) == 0);
    return 0;
}

static int test_invalid_and_unsolvable_boards(void)
{
    Board board;
    char error[128];

    CHECK(board_parse(
        "110000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000",
        &board, error, sizeof(error)));
    CHECK(!board_is_valid(&board));
    Board invalid = board;
    CHECK(!solve(&board));
    CHECK(memcmp(&board, &invalid, sizeof(board)) == 0);

    CHECK(board_parse(
        "123456780"
        "000000009"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000"
        "000000000",
        &board, error, sizeof(error)));
    CHECK(board_is_valid(&board));
    Board unsolvable = board;
    CHECK(!solve(&board));
    CHECK(memcmp(&board, &unsolvable, sizeof(board)) == 0);
    return 0;
}

static int test_already_solved_board(void)
{
    Board board;
    char error[128];
    CHECK(board_parse(sample_solution, &board, error, sizeof(error)));
    CHECK(solve(&board));
    char output[SUDOKU_STRING_SIZE];
    board_to_string(&board, output);
    CHECK(strcmp(output, sample_solution) == 0);
    return 0;
}

int main(void)
{
    CHECK(test_parser() == 0);
    CHECK(test_sample_solver() == 0);
    CHECK(test_invalid_and_unsolvable_boards() == 0);
    CHECK(test_already_solved_board() == 0);
    (void)puts("All Sudoku tests passed.");
    return 0;
}
