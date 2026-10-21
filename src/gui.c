#include "include/gui.h"

#include <stddef.h>

static GtkWidget *create_grid(AppState *state)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);

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
    return grid;
}

static GtkWidget *create_buttons(AppState *state)
{
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(buttons, GTK_ALIGN_CENTER);

    GtkWidget *import_button = gtk_button_new_with_label("Import");
    state->solve_button = gtk_button_new_with_label("Solve");
    state->clear_button = gtk_button_new_with_label("Clear");
    state->export_button = gtk_button_new_with_label("Export");
    gtk_widget_add_css_class(state->solve_button, "suggested-action");

    gtk_box_append(GTK_BOX(buttons), import_button);
    gtk_box_append(GTK_BOX(buttons), state->solve_button);
    gtk_box_append(GTK_BOX(buttons), state->clear_button);
    gtk_box_append(GTK_BOX(buttons), state->export_button);
    g_signal_connect(import_button, "clicked", G_CALLBACK(gui_import_clicked), state);
    g_signal_connect(state->solve_button, "clicked", G_CALLBACK(gui_solve_clicked), state);
    g_signal_connect(state->clear_button, "clicked", G_CALLBACK(gui_clear_clicked), state);
    g_signal_connect(state->export_button, "clicked", G_CALLBACK(gui_export_clicked), state);
    return buttons;
}

static GtkWidget *create_content(AppState *state)
{
    GtkWidget *outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(outer_box, 24);
    gtk_widget_set_margin_end(outer_box, 24);
    gtk_widget_set_margin_top(outer_box, 24);
    gtk_widget_set_margin_bottom(outer_box, 24);

    state->status = gtk_label_new("Enter a puzzle to solve");
    gtk_box_append(GTK_BOX(outer_box), create_grid(state));
    gtk_box_append(GTK_BOX(outer_box), create_buttons(state));
    gtk_box_append(GTK_BOX(outer_box), state->status);
    return outer_box;
}

void gui_activate(GtkApplication *application, gpointer user_data)
{
    AppState *state = user_data;
    GtkWidget *window = gtk_application_window_new(application);
    state->window = window;
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 420, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_child(GTK_WINDOW(window), create_content(state));
    gtk_window_present(GTK_WINDOW(window));
}
