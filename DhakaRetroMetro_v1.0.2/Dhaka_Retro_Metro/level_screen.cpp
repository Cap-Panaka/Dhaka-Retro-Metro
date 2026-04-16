#include "level_screen.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
unsigned int iLoadImage(char filename[]);

// Assets
static unsigned int sLevelBg = 0;
static unsigned int sBackButton = 0;
static unsigned int sLevel1Img = 0;
static unsigned int sLevel2Img = 0;
static unsigned int sLevel3Img = 0;
static unsigned int sLevel4Img = 0;

static int sScreenW = 0;
static int sScreenH = 0;
static bool sLevel4Unlocked = true;

// Back button position
static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 60;
static int sBackH = 60;

// Level button positions (2x2 grid)
static int sLevelBtnW = 200;
static int sLevelBtnH = 120;
static int sLevelGapX = 50;
static int sLevelGapY = 40;

static int sLevel1X, sLevel1Y;
static int sLevel2X, sLevel2Y;
static int sLevel3X, sLevel3Y;
static int sLevel4X, sLevel4Y;

// Dust particles (same style as choose screen)
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

void initLevelScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  sLevelBg = iLoadImage((char *)"Images/03. Level 1/bg.png");
  sBackButton =
      iLoadImage((char *)"Images/02.1 Start/01. Choose/back_button.png");

  // Load arena button images
  sLevel1Img = iLoadImage((char *)"Images/Arena/Arena 1.png");
  sLevel2Img = iLoadImage((char *)"Images/Arena/ARENA 2.png");
  sLevel3Img = iLoadImage((char *)"Images/Arena/ARENA 3.png");
  sLevel4Img = iLoadImage((char *)"Images/Arena/ARENA_4.png");

  sBackX = 30;
  sBackY = sScreenH - 90;
  sBackW = 60;
  sBackH = 60;

  // 2x2 grid layout: centered
  int totalW = 2 * sLevelBtnW + sLevelGapX;
  int totalH = 2 * sLevelBtnH + sLevelGapY;
  int startX = (sScreenW - totalW) / 2;
  int startY = (sScreenH - totalH) / 2 - 20; // Shift down a bit for title

  // Top row: Arena 1, Arena 2
  sLevel1X = startX;
  sLevel1Y = startY + sLevelBtnH + sLevelGapY;
  sLevel2X = startX + sLevelBtnW + sLevelGapX;
  sLevel2Y = startY + sLevelBtnH + sLevelGapY;

  // Bottom row: Arena 3, Arena 4
  sLevel3X = startX;
  sLevel3Y = startY;
  sLevel4X = startX + sLevelBtnW + sLevelGapX;
  sLevel4Y = startY;

  // Init dust particles
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
}

// Hover state
static int sHoveredLevel = 0; // 0=none, 1/2/3/4=level, 5=back

void setLevelScreenHover(int mx, int my) {
  sHoveredLevel = 0;
  if (mx >= sLevel1X && mx <= sLevel1X + sLevelBtnW && my >= sLevel1Y &&
      my <= sLevel1Y + sLevelBtnH)
    sHoveredLevel = 1;
  else if (mx >= sLevel2X && mx <= sLevel2X + sLevelBtnW && my >= sLevel2Y &&
           my <= sLevel2Y + sLevelBtnH)
    sHoveredLevel = 2;
  else if (mx >= sLevel3X && mx <= sLevel3X + sLevelBtnW && my >= sLevel3Y &&
           my <= sLevel3Y + sLevelBtnH)
    sHoveredLevel = 3;
  else if (mx >= sLevel4X && mx <= sLevel4X + sLevelBtnW && my >= sLevel4Y &&
           my <= sLevel4Y + sLevelBtnH)
    sHoveredLevel = 4;
  else if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY &&
           my <= sBackY + sBackH)
    sHoveredLevel = 5;
}

static void drawHoverBorder(int x, int y, int w, int h) {
  // Glowing orange border on hover
  iSetColor(255, 180, 60);
  iRectangle(x - 3, y - 3, w + 6, h + 6);
  iRectangle(x - 2, y - 2, w + 4, h + 4);
  iSetColor(255, 220, 120);
  iRectangle(x - 1, y - 1, w + 2, h + 2);
}

static void drawLockedOverlay(int x, int y, int w, int h, unsigned int img) {
  // Dark overlay + dimmed image + LOCKED text
  iSetColor(0, 0, 0);
  iFilledRectangle(x, y, w, h);
  iShowImage(x, y, w, h, img);
  iSetColor(0, 0, 0);
  iFilledRectangle(x + 1, y + 1, w - 2, h - 2);
  iShowImage(x + 2, y + 2, w - 4, h - 4, img);
  iSetColor(255, 100, 100);
  iText((double)(x + w / 2 - 25), (double)(y + h / 2 - 6), (char *)"LOCKED");
}

void drawLevelScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sLevelBg);

  // Dust particles
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

  // Title
  iSetColor(255, 200, 100);
  iText((double)(screenW / 2 - 80), (double)(screenH - 80),
        (char *)"SELECT ARENA", (void *)2);

  // Draw Arena 1 & 2 (unlocked)
  iShowImage(sLevel1X, sLevel1Y, sLevelBtnW, sLevelBtnH, sLevel1Img);
  iShowImage(sLevel2X, sLevel2Y, sLevelBtnW, sLevelBtnH, sLevel2Img);

  // Hover borders on unlocked arenas
  if (sHoveredLevel == 1)
    drawHoverBorder(sLevel1X, sLevel1Y, sLevelBtnW, sLevelBtnH);
  if (sHoveredLevel == 2)
    drawHoverBorder(sLevel2X, sLevel2Y, sLevelBtnW, sLevelBtnH);

  // Labels under unlocked arenas
  iSetColor(255, 255, 255);
  iText((double)(sLevel1X + sLevelBtnW / 2 - 30), (double)(sLevel1Y - 22),
        (char *)"ARENA 1");
  iText((double)(sLevel2X + sLevelBtnW / 2 - 30), (double)(sLevel2Y - 22),
        (char *)"ARENA 2");

  // Draw Arena 3 (unlocked)
  iShowImage(sLevel3X, sLevel3Y, sLevelBtnW, sLevelBtnH, sLevel3Img);

  // Draw Arena 4 (conditionally locked/unlocked)
  if (sLevel4Unlocked) {
    iShowImage(sLevel4X, sLevel4Y, sLevelBtnW, sLevelBtnH, sLevel4Img);
  } else {
    drawLockedOverlay(sLevel4X, sLevel4Y, sLevelBtnW, sLevelBtnH, sLevel4Img);
  }

  // Hover borders
  if (sHoveredLevel == 3)
    drawHoverBorder(sLevel3X, sLevel3Y, sLevelBtnW, sLevelBtnH);
  if (sHoveredLevel == 4 && sLevel4Unlocked)
    drawHoverBorder(sLevel4X, sLevel4Y, sLevelBtnW, sLevelBtnH);

  // Labels under arenas
  iSetColor(255, 255, 255);
  iText((double)(sLevel3X + sLevelBtnW / 2 - 30), (double)(sLevel3Y - 22),
        (char *)"ARENA 3");
  if (sLevel4Unlocked) {
    iSetColor(255, 200, 100);
    iText((double)(sLevel4X + sLevelBtnW / 2 - 30), (double)(sLevel4Y - 22),
          (char *)"ARENA 4");
  } else {
    iSetColor(180, 180, 180);
    iText((double)(sLevel4X + sLevelBtnW / 2 - 30), (double)(sLevel4Y - 22),
          (char *)"LOCKED");
  }

  // Back button
  iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
  if (sHoveredLevel == 5)
    drawHoverBorder(sBackX, sBackY, sBackW, sBackH);

  // Instructions
  iSetColor(200, 200, 200);
  iText((double)(screenW / 2 - 100), 20.0,
        (char *)"Click an arena to start  |  ESC to go back");
}

LevelAction handleLevelClick(int mx, int my) {
  if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY &&
      my <= sBackY + sBackH) {
    return LEVEL_BACK;
  }
  if (mx >= sLevel1X && mx <= sLevel1X + sLevelBtnW && my >= sLevel1Y &&
      my <= sLevel1Y + sLevelBtnH) {
    return LEVEL_1;
  }
  if (mx >= sLevel2X && mx <= sLevel2X + sLevelBtnW && my >= sLevel2Y &&
      my <= sLevel2Y + sLevelBtnH) {
    return LEVEL_2;
  }
  if (mx >= sLevel3X && mx <= sLevel3X + sLevelBtnW && my >= sLevel3Y &&
      my <= sLevel3Y + sLevelBtnH) {
    return LEVEL_3;
  }
  if (sLevel4Unlocked && mx >= sLevel4X && mx <= sLevel4X + sLevelBtnW &&
      my >= sLevel4Y && my <= sLevel4Y + sLevelBtnH) {
    return LEVEL_4;
  }
  return LEVEL_NONE;
}

void setLevel4Unlocked(bool v) { sLevel4Unlocked = v; }
bool isLevel4Unlocked() { return sLevel4Unlocked; }
