#include "loading_screen.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
unsigned int iLoadImage(char filename[]);

static unsigned int sLoadingBg = 0;
static unsigned int sLoadingStation = 0;
static unsigned int sLoadingTrain1 = 0;
static unsigned int sLoadingTrain2 = 0;
static unsigned int sLoadingTitle = 0;
static unsigned int sLoadingText = 0;

static float sTrainX = -900.0f;
static float sTrainY = -90.0f;
static float sTrainSpeed = 4.0f;
static float sTrain2X = 0.0f;
static float sTrain2Y = -90.0f;
static float sTrain2Speed = 3.0f;
static int sActiveTrain = 1;
static const int sTrainWidth = 1110;
static const int sTrainHeight = 540;
static int sTitleOffsetY = 0;
static int sTitleDir = 1;
static int sFlickerTick = 0;
static bool sShowLoadingText = true;

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

bool handleLoadingKey(unsigned char key) { return (key == 'p' || key == 'P'); }

void initLoadingScreen(int screenW, int screenH) {
  sLoadingBg = iLoadImage((char *)"Images/01.Loading Screen/bg.png");
  sLoadingStation = iLoadImage((char *)"Images/01.Loading Screen/station.png");
  sLoadingTrain1 = iLoadImage((char *)"Images/01.Loading Screen/trainLtoR.png");
  sLoadingTrain2 = iLoadImage((char *)"Images/01.Loading Screen/trainRtoL.png");
  sLoadingTitle = iLoadImage((char *)"Images/01.Loading Screen/title.png");
  sLoadingText = iLoadImage((char *)"Images/01.Loading Screen/text.png");

  sTrainX = -900.0f;
  sTrainY = -90.0f;
  sTrainSpeed = 4.0f;
  sTrain2X = (float)screenW + 100.0f;
  sTrain2Y = -90.0f;
  sTrain2Speed = 3.0f;
  sActiveTrain = 1;
  sTitleOffsetY = 0;
  sTitleDir = 1;
  sFlickerTick = 0;
  sShowLoadingText = true;

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

void drawLoadingScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sLoadingBg);
  if (sActiveTrain == 1) {
    iShowImage((int)sTrainX, (int)sTrainY, sTrainWidth, sTrainHeight,
               sLoadingTrain1);
  } else {
    iShowImage((int)sTrain2X, (int)sTrain2Y, sTrainWidth, sTrainHeight,
               sLoadingTrain2);
  }
  iShowImage(0, 0, screenW, screenH, sLoadingStation);

  // do not see ------------------------------------
  for (int i = 0; i < sDustCount; i++) {
    iSetColor(255, 150, 60);
    iFilledCircle(sDustX[i], sDustY[i], sDustR[i]);
  }
  for (int i = 0; i < sDustWhiteCount; i++) {
    iSetColor(255, 255, 255);
    iFilledCircle(sDustWhiteX[i], sDustWhiteY[i], sDustWhiteR[i]);
  }
  // do not see ------------------------------------

  iShowImage(0, sTitleOffsetY, screenW, screenH, sLoadingTitle);
  if (sShowLoadingText) {
    iShowImage(0, 0, screenW, screenH, sLoadingText);
  }

  iSetColor(255, 255, 255);
}

void updateLoadingScreen(int screenW, int screenH) {
  if (sActiveTrain == 1) {
    sTrainX += sTrainSpeed;
    if (sTrainX > screenW + 50) {
      sActiveTrain = 2;
      sTrain2X = (float)screenW + 100.0f;
    }
  } else {
    sTrain2X -= sTrain2Speed;
    if (sTrain2X < -sTrainWidth) {
      sActiveTrain = 1;
      sTrainX = -900.0f;
    }
  }

  sTitleOffsetY += sTitleDir;
  if (sTitleOffsetY > 6 || sTitleOffsetY < -6) {
    sTitleDir = -sTitleDir;
  }

  sFlickerTick++;
  if (sFlickerTick >= 20) {
    sShowLoadingText = !sShowLoadingText;
    sFlickerTick = 0;
  }

  // do not see ------------------------------------
  for (int i = 0; i < sDustCount; i++) {
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
}
