#include "name_input_screen.h"
#include <string.h>
#include <stdio.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font = 0);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
unsigned int iLoadImage(char filename[]);

static int sScreenW = 0;
static int sScreenH = 0;
static char sPlayerName[32] = "";
static bool sFinished = false;
static int sCursorTick = 0;
static unsigned int sBgTex = 0;

void initNameInputScreen(int screenW, int screenH) {
    sScreenW = screenW;
    sScreenH = screenH;
    sBgTex = iLoadImage((char*)"Images/02. Menu/bg.png"); // Re-use menu bg for consistency
    resetNameInput();
}

void resetNameInput() {
    sPlayerName[0] = '\0';
    sFinished = false;
    sCursorTick = 0;
}

void drawNameInputScreen(int screenW, int screenH) {
    iClear();
    if (sBgTex) iShowImage(0, 0, screenW, screenH, sBgTex);

    // Dark Overlay
    iSetColor(0, 0, 0);
    // Draw semi-transparent rectangle manually or just use dark shapes
    for(int i=0; i<screenH; i+=2) {
        iSetColor(0, 0, 0);
        iFilledRectangle(0, i, screenW, 1);
    }

    // Modal Box
    int boxW = 500;
    int boxH = 200;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2;

    iSetColor(20, 20, 30);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    
    iSetColor(100, 200, 255); // Cyan border
    iRectangle(boxX, boxY, boxW, boxH);
    iRectangle(boxX + 2, boxY + 2, boxW - 4, boxH - 4);

    // Header
    iSetColor(255, 255, 255);
    iText(boxX + 40, boxY + boxH - 50, (char*)"ENTER YOUR NINJA NAME:", (void*)2);

    // Input line
    int inputX = boxX + 40;
    int inputY = boxY + 60;
    int inputW = boxW - 80;
    
    iSetColor(40, 40, 60);
    iFilledRectangle(inputX, inputY, inputW, 40);
    iSetColor(100, 200, 255);
    iRectangle(inputX, inputY, inputW, 40);

    // Dynamic Text
    iSetColor(255, 255, 255);
    iText(inputX + 10, inputY + 12, sPlayerName, (void*)2);

    // Blinking Cursor
    sCursorTick++;
    if ((sCursorTick / 20) % 2 == 0) {
        int textOffset = strlen(sPlayerName) * 11; // Approx width
        iFilledRectangle(inputX + 15 + textOffset, inputY + 8, 10, 24);
    }

    iSetColor(150, 150, 150);
    iText(boxX + 40, boxY + 20, (char*)"PRESS ENTER TO CONFIRM", (void*)1);
}

void handleNameInputKeyboard(unsigned char key) {
    if (key == '\r') {
        if (strlen(sPlayerName) > 0) {
            sFinished = true;
        }
    } else if (key == '\b') {
        int len = strlen(sPlayerName);
        if (len > 0) {
            sPlayerName[len - 1] = '\0';
        }
    } else if (strlen(sPlayerName) < 20) {
        // Only allow alphanumeric and space
        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == ' ') {
            int len = strlen(sPlayerName);
            sPlayerName[len] = key;
            sPlayerName[len + 1] = '\0';
        }
    }
}

char* getPlayerName() {
    return sPlayerName;
}

bool isNameInputFinished() {
    return sFinished;
}
