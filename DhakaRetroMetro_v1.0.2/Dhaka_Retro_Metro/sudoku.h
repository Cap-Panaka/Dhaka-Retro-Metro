#ifndef SUDOKU_H
#define SUDOKU_H

void initSudoku();
void updateSudoku();
void drawSudoku(int screenW, int screenH);
void sudokuKeyboard(unsigned char key);
void sudokuSpecialKeyboard(unsigned char key);
void sudokuMouse(int button, int state, int mx, int my);
bool isSudokuSolved();
void resetSudoku();

#endif
