#define SUDOKU_GUI_NO_MAIN
#include "../src/gui_main.c"

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

static void enter_puzzle(AppState *state, const char *puzzle)
{
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        char value[2] = {puzzle[index], '\0'};
        gtk_editable_set_text(GTK_EDITABLE(state->entries[index]),
                              puzzle[index] == '0' ? "" : value);
    }
}

static int test_gui_workflow(void)
{
    AppState state = {0};
    GtkApplication *application =
        gtk_application_new("com.example.SudokuSolve.Test", G_APPLICATION_NON_UNIQUE);
    GError *error = NULL;
    CHECK(g_application_register(G_APPLICATION(application), NULL, &error));
    CHECK(error == NULL);
    activate(application, &state);
    CHECK(import_contents(&state, sample_puzzle, strlen(sample_puzzle)));
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(state.status)), "Puzzle imported") == 0);
    gtk_editable_set_text(GTK_EDITABLE(state.entries[0]), "9");
    CHECK(!import_contents(&state, "invalid", strlen("invalid")));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(state.entries[0])), "9") == 0);
    enter_puzzle(&state, sample_puzzle);

    g_signal_emit_by_name(state.solve_button, "clicked");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(state.status)), "Puzzle solved") == 0);
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(state.entries[index])),
                    (char[]){sample_solution[index], '\0'}) == 0);
    }

    g_signal_emit_by_name(state.clear_button, "clicked");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(state.status)), "Enter a puzzle to solve") == 0);
    for (size_t index = 0; index < SUDOKU_CELL_COUNT; index++)
    {
        CHECK(gtk_editable_get_text(GTK_EDITABLE(state.entries[index]))[0] == '\0');
    }

    g_object_unref(application);
    return 0;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    gtk_init();
    CHECK(test_gui_workflow() == 0);
    (void)puts("All GTK GUI tests passed.");
    return 0;
}
