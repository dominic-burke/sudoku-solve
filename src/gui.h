#ifndef SUDOKU_GUI_H
#define SUDOKU_GUI_H

#include "sudoku_solve.h"

#include <gtk/gtk.h>

typedef struct AppState
{
    GtkWidget *entries[SUDOKU_CELL_COUNT];
    GtkWidget *status;
    GtkWidget *solve_button;
    GtkWidget *clear_button;
    GtkWidget *export_button;
    GtkWidget *window;
} AppState;

void gui_activate(GtkApplication *application, gpointer user_data);
void gui_set_status(AppState *state, const char *message, gboolean error);
gboolean gui_read_board(const AppState *state, Board *board, char *error, size_t error_size);
void gui_set_board(AppState *state, const Board *board);
gboolean gui_import_contents(AppState *state, const char *contents, size_t length);
gboolean gui_export_contents(const AppState *state, char output[SUDOKU_STRING_SIZE],
                             char *error, size_t error_size);
void gui_import_clicked(GtkButton *button, gpointer user_data);
void gui_export_clicked(GtkButton *button, gpointer user_data);
void gui_solve_clicked(GtkButton *button, gpointer user_data);
void gui_clear_clicked(GtkButton *button, gpointer user_data);

#endif
