#include "gui.h"

int main(int argc, char **argv)
{
    AppState state = {0};
    GtkApplication *application =
        gtk_application_new("com.example.SudokuSolve", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(gui_activate), &state);
    const int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return status;
}
