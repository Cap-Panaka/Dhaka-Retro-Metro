#include "settings_screen.h"
#include "audio.h"
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
unsigned int iLoadImage(char filename[]);

static unsigned int sSettingsBg = 0;
static unsigned int sBackButton = 0;
static unsigned int sMusicOn = 0;
static unsigned int sMusicOff = 0;
static unsigned int sSfxOn = 0;
static unsigned int sSfxOff = 0;
static unsigned int sDeleteButton = 0;

static int sScreenW = 0;
static int sScreenH = 0;

static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 80;
static int sBackH = 80;

static int sMusicX = 140;
static int sMusicY = 360;
static int sMusicW = 80;
static int sMusicH = 80;

static int sSfxX = 140;
static int sSfxY = 270;
static int sSfxW = 80;
static int sSfxH = 80;

static int sDeleteX = 140;
static int sDeleteY = 180;
static int sDeleteW = 80;
static int sDeleteH = 80;

static bool sMusicEnabled = true;
static bool sSfxEnabled = true;
static bool sDeleteArmed = false;

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

void initSettingsScreen(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  sSettingsBg = iLoadImage((char *)"Images/02.4 Settings/bg.png");
  sBackButton = iLoadImage((char *)"Images/02.4 Settings/back_button.png");
  sMusicOn = iLoadImage((char *)"Images/02.4 Settings/music_on_button.png");
  sMusicOff = iLoadImage((char *)"Images/02.4 Settings/music_off_button.png");
  sSfxOn = iLoadImage((char *)"Images/02.4 Settings/sfx_on_button.png");
  sSfxOff = iLoadImage((char *)"Images/02.4 Settings/sfx_off_button.png");
  sDeleteButton = iLoadImage((char *)"Images/02.4 Settings/delete_button.png");

  sBackX = 30;
  sBackY = sScreenH - 90;
  sBackW = 60;
  sBackH = 60;

  sMusicX = 380;
  sMusicY = 275;
  sMusicW = 80;
  sMusicH = 80;

  sSfxX = 620;
  sSfxY = 275;
  sSfxW = 80;
  sSfxH = 80;

  sDeleteX = 500;
  sDeleteY = 100;
  sDeleteW = 80;
  sDeleteH = 80;

  sMusicEnabled = true;
  sSfxEnabled = true;
  sDeleteArmed = false;

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

void drawSettingsScreen(int screenW, int screenH) {
  iClear();
  iShowImage(0, 0, screenW, screenH, sSettingsBg);

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

  if (sMusicEnabled) {
    iShowImage(sMusicX, sMusicY, sMusicW, sMusicH, sMusicOn);
  } else {
    iShowImage(sMusicX, sMusicY, sMusicW, sMusicH, sMusicOff);
  }

  if (sSfxEnabled) {
    iShowImage(sSfxX, sSfxY, sSfxW, sSfxH, sSfxOn);
  } else {
    iShowImage(sSfxX, sSfxY, sSfxW, sSfxH, sSfxOff);
  }

  iShowImage(sDeleteX, sDeleteY, sDeleteW, sDeleteH, sDeleteButton);
  iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
}

bool handleSettingsClick(int mx, int my) {
  if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY &&
      my <= sBackY + sBackH) {
    if (sSfxEnabled) {
      playClick();
    }
    return true;
  }
  if (mx >= sMusicX && mx <= sMusicX + sMusicW && my >= sMusicY &&
      my <= sMusicY + sMusicH) {
    sMusicEnabled = !sMusicEnabled;
    setMusicEnabled(sMusicEnabled);
    if (sSfxEnabled) {
      playClick();
    }
  }
  if (mx >= sSfxX && mx <= sSfxX + sSfxW && my >= sSfxY &&
      my <= sSfxY + sSfxH) {
    if (sSfxEnabled) {
      playClick();
    }
    sSfxEnabled = !sSfxEnabled;
  }
  if (mx >= sDeleteX && mx <= sDeleteX + sDeleteW && my >= sDeleteY &&
      my <= sDeleteY + sDeleteH) {
    if (sSfxEnabled) {
      playClick();
    }
    sDeleteArmed = !sDeleteArmed;
  }
  return false;
}

bool isMusicOn() { return sMusicEnabled; }

bool isSfxOn() { return sSfxEnabled; }

bool isDeleteArmed() { return sDeleteArmed; }
