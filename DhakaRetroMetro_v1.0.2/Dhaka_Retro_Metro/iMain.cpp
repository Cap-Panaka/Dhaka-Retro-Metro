#define _CRT_SECURE_NO_WARNINGS
#include "audio.h"
#include "choose_screen.h"
#include "credits_screen.h"
#include "help_screen.h"
#include "iGraphics.h"
#include "leaderboard_screen.h"
// level_1.h removed
#include "level_screen.h"
#include "loading_screen.h"
#include "menu_screen.h"
#include "settings_screen.h"
#include "name_input_screen.h"
#include "sudoku.h"

const int SCREEN_W = 1080;
const int SCREEN_H = 635;

// Forward declarations for Level 1 functions
void initLevel1(int screenW, int screenH);
void updateLevel1();
void drawLevel1(int screenW, int screenH);
void level1Keyboard(unsigned char key);
void level1SpecialKeyboard(unsigned char key);
void level1KeyboardUp(unsigned char key);
void level1SpecialKeyboardUp(unsigned char key);
bool isLevel1GameOver();
bool isLevel1GameWon();
void resetLevel1();
bool isLevel1TransitionReady();

// Forward declarations for Level 2 functions
void initLevel2(int screenW, int screenH);
void updateLevel2();
void drawLevel2(int screenW, int screenH);
void level2Keyboard(unsigned char key);
void level2SpecialKeyboard(unsigned char key);
void level2KeyboardUp(unsigned char key);
void level2SpecialKeyboardUp(unsigned char key);
bool isLevel2GameOver();
bool isLevel2GameWon();
void resetLevel2();
bool isLevel2TransitionReady();

// Forward declarations for Level 3 functions
void initLevel3(int screenW, int screenH);
void updateLevel3();
void drawLevel3(int screenW, int screenH);
void level3Keyboard(unsigned char key);
void level3SpecialKeyboard(unsigned char key);
void level3KeyboardUp(unsigned char key);
void level3SpecialKeyboardUp(unsigned char key);
bool isLevel3GameOver();
bool isLevel3GameWon();
void resetLevel3();
bool isLevel3TransitionReady();

// Forward declarations for Level 4 functions
void initLevel4(int screenW, int screenH);
void updateLevel4();
void drawLevel4(int screenW, int screenH);
void level4Keyboard(unsigned char key);
void level4SpecialKeyboard(unsigned char key);
void level4KeyboardUp(unsigned char key);
void level4SpecialKeyboardUp(unsigned char key);
bool isLevel4GameOver();
bool isLevel4GameWon();
void resetLevel4();

// Here i'll add music
// const char *MUSIC_PATH = "audio/music.wav";
// const char *CLICK_PATH = "audio/click.wav";

enum GameState {
  STATE_LOADING,
  STATE_MENU,
  STATE_START_CHOOSE,
  STATE_LEVEL_SELECT,
  STATE_LEADERBOARD,
  STATE_HELP,
  STATE_CREDITS,
  STATE_SETTINGS,
  STATE_GAME,
  STATE_LEVEL_LOADING,
  STATE_SUDOKU,
  STATE_NAME_INPUT
};

GameState gameState = STATE_LOADING;
int selectedLevel = 1;

static double sGlobalTimer = 0.0;
static bool sScoreSaved = false;
static int sLevelTransitionTimer = 0;
static bool sIsTransitioning = false;

static unsigned int sTransitionLoadingTex = 0;

