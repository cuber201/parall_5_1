#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const int ROWS = 200;
const int COLS = 200;
const int side = 800;


void printBoard(const vector<vector<bool>>& board, Mat& frame) {
    int d = side / COLS;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            //cout << (board[i][j] ? "*" : "-") << " ";
            if (board[i][j])
                circle(frame, Point(j*d+d,i * d + d), d/2, Scalar(255, 255, 255), -100);
            else
                circle(frame, Point(j * d + d, i * d + d), d/2, Scalar(0, 0, 0), -100);
        }
        //cout << endl;
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
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            board[i][j] = rand()%2;

    Mat frame = Mat::zeros(Size(side+20, side+20), CV_8UC3);
    VideoWriter outputVideo("output_video.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(side+20, side+20));

    namedWindow("Game life", WINDOW_AUTOSIZE);
    int ITERATIONS = 600;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        cout << "Итерация " << iter + 1 << ":" << endl;
        auto start = chrono::high_resolution_clock::now(); 
        iterate(board); 
        auto end = chrono::high_resolution_clock::now(); 
        chrono::duration<double, milli> elapsed = end - start; 
        printBoard(board, frame);
        cout << "Время выполнения итерации: " << elapsed.count() << " мс" << endl;
        outputVideo.write(frame);
        imshow("Game life", frame);
        if (!outputVideo.isOpened()) {
            cout << "Ошибка: не удалось открыть файл видео для записи." << endl;
            return -1;
        }

        if (waitKey(30) == 27)
        {
            break;
        }
    }
    outputVideo.release();
    waitKey(0);
    return 0;
}