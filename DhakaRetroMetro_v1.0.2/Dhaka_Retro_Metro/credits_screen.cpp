#include "credits_screen.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
unsigned int iLoadImage(char filename[]);

static unsigned int sCreditsBg = 0;
static unsigned int sBackButton = 0;

static int sScreenW = 0;
static int sScreenH = 0;

static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 60;
static int sBackH = 60;

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

void initCreditsScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  sCreditsBg = iLoadImage((char *)"Images/02.6 Credits/bg.png");
  sBackButton = iLoadImage((char *)"Images/02.6 Credits/back_button.png");

  sBackX = 30;
  sBackY = sScreenH - 90;
  sBackW = 60;
  sBackH = 60;

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

void drawCreditsScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sCreditsBg);

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

  iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
}

bool handleCreditsClick(int mx, int my) {
  if (mx < sBackX || mx > sBackX + sBackW) {
    return false;
  }
  if (my < sBackY || my > sBackY + sBackH) {
    return false;
  }
  return true;
}
