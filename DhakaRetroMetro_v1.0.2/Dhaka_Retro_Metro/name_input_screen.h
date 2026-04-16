#pragma once

void initNameInputScreen(int screenW, int screenH);
void drawNameInputScreen(int screenW, int screenH);
void handleNameInputKeyboard(unsigned char key);
char* getPlayerName();
bool isNameInputFinished();
void resetNameInput();
