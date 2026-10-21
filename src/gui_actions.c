#include "include/gui.h"

#include <stddef.h>

void gui_set_status(AppState *state, const char *message, gboolean error)
{
    gtk_label_set_text(GTK_LABEL(state->status), message);
    gtk_widget_remove_css_class(state->status, error ? "success" : "error");
    gtk_widget_add_css_class(state->status, error ? "error" : "success");
}

gboolean gui_read_board(const AppState *state, Board *board, char *error, size_t error_size)
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

void gui_set_board(AppState *state, const Board *board)
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

gboolean gui_import_contents(AppState *state, const char *contents, size_t length)
{
    char *input = g_strndup(contents, length);
    Board board;
    char error[128];
    const gboolean parsed = board_parse(input, &board, error, sizeof(error));
    g_free(input);

    if (!parsed)
    {
        gui_set_status(state, error, TRUE);
        return FALSE;
    }
    if (!board_is_valid(&board))
    {
        gui_set_status(state, "The imported puzzle contains duplicate digits", TRUE);
        return FALSE;
    }

    gui_set_board(state, &board);
    gui_set_status(state, "Puzzle imported", FALSE);
    return TRUE;
}

static void import_file_finished(GObject *source_object, GAsyncResult *result,
                                 gpointer user_data)
{
    AppState *state = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source_object), result, &error);
    if (file == NULL)
    {
        if (!g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
        {
            gui_set_status(state, error->message, TRUE);
        }
        g_clear_error(&error);
        return;
    }

    char *contents = NULL;
    gsize length = 0;
    if (!g_file_load_contents(file, NULL, &contents, &length, NULL, &error))
    {
        gui_set_status(state, error->message, TRUE);
        g_clear_error(&error);
        g_object_unref(file);
        return;
    }

    (void)gui_import_contents(state, contents, length);
    g_free(contents);
    g_object_unref(file);
}

void gui_import_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Import Sudoku Puzzle");
    gtk_file_dialog_open(dialog, GTK_WINDOW(state->window), NULL, import_file_finished, state);
    g_object_unref(dialog);
}

gboolean gui_export_contents(const AppState *state, char output[SUDOKU_STRING_SIZE],
                             char *error, size_t error_size)
{
    Board board;
    if (!gui_read_board(state, &board, error, error_size))
    {
        return FALSE;
    }

    board_to_string(&board, output);
    return TRUE;
}

static void export_file_finished(GObject *source_object, GAsyncResult *result,
                                 gpointer user_data)
{
    AppState *state = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source_object), result, &error);
    if (file == NULL)
    {
        if (!g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
        {
            gui_set_status(state, error->message, TRUE);
        }
        g_clear_error(&error);
        return;
    }

    char contents[SUDOKU_STRING_SIZE];
    char validation_error[128];
    if (!gui_export_contents(state, contents, validation_error, sizeof(validation_error)))
    {
        gui_set_status(state, validation_error, TRUE);
        g_object_unref(file);
        return;
    }

    if (!g_file_replace_contents(file, contents, SUDOKU_CELL_COUNT, NULL, FALSE,
                                 G_FILE_CREATE_NONE, NULL, NULL, &error))
    {
        gui_set_status(state, error->message, TRUE);
        g_clear_error(&error);
        g_object_unref(file);
        return;
    }

    gui_set_status(state, "Puzzle exported", FALSE);
    g_object_unref(file);
}

void gui_export_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Export Sudoku Puzzle");
    gtk_file_dialog_set_initial_name(dialog, "sudoku.txt");
    gtk_file_dialog_save(dialog, GTK_WINDOW(state->window), NULL, export_file_finished, state);
    g_object_unref(dialog);
}

void gui_solve_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    Board board;
    char error[128];

    if (!gui_read_board(state, &board, error, sizeof(error)))
    {
        gui_set_status(state, error, TRUE);
        return;
    }
    if (!solve(&board))
    {
        gui_set_status(state, "This puzzle has no solution", TRUE);
        return;
    }

    gui_set_board(state, &board);
    gui_set_status(state, "Puzzle solved", FALSE);
}

void gui_clear_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppState *state = user_data;
    Board board = {{0}};
    gui_set_board(state, &board);
    gui_set_status(state, "Enter a puzzle to solve", FALSE);
}
