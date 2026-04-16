#define _CRT_SECURE_NO_WARNINGS
#include "leaderboard_screen.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <vector>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
void iLine(double x1, double y1, double x2, double y2);
unsigned int iLoadImage(char filename[]);

static unsigned int sBoardBg = 0;
static unsigned int sBackButton = 0;

static int sScreenW = 0;
static int sScreenH = 0;

static int sBackX = 30;
static int sBackY = 540;
static int sBackW = 60;
static int sBackH = 60;

struct LeaderboardEntry {
    char name[32];
    int timeSeconds;
};

static std::vector<LeaderboardEntry> sEntries;
static bool sLeaderboardLoaded = false;

// Dust particles for "cool" theme
static const int sDustCount = 40;
static float sDustX[sDustCount];
static float sDustY[sDustCount];
static float sDustVX[sDustCount];
static float sDustVY[sDustCount];
static float sDustR[sDustCount];

void sortLeaderboard() {
    std::sort(sEntries.begin(), sEntries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.timeSeconds < b.timeSeconds;
    });
    if (sEntries.size() > 10) {
        sEntries.resize(10);
    }
}

void loadLeaderboard() {
    sEntries.clear();
    FILE* f = fopen("leaderboard.txt", "r");
    if (!f) return;

    LeaderboardEntry temp;
    while (fscanf(f, "%31[^|]|%d\n", temp.name, &temp.timeSeconds) == 2) {
        sEntries.push_back(temp);
    }
    fclose(f);
    sortLeaderboard();
    sLeaderboardLoaded = true;
}

void saveLeaderboard() {
    FILE* f = fopen("leaderboard.txt", "w");
    if (!f) return;

    for (const auto& entry : sEntries) {
        fprintf(f, "%s|%d\n", entry.name, entry.timeSeconds);
    }
    fclose(f);
}

void addToLeaderboard(const char* name, int timeSeconds) {
    if (!sLeaderboardLoaded) loadLeaderboard();
    
    LeaderboardEntry newEntry;
    strncpy(newEntry.name, name, 31);
    newEntry.name[31] = '\0';
    newEntry.timeSeconds = timeSeconds;
    
    sEntries.push_back(newEntry);
    sortLeaderboard();
    saveLeaderboard();
}

void initLeaderboardScreen(int screenW, int screenH) {
    sScreenW = screenW;
    sScreenH = screenH;

    sBoardBg = iLoadImage((char *)"Images/02.3 Leaderboard/bg.png");
    sBackButton = iLoadImage((char *)"Images/02.3 Leaderboard/back_button.png");

    sBackX = 30;
    sBackY = sScreenH - 90;
    sBackW = 60;
    sBackH = 60;

    srand((unsigned int)time(0));
    for (int i = 0; i < sDustCount; i++) {
        sDustX[i] = (float)(rand() % screenW);
        sDustY[i] = (float)(rand() % screenH);
        sDustVX[i] = -0.2f - (float)(rand() % 40) / 100.0f;
        sDustVY[i] = -0.05f - (float)(rand() % 30) / 100.0f;
        sDustR[i] = 1.0f + (float)(rand() % 20) / 10.0f;
    }
    
    loadLeaderboard();
}

void drawLeaderboardScreen(int screenW, int screenH) {
    iClear();
    if (sBoardBg) iShowImage(0, 0, screenW, screenH, sBoardBg);

    // Particle background
    for (int i = 0; i < sDustCount; i++) {
        iSetColor(255, 150, 60);
        iFilledCircle(sDustX[i], sDustY[i], sDustR[i]);
        sDustX[i] += sDustVX[i];
        sDustY[i] += sDustVY[i];
        if (sDustX[i] < -20.0f || sDustY[i] < -20.0f) {
            sDustX[i] = (float)(screenW + (rand() % 200));
            sDustY[i] = (float)(rand() % screenH);
        }
    }

    // Leaderboard UI Box
    int boxW = 800;
    int boxH = 500;
    int boxX = (screenW - boxW) / 2;
    int boxY = (screenH - boxH) / 2 - 20;

    // Outer Frame
    iSetColor(10, 10, 20);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    iSetColor(100, 200, 255);
    iRectangle(boxX, boxY, boxW, boxH);
    iRectangle(boxX + 2, boxY + 2, boxW - 4, boxH - 4);

    // Header Color
    iSetColor(100, 200, 255);
    iText(boxX + 50, boxY + boxH - 50, (char*)"RANK", (void*)2);
    iText(boxX + 200, boxY + boxH - 50, (char*)"NINJA NAME", (void*)2);
    iText(boxX + boxW - 200, boxY + boxH - 50, (char*)"TIME TAKEN", (void*)2);

    iLine(boxX + 30, boxY + boxH - 65, boxX + boxW - 30, boxY + boxH - 65);

    // Draw Entries
    for (int i = 0; i < (int)sEntries.size(); i++) {
        int entryY = boxY + boxH - 110 - (i * 40);
        
        // Ranking Highlights
        if (i == 0) iSetColor(255, 215, 0);       // Gold
        else if (i == 1) iSetColor(200, 200, 255); // Silver
        else if (i == 2) iSetColor(205, 127, 50);  // Bronze
        else iSetColor(200, 200, 200);             // Others

        char rankStr[8];
        if (i == 0) sprintf(rankStr, "1ST");
        else if (i == 1) sprintf(rankStr, "2ND");
        else if (i == 2) sprintf(rankStr, "3RD");
        else sprintf(rankStr, "%dTH", i + 1);

        iText(boxX + 50, entryY, rankStr, (void*)2);
        iText(boxX + 200, entryY, sEntries[i].name, (void*)2);

        char timeStr[16];
        int mins = sEntries[i].timeSeconds / 60;
        int secs = sEntries[i].timeSeconds % 60;
        sprintf(timeStr, "%02d:%02d", mins, secs);
        iText(boxX + boxW - 200, entryY, timeStr, (void*)2);
    }

    iShowImage(sBackX, sBackY, sBackW, sBackH, sBackButton);
}

bool handleLeaderboardClick(int mx, int my) {
    if (mx >= sBackX && mx <= sBackX + sBackW && my >= sBackY && my <= sBackY + sBackH) {
        return true;
    }
    return false;
}

int getPlayerRank(int timeSeconds) {
    if (!sLeaderboardLoaded) loadLeaderboard();
    for (int i = 0; i < (int)sEntries.size(); i++) {
        if (timeSeconds <= sEntries[i].timeSeconds) {
            return i + 1;
        }
    }
    if (sEntries.size() < 10) return (int)sEntries.size() + 1;
    return -1; // Not in top 10
}