void iDraw() {
  if (gameState == STATE_LOADING) {
    drawLoadingScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_LEVEL_LOADING) {
    if (sTransitionLoadingTex) {
      iShowImage(0, 0, SCREEN_W, SCREEN_H, sTransitionLoadingTex);
    } else {
      drawLoadingScreen(SCREEN_W, SCREEN_H); // Fallback
    }
    iSetColor(255, 255, 255);
    iText(SCREEN_W - 200, 30, (char *)"LOADING ASSETS...", (void *)2);
    return;
  }
  if (gameState == STATE_NAME_INPUT) {
    drawNameInputScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_SUDOKU) {
    drawSudoku(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_MENU) {
    drawMenuScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_START_CHOOSE) {
    drawChooseScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_LEVEL_SELECT) {
    drawLevelScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_LEADERBOARD) {
    drawLeaderboardScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_HELP) {
    drawHelpScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_CREDITS) {
    drawCreditsScreen(SCREEN_W, SCREEN_H);
    return;
  }
  if (gameState == STATE_SETTINGS) {
    drawSettingsScreen(SCREEN_W, SCREEN_H);
    return;
  }

  // STATE_GAME
  if (selectedLevel == 1) {
    drawLevel1(SCREEN_W, SCREEN_H);
  } else if (selectedLevel == 2) {
    drawLevel2(SCREEN_W, SCREEN_H);
  } else if (selectedLevel == 3) {
    drawLevel3(SCREEN_W, SCREEN_H);
  } else if (selectedLevel == 4) {
    drawLevel4(SCREEN_W, SCREEN_H);
  }

  // Draw HUD (Live Timer)
  if (gameState == STATE_GAME) {
    char timeStr[32];
    int mins = (int)sGlobalTimer / 60;
    int secs = (int)sGlobalTimer % 60;
    sprintf(timeStr, "NINJA TIME: %02d:%02d", mins, secs);
    iSetColor(100, 200, 255);
    iText(SCREEN_W - 220, SCREEN_H - 40, timeStr, (void *)2);
    
    // Show Rank Feedback if won
    if (selectedLevel == 4 && isLevel4GameWon()) {
        int rank = getPlayerRank((int)sGlobalTimer);
        char rankMsg[64];
        if (rank > 0) sprintf(rankMsg, "NEW RECORD! RANK #%d ON METRO LEGENDS", rank);
        else sprintf(rankMsg, "VICTORY! FINISHED IN %02d:%02d", mins, secs);
        
        iSetColor(255, 215, 0);
        iText(SCREEN_W / 2 - 150, SCREEN_H / 2 + 50, rankMsg, (void *)2);
    }
  }
}

void iMouseMove(int mx, int my) {
  if (gameState == STATE_MENU) {
    setMenuHover(mx, my);
  }
  if (gameState == STATE_LEVEL_SELECT) {
    setLevelScreenHover(mx, my);
  }
}

void iPassiveMouseMove(int mx, int my) {
  if (gameState == STATE_MENU) {
    setMenuHover(mx, my);
  }
  if (gameState == STATE_LEVEL_SELECT) {
    setLevelScreenHover(mx, my);
  }
}

void iMouse(int button, int state, int mx, int my) {
  if (gameState == STATE_MENU && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    MenuAction action = handleMenuClick(mx, my);
    if (action == MENU_START || action == MENU_CONTINUE) {
      if (isSfxOn()) {
        playClick();
      }
      if (action == MENU_START) {
          gameState = STATE_NAME_INPUT;
          resetNameInput();
          sGlobalTimer = 0.0;
          sScoreSaved = false;
      } else {
          gameState = STATE_GAME;
      }
    } else if (action == MENU_LEADERBOARD) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_LEADERBOARD;
    } else if (action == MENU_HELP) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_HELP;
    } else if (action == MENU_CREDITS) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_CREDITS;
    } else if (action == MENU_SETTINGS) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_SETTINGS;
    } else if (action == MENU_EXIT) {
      if (isSfxOn()) {
        playClick();
      }
      exit(0);
    }
    return;
  }
  if (gameState == STATE_START_CHOOSE && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    ChooseAction action = handleChooseClick(mx, my);
    if (action == CHOOSE_BACK) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    } else if (action == CHOOSE_KAKASHI) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_LEVEL_SELECT;
    }
    return;
  }
  if (gameState == STATE_LEVEL_SELECT && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    LevelAction levelAction = handleLevelClick(mx, my);
    if (levelAction == LEVEL_BACK) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_START_CHOOSE;
    } else if (levelAction == LEVEL_1 || levelAction == LEVEL_2 ||
               levelAction == LEVEL_3 || levelAction == LEVEL_4) {
      if (isSfxOn()) {
        playClick();
      }
      if (levelAction == LEVEL_1) selectedLevel = 1;
      else if (levelAction == LEVEL_2) selectedLevel = 2;
      else if (levelAction == LEVEL_3) selectedLevel = 3;
      else if (levelAction == LEVEL_4) selectedLevel = 4;
      
      gameState = STATE_LEVEL_LOADING;
    }
    return;
  }
  if (gameState == STATE_LEADERBOARD && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    if (handleLeaderboardClick(mx, my)) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_HELP && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    if (handleHelpClick(mx, my)) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_CREDITS && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    if (handleCreditsClick(mx, my)) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_SETTINGS && button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {
    if (handleSettingsClick(mx, my)) {
      gameState = STATE_MENU;
    }
    return;
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      if (gameState == STATE_SUDOKU) {
          sudokuMouse(button, state, mx, my);
          return;
      }
  }

  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
  }
}

