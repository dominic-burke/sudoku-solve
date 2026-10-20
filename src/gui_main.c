#include "sudoku_solve.h"

#include <gtk/gtk.h>
#include <stddef.h>

typedef struct AppState
{
    GtkWidget *entries[SUDOKU_CELL_COUNT];
    GtkWidget *status;
    GtkWidget *solve_button;
    GtkWidget *clear_button;
} AppState;

static void set_status(AppState *state, const char *message, gboolean error)
{
    gtk_label_set_text(GTK_LABEL(state->status), message);
    gtk_widget_remove_css_class(state->status, error ? "success" : "error");
    gtk_widget_add_css_class(state->status, error ? "error" : "success");
}

static gboolean read_board(const AppState *state, Board *board, char *error, size_t error_size)
{
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        const char *text = gtk_editable_get_text(GTK_EDITABLE(state->entries[index]));
        if (text[0] == '\0')
        {
            board->square[index] = 0;
        }
        else if (text[1] == '\0' && text[0] >= '1' && text[0] <= '9')
        {
            board->square[index] = (unsigned short)(text[0] - '0');
        }
        else
        {
            g_snprintf(error, error_size, "Cell %zu must be empty or a digit from 1 to 9",
                       index + 1);
            return FALSE;
        }
    }

    if (!board_is_valid(board))
    {
        g_snprintf(error, error_size, "The puzzle contains duplicate digits");
        return FALSE;
    }
    return TRUE;
}

static void set_board(AppState *state, const Board *board)
{
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        char value[2] = {0};
        if (board->square[index] != 0)
        {
            value[0] = (char)('0' + board->square[index]);
        }
        gtk_editable_set_text(GTK_EDITABLE(state->entries[index]), value);
    }
}

static void solve_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    Board board;
    char error[128];

    if (!read_board(state, &board, error, sizeof(error)))
    {
        set_status(state, error, TRUE);
        return;
    }
    if (!solve(&board))
    {
        set_status(state, "This puzzle has no solution", TRUE);
        return;
    }

    set_board(state, &board);
    set_status(state, "Puzzle solved", FALSE);
}

static void clear_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    Board board = {{0}};
    set_board(state, &board);
    set_status(state, "Enter a puzzle to solve", FALSE);
}

static void activate(GtkApplication *application, gpointer user_data)
{
    AppState *state = user_data;
    GtkWidget *window = gtk_application_window_new(application);
    GtkWidget *outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    state->solve_button = gtk_button_new_with_label("Solve");
    state->clear_button = gtk_button_new_with_label("Clear");
    state->status = gtk_label_new("Enter a puzzle to solve");

    gtk_window_set_title(GTK_WINDOW(window), "Sudoku Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 420, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_margin_start(outer_box, 24);
    gtk_widget_set_margin_end(outer_box, 24);
    gtk_widget_set_margin_top(outer_box, 24);
    gtk_widget_set_margin_bottom(outer_box, 24);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);

    for (size_t row = 0; row < SUDOKU_SIZE; row++)
    {
        for (size_t column = 0; column < SUDOKU_SIZE; column++)
        {
            const size_t index = row * SUDOKU_SIZE + column;
            GtkWidget *entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 1);
            gtk_entry_set_alignment(GTK_ENTRY(entry), 0.5);
            gtk_widget_set_size_request(entry, 38, 38);
            gtk_widget_add_css_class(entry, "sudoku-cell");
            if (column % 3 == 0)
            {
                gtk_widget_add_css_class(entry, "box-left");
            }
            if (row % 3 == 0)
            {
                gtk_widget_add_css_class(entry, "box-top");
            }
            gtk_grid_attach(GTK_GRID(grid), entry, (int)column, (int)row, 1, 1);
            state->entries[index] = entry;
        }
    }

    gtk_widget_add_css_class(state->solve_button, "suggested-action");
    gtk_box_append(GTK_BOX(button_box), state->solve_button);
    gtk_box_append(GTK_BOX(button_box), state->clear_button);
    gtk_box_append(GTK_BOX(outer_box), grid);
    gtk_box_append(GTK_BOX(outer_box), button_box);
    gtk_box_append(GTK_BOX(outer_box), state->status);
    gtk_window_set_child(GTK_WINDOW(window), outer_box);

    g_signal_connect(state->solve_button, "clicked", G_CALLBACK(solve_clicked), state);
    g_signal_connect(state->clear_button, "clicked", G_CALLBACK(clear_clicked), state);
    gtk_window_present(GTK_WINDOW(window));
}

#ifndef SUDOKU_GUI_NO_MAIN
int main(int argc, char **argv)
{
    AppState state = {0};
    GtkApplication *application =
        gtk_application_new("com.example.SudokuSolve", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(activate), &state);
    const int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return status;
}
#endif
