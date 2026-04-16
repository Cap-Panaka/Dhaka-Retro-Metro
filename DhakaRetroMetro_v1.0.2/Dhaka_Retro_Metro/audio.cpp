#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "winmm.lib")

static char gMusicPath[MAX_PATH] = {0};
static char gClickPath[MAX_PATH] = {0};
static bool gMusicLoop = true;
static bool gMusicEnabled = false;
static bool gMusicOpened = false;

static void openMusicIfNeeded()
{
    if (gMusicOpened || gMusicPath[0] == '\0')
    {
        return;
    }

    char cmd[512];
    _snprintf_s(cmd, sizeof(cmd), _TRUNCATE, "open \"%s\" type mpegvideo alias BGM", gMusicPath);
    if (mciSendStringA(cmd, NULL, 0, NULL) == 0)
    {
        gMusicOpened = true;
    }
}

void initAudio(const char *musicPath, const char *clickPath, bool loopMusic)
{
    gMusicPath[0] = '\0';
    gClickPath[0] = '\0';
    if (musicPath != NULL)
    {
        strncpy_s(gMusicPath, MAX_PATH, musicPath, _TRUNCATE);
    }
    if (clickPath != NULL)
    {
        strncpy_s(gClickPath, MAX_PATH, clickPath, _TRUNCATE);
    }
    gMusicLoop = loopMusic;

    openMusicIfNeeded();
    if (gMusicEnabled)
    {
        setMusicEnabled(true);
    }
}

void setMusicEnabled(bool enabled)
{
    gMusicEnabled = enabled;
    openMusicIfNeeded();
    if (!gMusicOpened)
    {
        return;
    }

    if (enabled)
    {
        const char *cmd = gMusicLoop ? "play BGM repeat" : "play BGM";
        mciSendStringA(cmd, NULL, 0, NULL);
    }
    else
    {
        mciSendStringA("stop BGM", NULL, 0, NULL);
    }
}

void playClick()
{
    if (gClickPath[0] == '\0')
    {
        return;
    }
    PlaySoundA(gClickPath, NULL, SND_ASYNC | SND_FILENAME);
}

void shutdownAudio()
{
    if (gMusicOpened)
    {
        mciSendStringA("stop BGM", NULL, 0, NULL);
        mciSendStringA("close BGM", NULL, 0, NULL);
        gMusicOpened = false;
    }
}
