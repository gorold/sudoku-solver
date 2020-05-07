#include "Puzzle.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <queue>

Puzzle::Puzzle(string file_path) : file_path{file_path}, grid()
{
    this->load_puzzle();
}

void Puzzle::load_puzzle() {
    string file_path = this -> file_path;
    ifstream file_stream(file_path);

    if (!file_stream.is_open()) {
        cerr << "Could not open " + file_path << endl; 
        throw domain_error("Could not open file");
    } 

    string line;
    string all_lines;
    int square_count = 0;
    while (square_count < GRID_SIZE * GRID_SIZE) {
        file_stream >> line;
        all_lines += line;
        square_count += line.size();
    }

    if (file_stream >> line) {
        cerr << file_path << " has the wrong format" << endl;
        throw domain_error ("Wrong file path");
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            string num = all_lines.substr(i * GRID_SIZE + j, 1);
            string row = GRID_ROWS.substr(i, 1);
            string col = GRID_COLS.substr(j, 1);

            if (num.compare(".") == 0 || num.compare("0") == 0) {
                this->grid[row + col] = ".";
            } else {
                this->grid[row + col] = num;
            } 
        }
    }

    file_stream.close();
}

string Puzzle::get_cell(string cell) const {
    auto it = this->grid.find(cell);
    if (it != this->grid.end()) {
        return it->second;
    }
}

int Puzzle::max_grid_length() const {
    return max_element(this->grid.begin(), this->grid.end(), [](const pair<string, string>& a, const pair<string, string>& b) {
        return a.second.size() < b.second.size();
    })->second.size();
}

ostream& operator<<(ostream& out, Puzzle puzzle) {
    int width = puzzle.max_grid_length() + 1;
    string l = "";
    for (int i = 0; i < width * 3 + 2; i++) {
        l += "-";
    }
    string line = "";
    for (int i = 0; i < 3; i++) {
        line += l;
        if (i != 2) line += "+";
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            string cell = GRID_ROWS.substr(i, 1) + GRID_COLS.substr(j, 1);
            out << setw(width) << puzzle.get_cell(cell);
            if (j == 2 || j == 5) {
                out << '|';
            } else {
                out << ' ';
            }
        }
        if (i == 2 || i == 5) {
            out << '\n' << line;
        }
        if (i != GRID_SIZE - 1) out << '\n';
    }
    return out;
}

unordered_set<string> get_row_peers(string cell) {
    string row = cell.substr(0, 1);
    unordered_set<string> out;
    for (int i=0; i<GRID_SIZE; i++) {
        string peer_cell = row + GRID_COLS.substr(i, 1);
        out.insert(peer_cell);
    }    
    out.erase(cell);
    return out;
}

unordered_set<string> get_col_peers(string cell) {
    string col = cell.substr(1, 1);
    unordered_set<string> out;
    for (int i=0; i<GRID_SIZE; i++) {
        string peer_cell = GRID_ROWS.substr(i, 1) + col;
        out.insert(peer_cell);
    }
    out.erase(cell);
    return out;
}

unordered_set<string> get_square_peers(string cell) {
    vector<string> rows({
        "ABC",
        "DEF",
        "GHI"
    });
    vector<string> cols({
        "123",
        "456",
        "789"
    });

    string row = cell.substr(0, 1);
    string col = cell.substr(1, 1);

    for (unsigned int i = 0; i < rows.size(); i++) {
        if (rows[i].find(row) != string::npos) {
            for (unsigned int j = 0; j < cols.size(); j++) {
                if (cols[j].find(col) != string::npos) {
                    unordered_set<string> out;
                    for (int k = 0; k < 3; k++) {
                        for (int l = 0; l < 3; l++) {
                            string peer_cell = rows[i].substr(k, 1) + cols[j].substr(l, 1);
                            out.insert(peer_cell);
                        }
                    }
                    out.erase(cell);
                    return out;
                }
            }
        }   
    }
}

