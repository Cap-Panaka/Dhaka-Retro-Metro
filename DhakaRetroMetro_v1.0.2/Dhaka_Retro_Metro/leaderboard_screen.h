#pragma once

void initLeaderboardScreen(int screenW, int screenH);
void drawLeaderboardScreen(int screenW, int screenH);
bool handleLeaderboardClick(int mx, int my);
void addToLeaderboard(const char* name, int timeSeconds);
int getPlayerRank(int timeSeconds);
