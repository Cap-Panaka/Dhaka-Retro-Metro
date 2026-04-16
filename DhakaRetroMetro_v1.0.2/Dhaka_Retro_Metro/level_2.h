#pragma once

void initLevel2(int screenW, int screenH);
void drawLevel2(int screenW, int screenH);
void updateLevel2();
void level2Keyboard(unsigned char key);
void level2SpecialKeyboard(unsigned char key);
void level2KeyboardUp(unsigned char key);
void level2SpecialKeyboardUp(unsigned char key);
bool isLevel2GameOver();
bool isLevel2GameWon();
void resetLevel2();
bool isLevel2TransitionReady();
