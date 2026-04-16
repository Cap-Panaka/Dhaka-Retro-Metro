#include "menu_screen.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
unsigned int iLoadImage(char filename[]);

static unsigned int sMenuBg = 0;
static unsigned int sMenuButtons[7] = {0};
static unsigned int sSpriteFrames[6] = {0};

static int sScreenW = 0;
static int sScreenH = 0;

static int sButtonX = 180;
static int sButtonW = 260;
static int sButtonH = 60;
static int sButtonGap = 12;
static int sButtonY[7] = {0};
static int sHoverIndex = -1;
static float sHoverScale = 1.0f;
static const int sMenuItemCount = 7;

// do not see ------------------------------------
static const int sDustCount = 40;
static float sDustX[sDustCount];
static float sDustY[sDustCount];
static float sDustVX[sDustCount];
static float sDustVY[sDustCount];
static float sDustR[sDustCount];
static const int sDustWhiteCount = 30;
static float sDustWhiteX[sDustWhiteCount];
static float sDustWhiteY[sDustWhiteCount];
static float sDustWhiteVX[sDustWhiteCount];
static float sDustWhiteVY[sDustWhiteCount];
static float sDustWhiteR[sDustWhiteCount];
// do not see ------------------------------------

static int sSpriteX = 550;
static int sSpriteY = 60;
static int sSpriteW = 480;
static int sSpriteH = 480;
static int sSpriteFrame = 0;
static int sSpriteTick = 0;

static void initButtonLayout() {
  int topY = sScreenH - 160;
  for (int i = 0; i < 7; i++) {
    sButtonY[i] = topY - i * (sButtonH + sButtonGap);
  }
}

void initMenuScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  sMenuBg = iLoadImage((char *)"Images/02. Menu/bg.png");
  sMenuButtons[0] = iLoadImage((char *)"Images/02. Menu/start.png");
  sMenuButtons[1] = iLoadImage((char *)"Images/02. Menu/continue.png");
  sMenuButtons[2] = iLoadImage((char *)"Images/02. Menu/leaderboard.png");
  sMenuButtons[3] = iLoadImage((char *)"Images/02. Menu/settings.png");
  sMenuButtons[4] = iLoadImage((char *)"Images/02. Menu/help.png");
  sMenuButtons[5] = iLoadImage((char *)"Images/02. Menu/credits.png");
  sMenuButtons[6] = iLoadImage((char *)"Images/02. Menu/exit.png");

  sSpriteFrames[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft1.png");
  sSpriteFrames[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft2.png");
  sSpriteFrames[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft3.png");
  sSpriteFrames[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft4.png");
  sSpriteFrames[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft5.png");
  sSpriteFrames[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft6.png");

  sSpriteFrame = 0;
  sSpriteTick = 0;

  sHoverIndex = -1;
  sHoverScale = 1.0f;

  initButtonLayout();

  // do not see ------------------------------------
  srand((unsigned int)time(0));
  for (int i = 0; i < sDustCount; i++) {
    sDustX[i] = (float)(rand() % screenW);
    sDustY[i] = (float)(rand() % screenH);
    sDustVX[i] = -0.2f - (float)(rand() % 40) / 100.0f;
    sDustVY[i] = -0.05f - (float)(rand() % 30) / 100.0f;
    sDustR[i] = 1.0f + (float)(rand() % 20) / 10.0f;
  }
  for (int i = 0; i < sDustWhiteCount; i++) {
    sDustWhiteX[i] = (float)(rand() % screenW);
    sDustWhiteY[i] = (float)(rand() % screenH);
    sDustWhiteVX[i] = -0.15f - (float)(rand() % 30) / 120.0f;
    sDustWhiteVY[i] = -0.04f - (float)(rand() % 25) / 120.0f;
    sDustWhiteR[i] = 0.8f + (float)(rand() % 18) / 10.0f;
  }
  // do not see ------------------------------------
}

void drawMenuScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sMenuBg);

  // do not see ------------------------------------
  for (int i = 0; i < sDustCount; i++) {
    iSetColor(255, 150, 60);
    iFilledCircle(sDustX[i], sDustY[i], sDustR[i]);
    sDustX[i] += sDustVX[i];
    sDustY[i] += sDustVY[i];
    if (sDustX[i] < -20.0f || sDustY[i] < -20.0f) {
      sDustX[i] = (float)(screenW + (rand() % 200));
      sDustY[i] = (float)(rand() % screenH);
      sDustVX[i] = -0.2f - (float)(rand() % 40) / 100.0f;
      sDustVY[i] = -0.05f - (float)(rand() % 30) / 100.0f;
      sDustR[i] = 1.0f + (float)(rand() % 20) / 10.0f;
    }
  }
  for (int i = 0; i < sDustWhiteCount; i++) {
    iSetColor(255, 255, 255);
    iFilledCircle(sDustWhiteX[i], sDustWhiteY[i], sDustWhiteR[i]);
    sDustWhiteX[i] += sDustWhiteVX[i];
    sDustWhiteY[i] += sDustWhiteVY[i];
    if (sDustWhiteX[i] < -20.0f || sDustWhiteY[i] < -20.0f) {
      sDustWhiteX[i] = (float)(screenW + (rand() % 200));
      sDustWhiteY[i] = (float)(rand() % screenH);
      sDustWhiteVX[i] = -0.15f - (float)(rand() % 30) / 120.0f;
      sDustWhiteVY[i] = -0.04f - (float)(rand() % 25) / 120.0f;
      sDustWhiteR[i] = 0.8f + (float)(rand() % 18) / 10.0f;
    }
  }
  // do not see ------------------------------------

  for (int i = 0; i < 7; i++) {
    if (i == sHoverIndex) {
      int w = (int)(sButtonW * sHoverScale);
      int h = (int)(sButtonH * sHoverScale);
      int dx = sButtonX - (w - sButtonW) / 2;
      int dy = sButtonY[i] - (h - sButtonH) / 2;
      iShowImage(dx, dy, w, h, sMenuButtons[i]);
      continue;
    }
    iShowImage(sButtonX, sButtonY[i], sButtonW, sButtonH, sMenuButtons[i]);
  }

  iShowImage(sSpriteX, sSpriteY, sSpriteW, sSpriteH,
             sSpriteFrames[sSpriteFrame]);
}

void updateMenuScreen() {
  sSpriteTick++;
  if (sSpriteTick >= 4) {
    sSpriteFrame = (sSpriteFrame + 1) % 6;
    sSpriteTick = 0;
  }

  if (sHoverIndex >= 0) {
    if (sHoverScale < 1.08f) {
      sHoverScale += 0.01f;
    }
  } else {
    if (sHoverScale > 1.0f) {
      sHoverScale -= 0.01f;
    }
  }
}

void setMenuHover(int mx, int my) {
  if (mx < sButtonX || mx > sButtonX + sButtonW) {
    sHoverIndex = -1;
    return;
  }

  for (int i = 0; i < 7; i++) {
    if (my >= sButtonY[i] && my <= sButtonY[i] + sButtonH) {
      sHoverIndex = i;
      return;
    }
  }

  sHoverIndex = -1;
}

void setMenuHoverIndex(int index) {
  if (index < 0) {
    sHoverIndex = -1;
    return;
  }
  if (index >= sMenuItemCount) {
    index = sMenuItemCount - 1;
  }
  sHoverIndex = index;
}

int getMenuHoverIndex() { return sHoverIndex; }

int getMenuItemCount() { return sMenuItemCount; }

MenuAction handleMenuClick(int mx, int my) {
  if (mx < sButtonX || mx > sButtonX + sButtonW) {
    return MENU_NONE;
  }

  for (int i = 0; i < 7; i++) {
    if (my >= sButtonY[i] && my <= sButtonY[i] + sButtonH) {
      return (MenuAction)(i + 1);
    }
  }

  return MENU_NONE;
}
