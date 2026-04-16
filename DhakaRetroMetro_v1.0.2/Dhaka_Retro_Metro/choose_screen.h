#pragma once

enum ChooseAction
{
    CHOOSE_NONE = 0,
    CHOOSE_KAKASHI,
    CHOOSE_BACK
};

void initChooseScreen(int screenW, int screenH);
void drawChooseScreen(int screenW, int screenH);
ChooseAction handleChooseClick(int mx, int my);
