#include "Puzzle.hpp"

#include <iostream>

using namespace std;

int main() 
{
    string file_path = "puzzles/puzzle3.txt";
    Puzzle puzzle(file_path);
    cout << puzzle << endl;
    puzzle.solve();
    cout << "Solved: " << endl;
    cout << puzzle << endl;
}