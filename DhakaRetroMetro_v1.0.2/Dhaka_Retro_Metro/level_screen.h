#pragma once

enum LevelAction { LEVEL_NONE = 0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_BACK };

void initLevelScreen(int screenW, int screenH);
void drawLevelScreen(int screenW, int screenH);
LevelAction handleLevelClick(int mx, int my);
void setLevelScreenHover(int mx, int my);
void setLevel4Unlocked(bool v);
bool isLevel4Unlocked();
