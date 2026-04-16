#pragma once

void initLevel1(int screenW, int screenH);
void drawLevel1(int screenW, int screenH);
void updateLevel1();
void level1Keyboard(unsigned char key);
void level1SpecialKeyboard(unsigned char key);
void level1KeyboardUp(unsigned char key);
void level1SpecialKeyboardUp(unsigned char key);
bool isLevel1GameOver();
bool isLevel1GameWon();
void resetLevel1();
bool isLevel1TransitionReady();
