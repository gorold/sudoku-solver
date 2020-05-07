#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_set>

using namespace std;

#ifndef PUZZLE_H
#define PUZZLE_H

const int GRID_SIZE = 9;
const string GRID_ROWS = "ABCDEFGHI";
const string GRID_COLS = "123456789";
const string DIGITS = "123456789";

// Contains a single instance of a sudoku puzzle
// Example:
//    Puzzle puzzle(file_path);
//    puzzle.solve();
//    cout << puzzle << endl;
class Puzzle {
  public:
    Puzzle(string file_path);
    friend ostream& operator<<(ostream& out, Puzzle puzzle);
    string get_cell(string cell) const;
    void set_grid(map<string, string> new_grid);
    int max_grid_length() const;
    void solve();

  private:
    string file_path;
    map<string, string> grid;
    void load_puzzle();
};

#endif