void iKeyboard(unsigned char key) {
  if (gameState == STATE_LOADING) {
    if (handleLoadingKey(key)) {
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_MENU) {
    if (key == 'w' || key == 'W' || key == 's' || key == 'S') {
      int count = getMenuItemCount();
      int index = getMenuHoverIndex();
      if (index < 0) {
        index = 0;
      } else if (key == 'w' || key == 'W') {
        index = (index - 1 + count) % count;
      } else {
        index = (index + 1) % count;
      }
      setMenuHoverIndex(index);
    }
    if (key == '\r' || key == ' ') {
      int index = getMenuHoverIndex();
      if (index < 0) {
        index = 0;
      }
      MenuAction action = (MenuAction)(index + 1);
      if (action == MENU_START || action == MENU_CONTINUE) {
        if (isSfxOn()) {
          playClick();
        }
        if (action == MENU_START) {
          gameState = STATE_NAME_INPUT;
          resetNameInput();
          sGlobalTimer = 0.0;
          sScoreSaved = false;
        } else {
          gameState = STATE_GAME;
        }
      } else if (action == MENU_LEADERBOARD) {
        if (isSfxOn()) {
          playClick();
        }
        gameState = STATE_LEADERBOARD;
      } else if (action == MENU_HELP) {
        if (isSfxOn()) {
          playClick();
        }
        gameState = STATE_HELP;
      } else if (action == MENU_CREDITS) {
        if (isSfxOn()) {
          playClick();
        }
        gameState = STATE_CREDITS;
      } else if (action == MENU_SETTINGS) {
        if (isSfxOn()) {
          playClick();
        }
        gameState = STATE_SETTINGS;
      } else if (action == MENU_EXIT) {
        if (isSfxOn()) {
          playClick();
        }
        exit(0);
      }
    }
    return;
  }
  if (gameState == STATE_NAME_INPUT) {
    handleNameInputKeyboard(key);
    if (isNameInputFinished()) {
      gameState = STATE_START_CHOOSE;
    }
    return;
  }
  if (gameState == STATE_START_CHOOSE) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_LEVEL_SELECT) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_START_CHOOSE;
    }
    return;
  }
  if (gameState == STATE_LEADERBOARD) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_HELP) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_CREDITS) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }
  if (gameState == STATE_SUDOKU) {
    sudokuKeyboard(key);
    if (key == '\r' && isSudokuSolved()) {
      gameState = STATE_GAME;
    }
    return;
  }
  if (gameState == STATE_SETTINGS) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      gameState = STATE_MENU;
    }
    return;
  }

  // In-game keyboard
  if (gameState == STATE_GAME) {
    if (key == 27) {
      if (isSfxOn()) {
        playClick();
      }
      if (selectedLevel == 1)
        resetLevel1();
      else if (selectedLevel == 2)
        resetLevel2();
      else if (selectedLevel == 3)
        resetLevel3();
      else if (selectedLevel == 4)
        resetLevel4();
      gameState = STATE_MENU;
      return;
    }

    // Return to menu on Game Over or Win
    if (key == 'm' || key == 'M') {
      if (selectedLevel == 1) {
        if (isLevel1GameOver() || isLevel1GameWon()) {
          resetLevel1();
          gameState = STATE_MENU;
          return;
        }
      } else if (selectedLevel == 2) {
        if (isLevel2GameOver() || isLevel2GameWon()) {
          resetLevel2();
          gameState = STATE_MENU;
          return;
        }
      } else if (selectedLevel == 3) {
        if (isLevel3GameOver() || isLevel3GameWon()) {
          resetLevel3();
          gameState = STATE_MENU;
          return;
        }
      } else if (selectedLevel == 4) {
        if (isLevel4GameOver() || isLevel4GameWon()) {
          resetLevel4();
          gameState = STATE_MENU;
          return;
        }
      }
    }

    if (selectedLevel == 1)
      level1Keyboard(key);
    else if (selectedLevel == 2)
      level2Keyboard(key);
    else if (selectedLevel == 3)
      level3Keyboard(key);
    else if (selectedLevel == 4)
      level4Keyboard(key);
  }
}

void iSpecialKeyboard(unsigned char key) {
  if (gameState == STATE_MENU) {
    if (key == 101 || key == 103) { // 101: UP, 103: DOWN
      int count = getMenuItemCount();
      int index = getMenuHoverIndex();
      if (index < 0) {
        index = 0;
      } else if (key == 101) {
        index = (index - 1 + count) % count;
      } else {
        index = (index + 1) % count;
      }
      setMenuHoverIndex(index);
    }
    return;
  }
  if (gameState == STATE_SUDOKU) {
    sudokuSpecialKeyboard(key);
    return;
  }

  if (gameState == STATE_GAME) {
    if (selectedLevel == 1)
      level1SpecialKeyboard(key);
    else if (selectedLevel == 2)
      level2SpecialKeyboard(key);
    else if (selectedLevel == 3)
      level3SpecialKeyboard(key);
    else if (selectedLevel == 4)
      level4SpecialKeyboard(key);
  }
}

