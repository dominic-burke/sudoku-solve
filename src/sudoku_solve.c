#include "sudoku_solve.h"

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#define BOX_SIZE 3

static void set_error(char *error, size_t error_size, const char *format, ...)
{
    if (error == NULL || error_size == 0)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    (void)vsnprintf(error, error_size, format, args);
    va_end(args);
}

bool board_parse(const char *input, Board *board, char *error, size_t error_size)
{
    if (input == NULL || board == NULL)
    {
        set_error(error, error_size, "input and board must not be null");
        return false;
    }

    Board parsed = {{0}};
    size_t cell = 0;

    for (const unsigned char *cursor = (const unsigned char *)input; *cursor != '\0'; cursor++)
    {
        if (isspace(*cursor))
        {
            continue;
        }

        if (cell == SUDOKU_CELL_COUNT)
        {
            set_error(error, error_size, "puzzle contains more than %d cells", SUDOKU_CELL_COUNT);
            return false;
        }

        if (*cursor == '.' || *cursor == '0')
        {
            parsed.square[cell++] = 0;
        }
        else if (*cursor >= '1' && *cursor <= '9')
        {
            parsed.square[cell++] = (unsigned short)(*cursor - '0');
        }
        else
        {
            set_error(error, error_size, "invalid puzzle character at cell %zu", cell + 1);
            return false;
        }
    }

    if (cell != SUDOKU_CELL_COUNT)
    {
        set_error(error, error_size, "puzzle contains %zu cells; expected %d", cell, SUDOKU_CELL_COUNT);
        return false;
    }

    *board = parsed;
    if (error != NULL && error_size > 0)
    {
        error[0] = '\0';
    }
    return true;
}

bool board_is_valid(const Board *board)
{
    if (board == NULL)
    {
        return false;
    }

    unsigned short rows[SUDOKU_SIZE] = {0};
    unsigned short columns[SUDOKU_SIZE] = {0};
    unsigned short boxes[SUDOKU_SIZE] = {0};

    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        const unsigned short value = board->square[index];
        if (value > SUDOKU_SIZE)
        {
            return false;
        }
        if (value == 0)
        {
            continue;
        }

        const size_t row = index / SUDOKU_SIZE;
        const size_t column = index % SUDOKU_SIZE;
        const size_t box = (row / BOX_SIZE) * BOX_SIZE + (column / BOX_SIZE);
        const unsigned short bit = (unsigned short)(1U << value);
        if ((rows[row] & bit) != 0 || (columns[column] & bit) != 0 || (boxes[box] & bit) != 0)
        {
            return false;
        }

        rows[row] |= bit;
        columns[column] |= bit;
        boxes[box] |= bit;
    }

    return true;
}

static bool solve_recursive(Board *board)
{
    size_t empty_index = SUDOKU_CELL_COUNT;
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        if (board->square[index] == 0)
        {
            empty_index = index;
            break;
        }
    }
    if (empty_index == SUDOKU_CELL_COUNT)
    {
        return true;
    }

    for (unsigned short value = 1; value <= SUDOKU_SIZE; value++)
    {
        board->square[empty_index] = value;
        if (board_is_valid(board) && solve_recursive(board))
        {
            return true;
        }
    }

    board->square[empty_index] = 0;
    return false;
}

bool solve(Board *board)
{
    if (!board_is_valid(board))
    {
        return false;
    }

    Board original = *board;
    if (solve_recursive(board))
    {
        return true;
    }

    *board = original;
    return false;
}

void board_print(FILE *stream, const Board *board)
{
    if (stream == NULL || board == NULL)
    {
        return;
    }

    for (size_t row = 0; row < SUDOKU_SIZE; row++)
    {
        if (row % BOX_SIZE == 0)
        {
            (void)fputs("+-------+-------+-------+\n", stream);
        }
        for (size_t column = 0; column < SUDOKU_SIZE; column++)
        {
            if (column % BOX_SIZE == 0)
            {
                (void)fputc('|', stream);
            }

            const unsigned short value = board->square[row * SUDOKU_SIZE + column];
            (void)fprintf(stream, "%c ", value == 0 ? '.' : (char)('0' + value));
        }
        (void)fputs("|\n", stream);
    }
    (void)fputs("+-------+-------+-------+\n", stream);
}

void board_to_string(const Board *board, char output[SUDOKU_STRING_SIZE])
{
    if (board == NULL || output == NULL)
    {
        return;
    }

    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        output[index] = (char)('0' + board->square[index]);
    }
    output[SUDOKU_CELL_COUNT] = '\0';
}
