#include "choose_screen.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
unsigned int iLoadImage(char filename[]);

// variable assign
static unsigned int sChooseBg = 0;
static unsigned int sBackButton = 0;
static unsigned int sKakashiBtn = 0;
static unsigned int sComingSoonBtn = 0;

static int sScreenW = 0;
static int sScreenH = 0;

static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 60;
static int sBackH = 60;

static int sKakashiX = 220;
static int sKakashiY = 210;
static int sKakashiW = 180;
static int sKakashiH = 180;

static int sComingX = 600;
static int sComingY = 210;
static int sComingW = 180;
static int sComingH = 180;

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

void initChooseScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  sChooseBg = iLoadImage((char *)"Images/02.1 Start/01. Choose/bg.png");
  sBackButton =
      iLoadImage((char *)"Images/02.1 Start/01. Choose/back_button.png");
  sKakashiBtn =
      iLoadImage((char *)"Images/02.1 Start/01. Choose/HatakeKakashi.png");
  sComingSoonBtn =
      iLoadImage((char *)"Images/02.1 Start/01. Choose/ComingSoon.png");

  sBackX = 30;
  sBackY = sScreenH - 90;
  sBackW = 60;
  sBackH = 60;

  sKakashiW = 210;
  sKakashiH = 210;
  sComingW = 210;
  sComingH = 210;
  sKakashiX = (sScreenW / 2) - sKakashiW - 15;
  sComingX = (sScreenW / 2) + 15;
  sKakashiY = 180;
  sComingY = 180;

  // do not see ------------------------------------
  // dust effect
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

void drawChooseScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sChooseBg);

  // do not see ------------------------------------
  // dust effect
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

  iShowImage(sKakashiX, sKakashiY, sKakashiW, sKakashiH, sKakashiBtn);
  iShowImage(sComingX, sComingY, sComingW, sComingH, sComingSoonBtn);
  iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
}

ChooseAction handleChooseClick(int mx, int my) {
  if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY &&
      my <= sBackY + sBackH) {
    return CHOOSE_BACK;
  }
  if (mx >= sKakashiX && mx <= sKakashiX + sKakashiW && my >= sKakashiY &&
      my <= sKakashiY + sKakashiH) {
    return CHOOSE_KAKASHI;
  }
  return CHOOSE_NONE;
}