void iKeyboardUp(unsigned char key) {
  if (gameState == STATE_GAME) {
    if (selectedLevel == 1)
      level1KeyboardUp(key);
    else if (selectedLevel == 2)
      level2KeyboardUp(key);
    else if (selectedLevel == 3)
      level3KeyboardUp(key);
    else if (selectedLevel == 4)
      level4KeyboardUp(key);
  }
}

void iSpecialKeyboardUp(unsigned char key) {
  if (gameState == STATE_GAME) {
    if (selectedLevel == 1)
      level1SpecialKeyboardUp(key);
    else if (selectedLevel == 2)
      level2SpecialKeyboardUp(key);
    else if (selectedLevel == 3)
      level3SpecialKeyboardUp(key);
    else if (selectedLevel == 4)
      level4SpecialKeyboardUp(key);
  }
}

// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
// GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
// GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
// GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
// GLUT_KEY_INSERT

void fixedUpdate() {
  if (gameState == STATE_LEVEL_LOADING) {
    if (selectedLevel == 1) resetLevel1();
    else if (selectedLevel == 2) resetLevel2();
    else if (selectedLevel == 3) resetLevel3();
    else if (selectedLevel == 4) resetLevel4();

    if (selectedLevel == 3) {
      initSudoku();
      gameState = STATE_SUDOKU;
    } else {
      gameState = STATE_GAME;
    }
    return;
  }

  if (gameState == STATE_SUDOKU) {
    updateSudoku();
    return;
  }

  if (gameState != STATE_GAME) {
    return;
  }
  
  if (selectedLevel == 1) {
    updateLevel1();
    if (isLevel1TransitionReady()) {
      selectedLevel = 2;
      resetLevel2();
    }
  } else if (selectedLevel == 2) {
    updateLevel2();
    if (isLevel2TransitionReady()) {
      selectedLevel = 3;
      gameState = STATE_LEVEL_LOADING;
    }
  } else  if (selectedLevel == 3) {
    updateLevel3();
    if (isLevel3TransitionReady() && !sIsTransitioning) {
        sIsTransitioning = true;
        sLevelTransitionTimer = 180; // 3 seconds at 60fps
    }
  } else  if (selectedLevel == 4) {
    updateLevel4();
    if (isLevel4GameWon() && !sScoreSaved) {
        addToLeaderboard(getPlayerName(), (int)sGlobalTimer);
        sScoreSaved = true;
    }
  }

  // Handle timed level transitions
  if (sIsTransitioning) {
      sLevelTransitionTimer--;
      if (sLevelTransitionTimer <= 0) {
          sIsTransitioning = false;
          selectedLevel = 4;
          gameState = STATE_LEVEL_LOADING;
      }
  }

  // Global game time tracking
  if (gameState == STATE_GAME) {
      sGlobalTimer += 0.016; // Approx 60fps
  }
}

void updateLoading() {
  if (gameState != STATE_LOADING) {
    return;
  }

  updateLoadingScreen(SCREEN_W, SCREEN_H);
}

void updateMenu() {
  if (gameState != STATE_MENU) {
    return;
  }
  if (gameState == STATE_LEADERBOARD) {
    return;
  }

  updateMenuScreen();
}

int main() {
  iInitialize(SCREEN_W, SCREEN_H, "DHAKA RETRO-METRO");

  initLoadingScreen(SCREEN_W, SCREEN_H);
  initMenuScreen(SCREEN_W, SCREEN_H);
  initLeaderboardScreen(SCREEN_W, SCREEN_H);
  initNameInputScreen(SCREEN_W, SCREEN_H);
  initHelpScreen(SCREEN_W, SCREEN_H);
  initCreditsScreen(SCREEN_W, SCREEN_H);
  initSettingsScreen(SCREEN_W, SCREEN_H);
  initChooseScreen(SCREEN_W, SCREEN_H);
  initLevelScreen(SCREEN_W, SCREEN_H);
  
  sTransitionLoadingTex = iLoadImage((char *)"Images/Loading Screen.jpg");

  // Level assets are lazy-loaded on first play (via resetLevel1/resetLevel2)
  initAudio("Audio/audio.wav", "Audio/click.wav", true);
  setMusicEnabled(isMusicOn());

  iSetTimer(16, fixedUpdate);
  iSetTimer(30, updateLoading);
  iSetTimer(33, updateMenu);

  iStart();
  return 0;
}