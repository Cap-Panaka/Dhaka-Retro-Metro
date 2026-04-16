#pragma once

void initLevel3(int screenW, int screenH);
void drawLevel3(int screenW, int screenH);
void updateLevel3();
void level3Keyboard(unsigned char key);
void level3SpecialKeyboard(unsigned char key);
void level3KeyboardUp(unsigned char key);
void level3SpecialKeyboardUp(unsigned char key);
bool isLevel3GameOver();
bool isLevel3GameWon();
void resetLevel3();
bool isLevel3TransitionReady();
