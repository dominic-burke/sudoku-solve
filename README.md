# Sudoku solver

A small C11 Sudoku solver with a command-line interface, a reusable solver API,
and automated unit and CLI tests.

## Build and run

```sh
make
./build/sudoku_solve examples/sample_puzzle
```

The input file must contain exactly 81 puzzle cells. Use `1` through `9` for
givens and `0` or `.` for empty cells. Whitespace and line breaks are ignored.
The CLI prints the puzzle, its solution, and the solved 81-character string.
Malformed, contradictory, and unsolvable puzzles are reported with a non-zero
exit status.

## GTK 4 GUI

The optional GTK 4 application uses the same solver library as the CLI:

```sh
make gui
./build/sudoku_gui
```

Install GTK 4 development files and `pkg-config` before building the GUI. For
example, on Debian or Ubuntu:

```sh
sudo apt install build-essential pkg-config libgtk-4-dev
```

The default `make` target does not require GTK and continues to build the CLI.

## Tests

```sh
make test
```

The automated suite covers parsing, valid and invalid boards, sample and
already-solved puzzles, unsolvable puzzles, preservation of the input board on
failure, and CLI success/error behavior. Run `make showcase` to build and solve
the checked-in sample in one command. To run the GTK integration test locally,
use `make gui-test`; it exercises the solve and clear workflow and uses
`xvfb-run` automatically when available. Build artifacts are isolated under
`build/`. GitHub Actions runs both the solver tests and GTK integration test on
pushes and pull requests.

## Project layout

- `src/` — solver implementation, CLI entry point, and GTK GUI entry point
- `include/` — public solver API
- `tests/` — unit and CLI tests
- `examples/` — sample puzzle
