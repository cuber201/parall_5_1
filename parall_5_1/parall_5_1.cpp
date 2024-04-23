#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;

const int ROWS = 20;
const int COLS = 20;


void printBoard(const vector<vector<bool>>& board) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << (board[i][j] ? "*" : "-") << " ";
        }
        cout << endl;
    }
    cout << endl;
}


void iterate(vector<vector<bool>>& board) {
    vector<vector<bool>> newBoard(ROWS, vector<bool>(COLS));

#pragma omp parallel for
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int neighbors = 0;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue; 
                    int ni = i + dx;
                    int nj = j + dy;
                    if (ni >= 0 && ni < ROWS && nj >= 0 && nj < COLS && board[ni][nj]) {
                        ++neighbors;
                    }
                }
            }
            if (board[i][j]) {
                newBoard[i][j] = (neighbors == 2 || neighbors == 3);
            }
            else {
                newBoard[i][j] = (neighbors == 3);
            }
        }
    }


    board = move(newBoard);
}

int main() {
    setlocale(LC_ALL, "Russian");
    vector<vector<bool>> board(ROWS, vector<bool>(COLS, false));
    for (int i = 0; i < ROWS/2; i++)
        for (int j = 0; j < COLS/2; j++)
            board[i][j] = true;


    int ITERATIONS = 50;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        cout << "Итерация " << iter + 1 << ":" << endl;
        auto start = chrono::high_resolution_clock::now(); 
        iterate(board); 
        auto end = chrono::high_resolution_clock::now(); 
        chrono::duration<double, milli> elapsed = end - start; 
        printBoard(board); 
        cout << "Время выполнения итерации: " << elapsed.count() << " мс" << endl;
    }

    return 0;
}