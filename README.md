# sudoku-solver

This is my C++ implementation of Peter Norvig's Sudoku Solver: http://norvig.com/sudoku.html.

It uses constraint propagation and depth-first search using the minimum remaining values heuristic for variable ordering.

### Usage
Change the filepath to the sudoku puzzle you wish to solve. 

Use the makefile to compile main.exe after making any changes in main.cpp.

```make
make bin/main.exe
```

Run the main.exe to solve your sudoku puzzle.
```shell
./bin/main.exe
```