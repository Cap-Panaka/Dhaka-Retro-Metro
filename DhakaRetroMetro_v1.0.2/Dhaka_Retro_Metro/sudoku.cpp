#define _CRT_SECURE_NO_WARNINGS
#include "sudoku.h"
#include <stdio.h>
#include "glut.h"

// Forward declarations of iGraphics functions to avoid linker errors
void iClear();
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = (void*)3); // 3 is GLUT_BITMAP_8_BY_13
void iLine(double x1, double y1, double x2, double y2);

static int sGrid[9][9];
static bool sIsFixed[9][9];
static int sSelectedRow = 0;
static int sSelectedCol = 0;
static bool sIsSolved = false;

// A simple 9x9 Sudoku puzzle where only 9 numbers are missing
// 0 means empty
static int sPuzzle[9][9] = {
    {5, 3, 0, 6, 7, 8, 9, 1, 2}, // (0,2) is missing (4)
    {6, 0, 2, 1, 9, 5, 3, 4, 8}, // (1,1) is missing (7)
    {0, 9, 8, 3, 4, 2, 5, 6, 7}, // (2,0) is missing (1)
    {8, 5, 9, 0, 6, 1, 4, 2, 3}, // (3,3) is missing (7)
    {4, 2, 6, 8, 0, 3, 7, 9, 1}, // (4,4) is missing (5)
    {7, 1, 3, 9, 2, 0, 8, 5, 6}, // (5,5) is missing (4)
    {0, 6, 1, 5, 3, 7, 2, 8, 4}, // (6,0) is missing (9)
    {2, 8, 7, 0, 1, 9, 6, 3, 5}, // (7,3) is missing (4)
    {3, 4, 5, 2, 8, 6, 1, 7, 0}  // (8,8) is missing (9)
};

static int sSolution[9][9] = {
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
};

void initSudoku() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sGrid[i][j] = sPuzzle[i][j];
            sIsFixed[i][j] = (sPuzzle[i][j] != 0);
        }
    }
    sSelectedRow = 0;
    sSelectedCol = 2;
    sIsSolved = false;
}

void resetSudoku() {
    initSudoku();
}

bool checkSolved() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (sGrid[i][j] != sSolution[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void updateSudoku() {
    if (!sIsSolved) {
        if (checkSolved()) {
            sIsSolved = true;
        }
    }
}

void drawSudoku(int screenW, int screenH) {
    iClear();
    
    // Background (Dark)
    iSetColor(20, 20, 30);
    iFilledRectangle(0, 0, screenW, screenH);

    // Title
    iSetColor(255, 200, 100);
    iText(screenW / 2 - 100, screenH - 50, (char*)"NINJA SUDOKU GATE", (void*)2);
    iSetColor(200, 200, 200);
    iText(screenW / 2 - 150, screenH - 80, (char*)"Solve to unlock Level 3 Arena", (void*)1);

    int cellSize = 50; 
    int boardSize = cellSize * 9;
    int startX = (screenW - boardSize) / 2;
    int startY = (screenH - boardSize) / 2 - 20;

    // Draw Grid
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int x = startX + j * cellSize;
            int y = startY + (8 - i) * cellSize; // Flip Y for typical grid display

            // Cell Background
            if (i == sSelectedRow && j == sSelectedCol) {
                iSetColor(60, 60, 100); // Highlighted cell
            } else if (sIsFixed[i][j]) {
                iSetColor(40, 40, 50); // Fixed cell
            } else {
                iSetColor(30, 30, 40); // Empty cell
            }
            iFilledRectangle(x + 1, y + 1, cellSize - 2, cellSize - 2);

            // Cell Border
            iSetColor(100, 100, 120);
            iRectangle(x, y, cellSize, cellSize);

            // Numbers
            if (sGrid[i][j] != 0) {
                if (sIsFixed[i][j]) {
                    iSetColor(255, 255, 255); // Fixed numbers white
                } else {
                    iSetColor(100, 200, 255); // Input numbers blue
                }
                char numStr[2];
                sprintf(numStr, "%d", sGrid[i][j]);
                iText(x + 18, y + 15, numStr, (void*)2);
            }
        }
    }

    // Bold lines for 3x3 blocks
    iSetColor(200, 200, 255);
    for (int i = 0; i <= 9; i += 3) {
        iLine(startX + i * cellSize, startY, startX + i * cellSize, startY + boardSize);
        iLine(startX, startY + i * cellSize, startX + boardSize, startY + i * cellSize);
    }

    // Instructions
    iSetColor(150, 150, 150);
    iText(startX, startY - 40, (char*)"Use Arrows to navigate  |  Keys 1-9 to input  |  0/Backspace to clear");
    iText(startX, startY - 70, (char*)"Every row, column, and 3x3 box must contain 1-9.");

    if (sIsSolved) {
        iSetColor(100, 255, 100);
        iText(screenW / 2 - 80, 50, (char*)"PUZZLE SOLVED!", (void*)2);
        iSetColor(255, 255, 255);
        iText(screenW / 2 - 120, 20, (char*)"Press ENTER to Start Level 3", (void*)1);
    }
}

void sudokuKeyboard(unsigned char key) {
    if (sIsSolved) return;

    if (key >= '1' && key <= '9') {
        if (!sIsFixed[sSelectedRow][sSelectedCol]) {
            sGrid[sSelectedRow][sSelectedCol] = key - '0';
        }
    } else if (key == '0' || key == 8) { // 8 is Backspace
        if (!sIsFixed[sSelectedRow][sSelectedCol]) {
            sGrid[sSelectedRow][sSelectedCol] = 0;
        }
    }
}

void sudokuSpecialKeyboard(unsigned char key) {
    if (key == GLUT_KEY_UP) {
        if (sSelectedRow > 0) sSelectedRow--;
    } else if (key == GLUT_KEY_DOWN) {
        if (sSelectedRow < 8) sSelectedRow++;
    } else if (key == GLUT_KEY_LEFT) {
        if (sSelectedCol > 0) sSelectedCol--;
    } else if (key == GLUT_KEY_RIGHT) {
        if (sSelectedCol < 8) sSelectedCol++;
    }
}

void sudokuMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int cellSize = 50;
        int boardSize = cellSize * 9;
        int startX = (1080 - boardSize) / 2;
        int startY = (635 - boardSize) / 2 - 20;

        if (mx >= startX && mx <= startX + boardSize && my >= startY && my <= startY + boardSize) {
            sSelectedCol = (mx - startX) / cellSize;
            sSelectedRow = 8 - (my - startY) / cellSize;
        }
    }
}

bool isSudokuSolved() {
    return sIsSolved;
}
