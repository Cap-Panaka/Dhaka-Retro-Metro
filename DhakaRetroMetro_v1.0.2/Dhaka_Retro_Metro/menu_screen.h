#pragma once

enum MenuAction
{
    MENU_NONE = 0,
    MENU_START,
    MENU_CONTINUE,
    MENU_LEADERBOARD,
    MENU_SETTINGS,
    MENU_HELP,
    MENU_CREDITS,
    MENU_EXIT
};

void initMenuScreen(int screenW, int screenH);
void drawMenuScreen(int screenW, int screenH);
void updateMenuScreen();
void setMenuHover(int mx, int my);
void setMenuHoverIndex(int index);
int getMenuHoverIndex();
int getMenuItemCount();
MenuAction handleMenuClick(int mx, int my);