unordered_set<string> get_peers(string cell) {
    unordered_set<string> row_peers = get_row_peers(cell);
    unordered_set<string> col_peers = get_col_peers(cell);
    unordered_set<string> square_peers = get_square_peers(cell);
    unordered_set<string> out;
    for (auto peer: row_peers) {
        out.insert(peer);
    }
    for (auto peer: col_peers) {
        out.insert(peer);
    }
    for (auto peer: square_peers) {
        out.insert(peer);
    }
    return out;
}

bool assign(map<string, string>& values, string square, string choice);

// Eliminates choice from square
bool eliminate(map<string, string>& values, string square, string choice) {
    size_t pos = values[square].find(choice);
    if (pos == string::npos) {
        return true;
    } 
    values[square] = values[square].erase(pos, 1);
    unordered_set<string> peers = get_peers(square);

    if (values[square].size() == 0) {
        // If no choices in square, there is an issue
        return false;
    } else if (values[square].size() == 1) {
        // If there is only 1 choice left this square, assign that value to the square and eliminate from peers
        bool check = true;
        for (auto it = peers.begin(); it != peers.end(); ++it) {
            check = check & eliminate(values, *it, values[square]);
        }
        if (!check) return false;
    }

    // Look through all peers
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        if (values[*it].size() == 0) {
            return false;
        } else if (values[*it].size() == 1) {
            if (assign(values, *it, values[*it])) {
                return false;
            }
        }
    }
    return true;
}

// Assigns choice to square
bool assign(map<string, string>& values, string square, string choice) {
    string current_choices = values[square];
    size_t pos = current_choices.find(choice);
    if (pos != string::npos) {
        current_choices.erase(pos, 1);
    }

    // Eliminate all other choices from this square
    bool check = true;
    for (unsigned int i = 0; i < current_choices.size(); i++) {
        check = check & eliminate(values, square, current_choices.substr(i, 1));
    }
    return check;        
}

map<string, string> parse_grid(const Puzzle& puzzle) {
    map<string, string> output_grid;
    for (unsigned int i = 0; i < GRID_ROWS.size(); i++) {
        for (unsigned int j = 0; j < GRID_COLS.size(); j++) {
            string square = GRID_ROWS.substr(i, 1) + GRID_COLS.substr(j, 1);
            output_grid[square] = DIGITS;
        }
    }

    for (auto it = output_grid.begin(); it != output_grid.end(); ++it) {
        string square = it->first;
        if (puzzle.get_cell(square).compare(".") != 0) {
            assign(output_grid, square, puzzle.get_cell(square));
        }
    }
    return output_grid;
}

typedef pair<bool, map<string, string>> search_result;

search_result search(search_result sr) {
    if (!sr.first) return search_result{false, map<string, string>{}};

    bool all_1s = true;
    string least_possibilities_square;
    int possibilities = 10;
    for (auto it = sr.second.begin(); it != sr.second.end(); ++it) {
        if (it->second.size() != 1) {
            all_1s = false;
            if (it->second.size() < possibilities) {
                least_possibilities_square = it->first;
                possibilities = it->second.size();
            }
        }
    }
    if (all_1s) return sr;

    for (unsigned int i = 0; i < sr.second[least_possibilities_square].size(); i++) {
        map<string, string> values = sr.second;
        assign(values, least_possibilities_square, values[least_possibilities_square].substr(i, 1));
        search_result new_sr = search(search_result{true, values});
        if (new_sr.first) return new_sr;
    }
    return search_result{false, map<string, string>{}};
}

void Puzzle::set_grid(map<string, string> new_grid) {
    this->grid = new_grid;
}

void Puzzle::solve() {
    map<string, string> output_values = parse_grid(*this);
    this->set_grid(output_values);
    auto [check, out] = search(search_result{true, output_values});
    if (check) this->set_grid(out);
}
