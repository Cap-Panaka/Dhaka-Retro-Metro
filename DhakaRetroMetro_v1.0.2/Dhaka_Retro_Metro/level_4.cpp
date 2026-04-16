
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iRectangle(double left, double bottom, double dx, double dy);
void iText(double x, double y, char *str, void *font = 0);
unsigned int iLoadImage(char filename[]);

static bool sLevel4AssetsLoaded = false;

// Screen dimensions
static int sScreenW = 0;
static int sScreenH = 0;

// Score and Lives
static int sScore = 0;
static int sPlayerLives = 3;

// Timer
static int sElapsedFrames = 0;
static double sElapsedSeconds = 0.0;

// Backgrounds (3 segments)
static unsigned int sBg1Tex = 0;
static unsigned int sBg2Tex = 0;
static unsigned int sBg3Tex = 0;

// Scrolling Camera
static double sCameraX = 0;
static const double sWorldWidth = 3240.0;

// Level Phases
enum LevelPhase { PHASE_CODE_GATE, PHASE_BOSS, BOSS_DEFEATED };
static LevelPhase sCurrentPhase = PHASE_CODE_GATE;
static double sWinDelayTimer = 0.0;

// Kakashi sprite frames
static const int FRAME_COUNT = 6;
static unsigned int sKakashiRight[FRAME_COUNT];
static unsigned int sKakashiLeft[FRAME_COUNT];
static unsigned int sKakashiWalkRight[FRAME_COUNT];
static unsigned int sKakashiWalkLeft[FRAME_COUNT];
static const int JUMP_FRAME_COUNT = 5;
static unsigned int sKakashiJumpRight[JUMP_FRAME_COUNT];
static unsigned int sKakashiJumpLeft[JUMP_FRAME_COUNT];
static const int ATTACK_FRAME_COUNT = 3;
static unsigned int sKakashiAttackRight[ATTACK_FRAME_COUNT];
static unsigned int sKakashiAttackLeft[ATTACK_FRAME_COUNT];

static const int CROUCH_FRAME_COUNT = 2;
static unsigned int sKakashiCrouchRight[CROUCH_FRAME_COUNT];
static unsigned int sKakashiCrouchLeft[CROUCH_FRAME_COUNT];

static const int DAMAGE_FRAME_COUNT = 6;
static unsigned int sKakashiDamageRight[DAMAGE_FRAME_COUNT];
static unsigned int sKakashiDamageLeft[DAMAGE_FRAME_COUNT];

static const int DEATH_FRAME_COUNT = 5;
static unsigned int sKakashiDeathRight[DEATH_FRAME_COUNT];
static unsigned int sKakashiDeathLeft[DEATH_FRAME_COUNT];

static const int RUN_FRAME_COUNT = 6;
static unsigned int sKakashiRunRight[RUN_FRAME_COUNT];
static unsigned int sKakashiRunLeft[RUN_FRAME_COUNT];

static const int CROUCH_WALK_FRAME_COUNT = 6;
static unsigned int sKakashiCrouchWalkRight[CROUCH_WALK_FRAME_COUNT];
static unsigned int sKakashiCrouchWalkLeft[CROUCH_WALK_FRAME_COUNT];

static const int KNIFE_ATTACK_FRAME_COUNT = 6;
static unsigned int sKakashiKnifeAttackRight[KNIFE_ATTACK_FRAME_COUNT];
static unsigned int sKakashiKnifeAttackLeft[KNIFE_ATTACK_FRAME_COUNT];

static const int WIN_FRAME_COUNT = 11;
static unsigned int sKakashiWinRight[WIN_FRAME_COUNT];
static unsigned int sKakashiWinLeft[WIN_FRAME_COUNT];

static const int SPECIAL_ATTACK_FRAME_COUNT = 30;
static unsigned int sKakashiSpecialRight[SPECIAL_ATTACK_FRAME_COUNT];
static unsigned int sKakashiSpecialLeft[SPECIAL_ATTACK_FRAME_COUNT];

static unsigned int sKunaiRightTex = 0;
static unsigned int sKunaiLeftTex = 0;
static unsigned int sKakashiHudTex = 0;

// Physics constants
static const double GRAVITY = -1.2;
static const double GROUND_LEVEL = 100;
static const double JUMP_FORCE = 18.0;
static const double FRICTION = 0.0;
static const double MAX_HORIZONTAL_SPEED = 10.0;
static const double ACCELERATION = 10.0;

// Boss States
enum BossState {
  STATE_STANCE,
  STATE_RUN,
  STATE_WALK,
  STATE_ATTACK1,
  STATE_ATTACK2,
  STATE_DAMAGE,
  STATE_DEATH
};

// Sasori sprite counts
static const int SAS_STANCE_FRAMES = 4;
static const int SAS_RUN_FRAMES = 6;
static const int SAS_ATTACK1_FRAMES = 8;
static const int SAS_ATTACK2_FRAMES = 10;
static const int SAS_DAMAGE_FRAMES = 2;
static const int SAS_DEATH_FRAMES = 3;

static unsigned int sSasStanceRight[SAS_STANCE_FRAMES];
static unsigned int sSasStanceLeft[SAS_STANCE_FRAMES];
static unsigned int sSasRunRight[SAS_RUN_FRAMES];
static unsigned int sSasRunLeft[SAS_RUN_FRAMES];
static unsigned int sSasAttack1Right[SAS_ATTACK1_FRAMES];
static unsigned int sSasAttack1Left[SAS_ATTACK1_FRAMES];
static unsigned int sSasAttack2Right[SAS_ATTACK2_FRAMES];
static unsigned int sSasAttack2Left[SAS_ATTACK2_FRAMES];
static unsigned int sSasDamageRight[SAS_DAMAGE_FRAMES];
static unsigned int sSasDamageLeft[SAS_DAMAGE_FRAMES];
static unsigned int sSasDeathRight[SAS_DEATH_FRAMES];
static unsigned int sSasDeathLeft[SAS_DEATH_FRAMES];

static const int BOSS_MAX_HEALTH = 1200;

struct Enemy {
  double x, y;
  double velX, velY;
  int direction;
  int health;
  bool isAlive;
  int currentFrame;
  int animTimer;
  int enemyType;
  BossState state;
  bool isAttacking;
  int attackFrame;
  int attackTimer;
  double width, height;
  bool isOnGround;
  bool isJumping;
  int attackCooldown;
};

static double sPlayerFuel = 100.0;
static bool sIsSpecialCombo = false;
static int sSpecialComboFrame = 0;
static int sSpecialComboTimer = 0;

struct Bullet {
  double x, y;
  double velX;
  bool isActive;
  double radius;
};

// Game Entities
static const int MAX_ENEMIES = 1;
static Enemy sEnemies[MAX_ENEMIES];
static const int MAX_BULLETS = 10;
static Bullet sBullets[MAX_BULLETS];

// Player state
static double sPlayerX = 100;
static double sPlayerY = GROUND_LEVEL;
static int sPlayerW = 80;
static int sPlayerH = 80;
static int sPlayerHealth = 100;

// Velocity
static double sPlayerVelX = 0;
static double sPlayerVelY = 0;

// Physics state
static bool sIsOnGround = true;
static bool sIsJumping = false;

// Input state
static bool sMovingLeft = false;
static bool sMovingRight = false;

// Direction: 0 = right, 1 = left
static int sDirection = 0;

// Animation
static int sCurrentFrame = 0;
static int sAnimTimer = 0;
static const int sAnimDelay = 8;

// Attack state (K key - melee)
static bool sIsAttacking = false;
static int sAttackFrame = 0;
static int sAttackTimer = 0;
static const int sAttackDelay = 6;

// Throwing state (J key - projectile)
static bool sIsThrowing = false;
static int sThrowFrame = 0;
static int sThrowTimer = 0;
static const int sThrowDelay = 6;
static bool sThrowBulletSpawned = false;

// Player states
static bool sIsRunning = false;
static bool sIsWinning = false;
static int sWinFrame = 0;
static int sWinTimer = 0;

static bool sIsCrouching = false;
static int sCrouchFrame = 0;
static int sCrouchTimer = 0;

static bool sIsTakingDamage = false;
static int sDamageFrame = 0;
static int sDamageTimer = 0;

static bool sIsDying = false;
static int sDeathFrame = 0;
static int sDeathTimer = 0;

static bool sIsGameOver = false;

// Boss rage mode
static bool sBossRageMode = false;

// Potion
static unsigned int sPotionTex = 0;
struct Potion {
  double x, y;
  bool isActive;
};
static Potion sPhasePotion;

// =====================================================
// CODE GATE SYSTEM
// =====================================================
static int sCodeGateStep = 0; // 0, 1, 2 = which code to enter; 3 = all verified
static char sCodeBuffer[32];
static int sCodeBufferPos = 0;
static bool sCodeError = false;
static int sCodeErrorTimer = 0;

static const char *sExpectedCodes[3] = {
  "1DC 75UA",  // Level 1 code
  "1DC T5UA",  // Level 2 code
  "1AC T5VB"   // Level 3 code
};
static const char *sCodeLabels[3] = {
  "ARENA 1 CODE",
  "ARENA 2 CODE",
  "ARENA 3 CODE"
};
static bool sCodesVerified[3] = {false, false, false};

// =====================================================
// HELPER FUNCTIONS
// =====================================================
static void resetEnemy(int index) {
  sEnemies[index].x = sWorldWidth - 300;
  sEnemies[index].y = GROUND_LEVEL;
  sEnemies[index].velX = 0;
  sEnemies[index].velY = 0;
  sEnemies[index].direction = 1; // Face left
  sEnemies[index].health = BOSS_MAX_HEALTH;
  sEnemies[index].isAlive = true;
  sEnemies[index].currentFrame = 0;
  sEnemies[index].animTimer = 0;
  sEnemies[index].enemyType = 2; // Boss
  sEnemies[index].state = STATE_STANCE;
  sEnemies[index].isAttacking = false;
  sEnemies[index].attackFrame = 0;
  sEnemies[index].attackTimer = 0;
  sEnemies[index].width = 80;
  sEnemies[index].height = 80;
  sEnemies[index].isOnGround = true;
  sEnemies[index].isJumping = false;
  sEnemies[index].attackCooldown = 30; // Shorter cooldown
}

// =====================================================
// INIT & ASSET LOADING
// =====================================================
void initLevel4(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  if (sLevel4AssetsLoaded) return;

  // Load backgrounds
  sBg1Tex = iLoadImage((char *)"Images/Level 4 Background/1.png");
  sBg2Tex = iLoadImage((char *)"Images/Level 4 Background/2.png");
  sBg3Tex = iLoadImage((char *)"Images/Level 4 Background/3.png");

  sKakashiHudTex = iLoadImage((char *)"Images/Sprites/Kakashi_hud.png");
  sPotionTex =
      iLoadImage((char *)"Images/Sprites/Collectibles/healing_potion.png");

  // Load Kakashi sprite frames (identical to Level 3)
  sKakashiRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight1.png");
  sKakashiRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight2.png");
  sKakashiRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight3.png");
  sKakashiRight[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight4.png");
  sKakashiRight[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight5.png");
  sKakashiRight[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Right/StanceRight6.png");
  sKakashiLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft1.png");
  sKakashiLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft2.png");
  sKakashiLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft3.png");
  sKakashiLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft4.png");
  sKakashiLeft[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft5.png");
  sKakashiLeft[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft6.png");

  // Crouch
  sKakashiCrouchRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Right/CrouchRight1.png");
  sKakashiCrouchRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Right/CrouchRight2.png");
  sKakashiCrouchLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Left/CrouchLeft1.png");
  sKakashiCrouchLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Left/CrouchLeft2.png");

  // Damage
  for (int i = 0; i < DAMAGE_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Damage Right/DamageRight%d.png", i + 1);
    sKakashiDamageRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft%d.png", i + 1);
    sKakashiDamageLeft[i] = iLoadImage(path);
  }

  // Death
  for (int i = 0; i < DEATH_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Death Right/DeathRight%d.png", i + 1);
    sKakashiDeathRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Death Left/DeathLeft%d.png", i + 1);
    sKakashiDeathLeft[i] = iLoadImage(path);
  }

  // Walk
  for (int i = 0; i < FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Walk Right/WalkRight%d.png", i + 1);
    sKakashiWalkRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft%d.png", i + 1);
    sKakashiWalkLeft[i] = iLoadImage(path);
  }

  // Jump
  for (int i = 0; i < JUMP_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Jump Right/JumpRight%d.png", i + 1);
    sKakashiJumpRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft%d.png", i + 1);
    sKakashiJumpLeft[i] = iLoadImage(path);
  }

  // Weapon Throw
  sKakashiAttackRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Right/WeaponThrow_Right1.png");
  sKakashiAttackRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Right/WeaponThrow_Right2.png");
  sKakashiAttackRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Right/WeaponThrow_Right3.png");
  sKakashiAttackLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Left/WeaponThrow_Left1.png");
  sKakashiAttackLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Left/WeaponThrow_Left2.png");
  sKakashiAttackLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Weapon Throw Left/WeaponThrow_Left3.png");

  // Run
  for (int i = 0; i < RUN_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Run Right/RunRight%d.png", i + 1);
    sKakashiRunRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Run Left/RunLeft%d.png", i + 1);
    sKakashiRunLeft[i] = iLoadImage(path);
  }

  // Crouch Walk
  for (int i = 0; i < CROUCH_WALK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Crouch Walk Right/CrouchWalkRight%d.png", i + 1);
    sKakashiCrouchWalkRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Crouch Walk Left/CrouchWalkLeft%d.png", i + 1);
    sKakashiCrouchWalkLeft[i] = iLoadImage(path);
  }

  // Knife Attack
  for (int i = 0; i < KNIFE_ATTACK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Knife Attack Right/KnifeAttackRight%d.png", i + 1);
    sKakashiKnifeAttackRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Knife Attack Left/KnifeAttackLeft%d.png", i + 1);
    sKakashiKnifeAttackLeft[i] = iLoadImage(path);
  }

  // Win
  for (int i = 0; i < WIN_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Win Right/WinRight%d.png", i + 1);
    sKakashiWinRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Win Left/WinLeft%d.png", i + 1);
    sKakashiWinLeft[i] = iLoadImage(path);
  }

  sKunaiRightTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_right.png");
  sKunaiLeftTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_left.png");

  // =====================================================
  // LOAD SASORI SPRITES
  // =====================================================
  for (int i = 0; i < SAS_STANCE_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Stance Right/Stance Right_%d.png", i);
    sSasStanceRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Stance Left/Stance Left_%d.png", i);
    sSasStanceLeft[i] = iLoadImage(path);
  }
  for (int i = 0; i < SAS_RUN_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Run Right/Run Right_%d.png", i);
    sSasRunRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Run Left/Run Left_%d.png", i);
    sSasRunLeft[i] = iLoadImage(path);
  }
  for (int i = 0; i < SAS_ATTACK1_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Attack 1 Right/Attack 1 Right_%d.png", i);
    sSasAttack1Right[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Attack 1 Left/Attack 1 Left_%d.png", i);
    sSasAttack1Left[i] = iLoadImage(path);
  }
  for (int i = 0; i < SAS_ATTACK2_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Attack 2 Right/Attack 2 Right_%d.png", i);
    sSasAttack2Right[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Attack 2 Left/Attack 2 Left_%d.png", i);
    sSasAttack2Left[i] = iLoadImage(path);
  }
  for (int i = 0; i < SAS_DAMAGE_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Damage Right/Damage Right_%d.png", i);
    sSasDamageRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Damage Left/Damage Left_%d.png", i);
    sSasDamageLeft[i] = iLoadImage(path);
  }
  for (int i = 0; i < SAS_DEATH_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Sasori/Death Right/Death Right_%d.png", i);
    sSasDeathRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Sasori/Death Left/Death Left_%d.png", i);
    sSasDeathLeft[i] = iLoadImage(path);
  }

  // Load Special Attack
  for (int i = 0; i < SPECIAL_ATTACK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Special Attack/Layer %d.png", i + 1);
    sKakashiSpecialRight[i] = iLoadImage(path);
    sKakashiSpecialLeft[i] = iLoadImage(path);
  }

  sLevel4AssetsLoaded = true;
}

// =====================================================
// RESET
// =====================================================
void resetLevel4() {
  if (!sLevel4AssetsLoaded) {
    initLevel4(sScreenW > 0 ? sScreenW : 1080, sScreenH > 0 ? sScreenH : 635);
  }

  sScore = 0;
  sPlayerHealth = 100;
  sPlayerLives = 3;
  sPlayerX = 100;
  sPlayerY = GROUND_LEVEL;
  sCameraX = 0;
  sCurrentPhase = PHASE_CODE_GATE;
  sElapsedSeconds = 0;
  sElapsedFrames = 0;
  sIsGameOver = false;
  sIsWinning = false;
  sWinDelayTimer = 0.0;
  sBossRageMode = false;

  // Reset code gate
  sCodeGateStep = 0;
  memset(sCodeBuffer, 0, sizeof(sCodeBuffer));
  sCodeBufferPos = 0;
  sCodeError = false;
  sCodeErrorTimer = 0;
  sCodesVerified[0] = false;
  sCodesVerified[1] = false;
  sCodesVerified[2] = false;

  // Reset physics
  sPlayerVelX = 0;
  sPlayerVelY = 0;
  sIsOnGround = true;
  sIsJumping = false;

  // Reset input
  sMovingLeft = false;
  sMovingRight = false;

  // Reset states
  sIsAttacking = false;
  sIsThrowing = false;
  sIsCrouching = false;
  sIsRunning = false;
  sIsTakingDamage = false;
  sIsDying = false;
  sCurrentFrame = 0;
  sAnimTimer = 0;
  sDirection = 0;
  sWinFrame = 0;
  sWinTimer = 0;

  // Reset potion
  sPhasePotion.isActive = false;
  sPlayerFuel = 100.0;
  sIsSpecialCombo = false;
  sSpecialComboFrame = 0;
  sSpecialComboTimer = 0;

  // Reset bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    sBullets[i].isActive = false;
  }

  // Reset boss
  for (int i = 0; i < MAX_ENEMIES; i++) {
    sEnemies[i].isAlive = false;
  }
}

// =====================================================
// DRAW
// =====================================================
void drawLevel4(int screenW, int screenH) {
  iClear();

  // Draw Code Gate Screen
  if (sCurrentPhase == PHASE_CODE_GATE) {
    // Dark background with the 3 background images tiled
    iShowImage(0, 0, screenW, screenH, sBg1Tex);

    // Dark overlay
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, screenW, screenH);

    // Title
    iSetColor(255, 80, 80);
    iText(screenW / 2.0 - 130, screenH - 80, (char *)"FINAL ARENA - CODE GATE", (void *)2);

    iSetColor(200, 200, 200);
    iText(screenW / 2.0 - 160, screenH - 110,
          (char *)"Enter the secret codes from all 3 arenas to proceed");

    // Draw 3 code input areas
    for (int c = 0; c < 3; c++) {
      int boxX = screenW / 2 - 120;
      int boxY = screenH - 180 - (c * 100);

      // Label
      if (sCodesVerified[c]) {
        iSetColor(50, 255, 50);
      } else if (c == sCodeGateStep) {
        iSetColor(255, 220, 100);
      } else {
        iSetColor(150, 150, 150);
      }
      iText(boxX, boxY + 35, (char *)sCodeLabels[c], (void *)1);

      // Input box
      if (sCodesVerified[c]) {
        iSetColor(20, 60, 20);
      } else if (c == sCodeGateStep) {
        iSetColor(40, 40, 60);
      } else {
        iSetColor(25, 25, 25);
      }
      iFilledRectangle(boxX, boxY, 240, 30);

      // Box border
      if (c == sCodeGateStep && !sCodesVerified[c]) {
        iSetColor(255, 180, 60);
      } else if (sCodesVerified[c]) {
        iSetColor(50, 255, 50);
      } else {
        iSetColor(80, 80, 80);
      }
      iRectangle(boxX, boxY, 240, 30);

      // Content
      if (sCodesVerified[c]) {
        iSetColor(50, 255, 50);
        iText(boxX + 10, boxY + 8, (char *)"VERIFIED", (void *)2);
      } else if (c == sCodeGateStep) {
        iSetColor(255, 255, 255);
        iText(boxX + 10, boxY + 8, sCodeBuffer, (void *)2);
        // Cursor blink
        if ((sElapsedFrames / 20) % 2 == 0) {
          double cursorX = boxX + 10 + sCodeBufferPos * 12;
          iFilledRectangle(cursorX, boxY + 4, 2, 22);
        }
      } else {
        iSetColor(80, 80, 80);
        iText(boxX + 10, boxY + 8, (char *)"--------", (void *)2);
      }
    }

    // Error message
    if (sCodeError) {
      iSetColor(255, 0, 0);
      iText(screenW / 2.0 - 60, screenH - 490, (char *)"WRONG CODE!", (void *)2);
    }

    // Instructions
    iSetColor(180, 180, 180);
    iText(screenW / 2.0 - 120, 40, (char *)"Type code and press ENTER  |  ESC to go back");

    // Update error timer
    if (sCodeError) {
      sCodeErrorTimer--;
      if (sCodeErrorTimer <= 0) sCodeError = false;
    }
    sElapsedFrames++;
    return;
  }

  // =====================================================
  // DRAW BOSS PHASE
  // =====================================================
  int segmentW = screenW;
  // Draw the three background segments based on camera position
  for (int seg = 0; seg < 3; seg++) {
    int segStartX = seg * segmentW;
    int drawX = segStartX - (int)sCameraX;
    if (drawX > -segmentW && drawX < segmentW) {
      unsigned int tex = (seg == 0) ? sBg1Tex : (seg == 1) ? sBg2Tex : sBg3Tex;
      iShowImage(drawX, 0, segmentW, screenH, tex);
    }
  }

  // Draw Potion
  if (sPhasePotion.isActive) {
    iShowImage((int)(sPhasePotion.x - sCameraX), (int)sPhasePotion.y, 30, 30, sPotionTex);
  }

  // Draw Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      unsigned int kunaiTex = (sBullets[i].velX > 0) ? sKunaiRightTex : sKunaiLeftTex;
      iShowImage((int)(sBullets[i].x - sCameraX), (int)sBullets[i].y, 30, 10, kunaiTex);
    }
  }

  // Draw Player
  unsigned int playerTex = 0;
  int playerRenderW = sPlayerW;
  int playerRenderH = sPlayerH;

  if (sIsDying) {
    playerTex = (sDirection == 0)
                    ? sKakashiDeathRight[sDeathFrame % DEATH_FRAME_COUNT]
                    : sKakashiDeathLeft[sDeathFrame % DEATH_FRAME_COUNT];
  } else if (sIsWinning) {
    playerTex = (sDirection == 0)
                    ? sKakashiWinRight[sWinFrame % WIN_FRAME_COUNT]
                    : sKakashiWinLeft[sWinFrame % WIN_FRAME_COUNT];
  } else if (sIsTakingDamage) {
    playerTex = (sDirection == 0)
                    ? sKakashiDamageRight[sDamageFrame % DAMAGE_FRAME_COUNT]
                    : sKakashiDamageLeft[sDamageFrame % DAMAGE_FRAME_COUNT];
  } else if (sIsAttacking) {
    playerTex = (sDirection == 0)
                    ? sKakashiKnifeAttackRight[sAttackFrame % KNIFE_ATTACK_FRAME_COUNT]
                    : sKakashiKnifeAttackLeft[sAttackFrame % KNIFE_ATTACK_FRAME_COUNT];
    playerRenderW = sPlayerW + 20;
  } else if (sIsThrowing) {
    playerTex = (sDirection == 0)
                    ? sKakashiAttackRight[sThrowFrame % ATTACK_FRAME_COUNT]
                    : sKakashiAttackLeft[sThrowFrame % ATTACK_FRAME_COUNT];
  } else if (!sIsOnGround) {
    int jf = sIsJumping ? 0 : 2;
    playerTex = (sDirection == 0)
                    ? sKakashiJumpRight[jf % JUMP_FRAME_COUNT]
                    : sKakashiJumpLeft[jf % JUMP_FRAME_COUNT];
  } else if (sIsCrouching) {
    bool isMoving = (sPlayerVelX > 0.5 || sPlayerVelX < -0.5);
    if (isMoving) {
      playerTex = (sDirection == 0)
                      ? sKakashiCrouchWalkRight[sCurrentFrame % CROUCH_WALK_FRAME_COUNT]
                      : sKakashiCrouchWalkLeft[sCurrentFrame % CROUCH_WALK_FRAME_COUNT];
    } else {
      playerTex = (sDirection == 0)
                      ? sKakashiCrouchRight[sCrouchFrame % CROUCH_FRAME_COUNT]
                      : sKakashiCrouchLeft[sCrouchFrame % CROUCH_FRAME_COUNT];
    }
    playerRenderH = sPlayerH / 2;
  } else if (sIsRunning && (sPlayerVelX > 0.5 || sPlayerVelX < -0.5)) {
    playerTex = (sDirection == 0)
                    ? sKakashiRunRight[sCurrentFrame % RUN_FRAME_COUNT]
                    : sKakashiRunLeft[sCurrentFrame % RUN_FRAME_COUNT];
  } else if (sPlayerVelX > 0.5 || sPlayerVelX < -0.5) {
    playerTex = (sDirection == 0)
                    ? sKakashiWalkRight[sCurrentFrame % FRAME_COUNT]
                    : sKakashiWalkLeft[sCurrentFrame % FRAME_COUNT];
  } else {
    playerTex = (sDirection == 0)
                    ? sKakashiRight[sCurrentFrame % FRAME_COUNT]
                    : sKakashiLeft[sCurrentFrame % FRAME_COUNT];
  }

  iShowImage((int)(sPlayerX - sCameraX), (int)sPlayerY, playerRenderW,
             playerRenderH, playerTex);

  // Draw Special Attack
  if (sIsSpecialCombo) {
      unsigned int tex = (sDirection == 0) ? sKakashiSpecialRight[sSpecialComboFrame] : sKakashiSpecialLeft[sSpecialComboFrame];
      iShowImage((int)(sPlayerX - sCameraX - 40), (int)sPlayerY, 160, 100, tex);
  }

  // Draw Boss
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive) {
      unsigned int enemyTex = 0;
      int renderW = (int)sEnemies[i].width;
      int renderH = (int)sEnemies[i].height;
      int offsetX = 0;

      int frame = sEnemies[i].currentFrame;
      int dir = sEnemies[i].direction;

      switch (sEnemies[i].state) {
      case STATE_STANCE:
        enemyTex = (dir == 0) ? sSasStanceRight[frame % SAS_STANCE_FRAMES]
                              : sSasStanceLeft[frame % SAS_STANCE_FRAMES];
        break;
      case STATE_RUN:
      case STATE_WALK:
        enemyTex = (dir == 0) ? sSasRunRight[frame % SAS_RUN_FRAMES]
                              : sSasRunLeft[frame % SAS_RUN_FRAMES];
        break;
      case STATE_DAMAGE:
        enemyTex = (dir == 0) ? sSasDamageRight[frame % SAS_DAMAGE_FRAMES]
                              : sSasDamageLeft[frame % SAS_DAMAGE_FRAMES];
        break;
      case STATE_DEATH:
        enemyTex = (dir == 0) ? sSasDeathRight[frame % SAS_DEATH_FRAMES]
                              : sSasDeathLeft[frame % SAS_DEATH_FRAMES];
        break;
      case STATE_ATTACK1:
        enemyTex = (dir == 0) ? sSasAttack1Right[sEnemies[i].attackFrame % SAS_ATTACK1_FRAMES]
                              : sSasAttack1Left[sEnemies[i].attackFrame % SAS_ATTACK1_FRAMES];
        renderW += 40;
        if (dir == 1) offsetX = -40;
        break;
      case STATE_ATTACK2:
        enemyTex = (dir == 0) ? sSasAttack2Right[sEnemies[i].attackFrame % SAS_ATTACK2_FRAMES]
                              : sSasAttack2Left[sEnemies[i].attackFrame % SAS_ATTACK2_FRAMES];
        renderW += 60;
        if (dir == 1) offsetX = -60;
        break;
      default:
        enemyTex = (dir == 0) ? sSasStanceRight[0] : sSasStanceLeft[0];
        break;
      }

      if (enemyTex > 0) {
        iShowImage((int)(sEnemies[i].x - sCameraX) + offsetX,
                   (int)sEnemies[i].y, renderW, renderH, enemyTex);
      }

      // Boss HP bar
      int bossBarW = 400;
      int bossBarH = 12;
      int bossBarX = (screenW - bossBarW) / 2;
      int bossBarY = screenH - 55;

      iSetColor(40, 20, 20);
      iFilledRectangle(bossBarX - 2, bossBarY - 2, bossBarW + 4, bossBarH + 4);
      iSetColor(100, 0, 0);
      iFilledRectangle(bossBarX, bossBarY, bossBarW, bossBarH);

      double hpRatio = sEnemies[i].health / (double)BOSS_MAX_HEALTH;
      if (hpRatio < 0) hpRatio = 0;
      if (sBossRageMode) {
        iSetColor(255, 50, 0);
      } else {
        iSetColor(200, 50, 50);
      }
      iFilledRectangle(bossBarX, bossBarY, bossBarW * hpRatio, bossBarH);

      iSetColor(255, 180, 100);
      iText(bossBarX, bossBarY + bossBarH + 4,
            (char *)(sBossRageMode ? "SASORI - RAGE!" : "BOSS: SASORI - The Puppeteer"));
    }
  }

  // HUD
  iShowImage(10, screenH - 50, 50, 50, sKakashiHudTex);
  iSetColor(255, 0, 0);
  iFilledRectangle(70, screenH - 40, 200, 20);
  iSetColor(0, 255, 0);
  iFilledRectangle(70, screenH - 40, 200 * (sPlayerHealth / 100.0), 20);
  iSetColor(255, 255, 255);
  iText(70, screenH - 55, (char *)"Health");

  char livesStr[20];
  sprintf_s(livesStr, "Lives: %d", sPlayerLives);
  iText(70, screenH - 75, livesStr);
  char scoreStr[20];
  sprintf_s(scoreStr, "Score: %d", sScore);
  iSetColor(0, 255, 0);
  iText(70, screenH - 95, scoreStr);

  // Timer
  int t = (int)sElapsedSeconds;
  char timeStr[20];
  sprintf_s(timeStr, "TIME: %d:%02d", t / 60, t % 60);
  iSetColor(255, 255, 255);
  iText(screenW / 2.0 - 50.0, screenH - 30.0, timeStr, (void *)2);

  // Phase label
  iSetColor(255, 100, 100);
  char phaseStr[30];
  if (sBossRageMode)
    sprintf_s(phaseStr, "SASORI - RAGE!");
  else
    sprintf_s(phaseStr, "FINAL BATTLE");
  iText(screenW - 180, screenH - 30, phaseStr);

  // Game Over
  if (sIsGameOver) {
    iSetColor(255, 0, 0);
    iText(screenW / 2.0 - 50.0, screenH / 2.0 + 20.0, (char *)"YOU DIED", (void *)2);
    iSetColor(255, 255, 255);
    iText(screenW / 2.0 - 80.0, screenH / 2.0 - 20.0,
          (char *)"Press M for Main Menu");
  } else if (sCurrentPhase == BOSS_DEFEATED) {
    // Victory banner
    iSetColor(15, 15, 15);
    iFilledRectangle(0, screenH / 2.0 - 60.0, screenW, 120.0);

    iSetColor(218, 165, 32);
    iFilledRectangle(0, screenH / 2.0 + 58.0, screenW, 4.0);
    iFilledRectangle(0, screenH / 2.0 - 62.0, screenW, 4.0);

    iSetColor(50, 255, 50);
    iText(screenW / 2.0 - 120.0, screenH / 2.0 + 20.0,
          (char *)"GAME COMPLETE!", (void *)2);
    iSetColor(255, 215, 0);
    iText(screenW / 2.0 - 150.0, screenH / 2.0 - 10.0,
          (char *)"The Puppeteer has fallen!", (void *)2);
    iSetColor(200, 200, 200);
    iText(screenW / 2.0 - 100.0, screenH / 2.0 - 40.0,
          (char *)"Press M for Main Menu");
  }
}

// =====================================================
// UPDATE
// =====================================================
void updateLevel4() {
  if (sCurrentPhase == PHASE_CODE_GATE) return;
  if (sIsGameOver || sIsDying) {
    if (sIsDying) {
      sDeathTimer++;
      if (sDeathTimer >= 8) {
        sDeathTimer = 0;
        sDeathFrame++;
        if (sDeathFrame >= DEATH_FRAME_COUNT) {
          sIsDying = false;
          sDeathFrame = 0;
          sPlayerHealth = 100;
          sPlayerX = sCameraX + 100;
          sPlayerY = GROUND_LEVEL;
          sPlayerVelX = 0;
          sPlayerVelY = 0;
        }
      }
    }
    return;
  }

  if (sIsWinning) {
    sWinTimer++;
    if (sWinTimer >= 8) {
      sWinTimer = 0;
      sWinFrame = (sWinFrame + 1) % WIN_FRAME_COUNT;
    }
    sWinDelayTimer += 0.016;
    return;
  }

  // Global game time tracking
  sElapsedFrames++;
  sElapsedSeconds = sElapsedFrames * 0.016;

  // Fuel regeneration
  if (sPlayerFuel < 100.0) {
      sPlayerFuel += 0.15;
      if (sPlayerFuel > 100.0) sPlayerFuel = 100.0;
  }

  // Spawn boss if not yet spawned
  if (!sEnemies[0].isAlive && sCurrentPhase == PHASE_BOSS) {
    resetEnemy(0);
  }

  // Update Camera
  double targetCamX = sPlayerX - 400;
  if (targetCamX < 0) targetCamX = 0;
  if (targetCamX > sWorldWidth - sScreenW)
    targetCamX = sWorldWidth - sScreenW;
  sCameraX += (targetCamX - sCameraX) * 0.1;

  // Handle Damage animation
  if (sIsTakingDamage) {
    sDamageTimer++;
    if (sDamageTimer >= 6) {
      sDamageTimer = 0;
      sDamageFrame++;
      if (sDamageFrame >= DAMAGE_FRAME_COUNT) {
        sIsTakingDamage = false;
        sDamageFrame = 0;
      }
    }
  }

  // Handle Throw
  if (sIsThrowing) {
    sThrowTimer++;
    if (sThrowTimer >= sThrowDelay) {
      sThrowTimer = 0;
      sThrowFrame++;
      if (sThrowFrame == 1 && !sThrowBulletSpawned) {
        for (int i = 0; i < MAX_BULLETS; i++) {
          if (!sBullets[i].isActive) {
            sBullets[i].isActive = true;
            sBullets[i].x = sPlayerX + sPlayerW / 2;
            sBullets[i].y = sPlayerY + sPlayerH / 2;
            sBullets[i].velX = (sDirection == 0) ? 15.0 : -15.0;
            sBullets[i].radius = 5;
            sThrowBulletSpawned = true;
            break;
          }
        }
      }
      if (sThrowFrame >= ATTACK_FRAME_COUNT) {
        sIsThrowing = false;
        sThrowFrame = 0;
      }
    }
  }

  // Handle Melee Attack
  if (sIsAttacking) {
    sAttackTimer++;
    if (sAttackTimer >= sAttackDelay) {
      sAttackTimer = 0;
      sAttackFrame++;
      if (sAttackFrame >= KNIFE_ATTACK_FRAME_COUNT) {
        sIsAttacking = false;
        sAttackFrame = 0;
      }
    }
    // Hit on frame 3
    if (sAttackFrame == 3 && sAttackTimer == 0) {
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (sEnemies[i].isAlive) {
          if (fabs(sPlayerX - sEnemies[i].x) < 100.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 80.0) {
            sEnemies[i].health -= 25;
            sScore += 5;
            if (sEnemies[i].health <= 0) {
              sEnemies[i].health = 1;
              if (sEnemies[i].state != STATE_DEATH) {
                sEnemies[i].state = STATE_DEATH;
                sEnemies[i].currentFrame = 0;
                sEnemies[i].animTimer = 0;
                sEnemies[i].isAttacking = false;
              }
            } else if (sEnemies[i].state != STATE_DEATH) {
              if (sEnemies[i].state != STATE_DAMAGE) {
                sEnemies[i].state = STATE_DAMAGE;
                sEnemies[i].currentFrame = 0;
                sEnemies[i].animTimer = 0;
              }
              sEnemies[i].isAttacking = false;
            }
          }
        }
      }
    }
  }

  // Handle Special Combo
  if (sIsSpecialCombo) {
      sSpecialComboTimer++;
      if (sSpecialComboTimer >= 3) {
          sSpecialComboTimer = 0;
          sSpecialComboFrame++;
          if (sSpecialComboFrame >= SPECIAL_ATTACK_FRAME_COUNT) {
              sIsSpecialCombo = false;
              sSpecialComboFrame = 0;
          }
      }
      // Hit check
      if (sSpecialComboFrame > 5 && sSpecialComboFrame < 25) {
          for (int i = 0; i < MAX_ENEMIES; i++) {
              if (sEnemies[i].isAlive && fabs(sPlayerX - sEnemies[i].x) < 150) {
                  sEnemies[i].health -= 2;
              }
          }
      }
  }

  // Player Health / Lives
  if (sPlayerHealth <= 0) {
    sPlayerLives--;
    if (sPlayerLives <= 0) {
      sIsGameOver = true;
    } else {
      sIsDying = true;
      sDeathFrame = 0;
      sDeathTimer = 0;
    }
    return;
  }

  // Update Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      sBullets[i].x += sBullets[i].velX;
      if (sBullets[i].x < sCameraX || sBullets[i].x > sCameraX + sScreenW)
        sBullets[i].isActive = false;
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (sEnemies[j].isAlive && fabs(sBullets[i].x - sEnemies[j].x) < 50 &&
            fabs(sBullets[i].y - sEnemies[j].y) < 50) {
          sEnemies[j].health -= 10;
          sScore += 5;
          sBullets[i].isActive = false;
          if (sEnemies[j].health <= 0) {
            sEnemies[j].health = 1;
            if (sEnemies[j].state != STATE_DEATH) {
              sEnemies[j].state = STATE_DEATH;
              sEnemies[j].currentFrame = 0;
              sEnemies[j].animTimer = 0;
              sEnemies[j].isAttacking = false;
            }
          } else if (sEnemies[j].state != STATE_DEATH) {
            if (sEnemies[j].state != STATE_DAMAGE) {
              sEnemies[j].state = STATE_DAMAGE;
              sEnemies[j].currentFrame = 0;
              sEnemies[j].animTimer = 0;
            }
            sEnemies[j].isAttacking = false;
          }
          break;
        }
      }
    }
  }

  // Physics
  double currentMaxSpeed =
      sIsRunning ? MAX_HORIZONTAL_SPEED * 1.5 : MAX_HORIZONTAL_SPEED;
  if (sIsCrouching) currentMaxSpeed = MAX_HORIZONTAL_SPEED * 0.5;

  if (sMovingRight) {
    sPlayerVelX += ACCELERATION;
    sDirection = 0;
  } else if (sMovingLeft) {
    sPlayerVelX -= ACCELERATION;
    sDirection = 1;
  } else {
    sPlayerVelX *= FRICTION;
  }

  if (sPlayerVelX > currentMaxSpeed) sPlayerVelX = currentMaxSpeed;
  if (sPlayerVelX < -currentMaxSpeed) sPlayerVelX = -currentMaxSpeed;
  if (sPlayerVelX > -0.1 && sPlayerVelX < 0.1) sPlayerVelX = 0;

  if (!sIsOnGround) sPlayerVelY += GRAVITY;

  sPlayerX += sPlayerVelX;
  double oldY = sPlayerY;
  sPlayerY += sPlayerVelY;

  // Ground collision
  if (sPlayerY <= GROUND_LEVEL) {
    sPlayerY = GROUND_LEVEL;
    sPlayerVelY = 0;
    sIsOnGround = true;
    sIsJumping = false;
  }

  // World bounds
  if (sPlayerX < 0) { sPlayerX = 0; sPlayerVelX = 0; }
  if (sPlayerX > sWorldWidth - sPlayerW) {
    sPlayerX = sWorldWidth - sPlayerW;
    sPlayerVelX = 0;
  }

  // Potion Collection
  if (sPhasePotion.isActive) {
    if (fabs(sPlayerX - sPhasePotion.x) < 50.0 &&
        fabs(sPlayerY - sPhasePotion.y) < 50.0) {
      sPlayerHealth += 40;
      if (sPlayerHealth > 100) sPlayerHealth = 100;
      sPhasePotion.isActive = false;
    }
  }

  // =====================================================
  // BOSS AI
  // =====================================================
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!sEnemies[i].isAlive) continue;

    double dist = sPlayerX - sEnemies[i].x;
    sEnemies[i].direction = (dist > 0) ? 0 : 1;

    if (sEnemies[i].attackCooldown > 0) sEnemies[i].attackCooldown--;

    // Check rage mode
    if (sEnemies[i].health <= (BOSS_MAX_HEALTH / 2) && !sBossRageMode) {
      sBossRageMode = true;
      sPhasePotion.isActive = true;
      sPhasePotion.x = sPlayerX + 100;
      sPhasePotion.y = GROUND_LEVEL;
    }

    if (sEnemies[i].state == STATE_DAMAGE) {
      // Stay in damage
    } else if (sEnemies[i].state == STATE_DEATH) {
      // Stay in death
    } else if (!sEnemies[i].isAttacking) {
      double moveSpeed = sBossRageMode ? 7.0 : 4.5;

      if (fabs(dist) > 300.0) {
        sEnemies[i].x += (dist > 0) ? moveSpeed : -moveSpeed;
        sEnemies[i].state = STATE_RUN;
      } else if (fabs(dist) > 100.0) {
        sEnemies[i].x += (dist > 0) ? moveSpeed * 0.7 : -moveSpeed * 0.7;
        sEnemies[i].state = STATE_WALK;
      } else {
        sEnemies[i].state = STATE_STANCE;

        int attackChance = sBossRageMode ? 3 : 12;
        if (sEnemies[i].attackCooldown <= 0 && rand() % attackChance < 3) {
          sEnemies[i].isAttacking = true;
          sEnemies[i].attackFrame = 0;
          sEnemies[i].attackTimer = 0;

          int r = rand() % 10;
          if (r < 6) {
            sEnemies[i].state = STATE_ATTACK1;
            sEnemies[i].attackCooldown = sBossRageMode ? 40 : 80;
          } else {
            sEnemies[i].state = STATE_ATTACK2;
            sEnemies[i].attackCooldown = sBossRageMode ? 60 : 120;
          }
        }
      }

      // Rage jump
      if (sBossRageMode && sEnemies[i].isOnGround && fabs(dist) > 150 &&
          fabs(dist) < 400 && rand() % 60 == 0) {
        sEnemies[i].velY = 15.0;
        sEnemies[i].isOnGround = false;
        sEnemies[i].isJumping = true;
      }
    }

    // Handle Attack Animation
    if (sEnemies[i].isAttacking) {
      sEnemies[i].attackTimer++;
      int atkDelay = sBossRageMode ? 5 : 8;

      if (sEnemies[i].attackTimer >= atkDelay) {
        sEnemies[i].attackTimer = 0;
        sEnemies[i].attackFrame++;

        int maxFrames = (sEnemies[i].state == STATE_ATTACK1)
                            ? SAS_ATTACK1_FRAMES
                            : SAS_ATTACK2_FRAMES;
        if (sEnemies[i].attackFrame >= maxFrames) {
          sEnemies[i].isAttacking = false;
          sEnemies[i].attackFrame = 0;
          sEnemies[i].state = STATE_STANCE;
        }
      }

      // Melee damage check - Synced with sprites
      // Attack 1: Short range melee combo, hit on frame 4
      // Attack 2: Long range heavy attack, hit on frame 6
      int hitFrame = (sEnemies[i].state == STATE_ATTACK1) ? 4 : 6;
      if (sEnemies[i].attackFrame == hitFrame && sEnemies[i].attackTimer == 0) {
        double range = (sEnemies[i].state == STATE_ATTACK2) ? 140.0 : 100.0;
        if (fabs(sPlayerX - sEnemies[i].x) < range &&
            fabs(sPlayerY - sEnemies[i].y) < 100.0) {
          int dmg = (sEnemies[i].state == STATE_ATTACK2)
                        ? (sBossRageMode ? 35 : 25)
                        : (sBossRageMode ? 20 : 15);
          sPlayerHealth -= dmg;
          sIsTakingDamage = true;
          sDamageFrame = 0;
          sDamageTimer = 0;
          
          // Small knockback
          if (sPlayerX < sEnemies[i].x) sPlayerVelX = -8.0;
          else sPlayerVelX = 8.0;
        }
      }
    }

    // Boss bounds
    if (sEnemies[i].x < 0) sEnemies[i].x = 0;
    if (sEnemies[i].x > sWorldWidth - sEnemies[i].width)
      sEnemies[i].x = sWorldWidth - sEnemies[i].width;

    // Boss Physics
    if (!sEnemies[i].isOnGround) sEnemies[i].velY += GRAVITY;
    sEnemies[i].y += sEnemies[i].velY;
    if (sEnemies[i].y <= GROUND_LEVEL) {
      sEnemies[i].y = GROUND_LEVEL;
      sEnemies[i].velY = 0;
      sEnemies[i].isOnGround = true;
      sEnemies[i].isJumping = false;
    }

    // Boss animation
    if (!sEnemies[i].isAttacking) {
      sEnemies[i].animTimer++;
      int animDelay = sBossRageMode ? sAnimDelay / 2 : sAnimDelay;
      if (sEnemies[i].animTimer >= animDelay) {
        sEnemies[i].animTimer = 0;
        int frameCount = SAS_STANCE_FRAMES;
        switch (sEnemies[i].state) {
        case STATE_STANCE: frameCount = SAS_STANCE_FRAMES; break;
        case STATE_RUN:
        case STATE_WALK: frameCount = SAS_RUN_FRAMES; break;
        case STATE_DAMAGE: frameCount = SAS_DAMAGE_FRAMES; break;
        case STATE_DEATH: frameCount = SAS_DEATH_FRAMES; break;
        default: frameCount = SAS_STANCE_FRAMES; break;
        }

        // Boss death handling
        if (sEnemies[i].state == STATE_DEATH &&
            sEnemies[i].currentFrame == SAS_DEATH_FRAMES - 1) {
          sEnemies[i].isAlive = false;
          sScore += 100;
          sCurrentPhase = BOSS_DEFEATED;
          sIsWinning = true;
          sWinFrame = 0;
          sWinTimer = 0;
        } else if (sEnemies[i].state == STATE_DAMAGE) {
          if (sEnemies[i].currentFrame >= SAS_DAMAGE_FRAMES - 1) {
            sEnemies[i].state = STATE_STANCE;
            sEnemies[i].currentFrame = 0;
          } else {
            sEnemies[i].currentFrame++;
          }
        } else {
          sEnemies[i].currentFrame =
              (sEnemies[i].currentFrame + 1) % frameCount;
        }
      }
    }
  }

  // Player animation
  sPlayerX = fmax(0, fmin(sPlayerX, sWorldWidth - sPlayerW));
  bool isMovingNow = (sPlayerVelX > 0.5 || sPlayerVelX < -0.5);
  if (isMovingNow && sIsOnGround) {
    sAnimTimer++;
    int delay = sIsRunning ? sAnimDelay / 2 : sAnimDelay;
    if (sAnimTimer >= delay) {
      sAnimTimer = 0;
      sCurrentFrame = (sCurrentFrame + 1) % FRAME_COUNT;
    }
  } else if (!isMovingNow && sIsOnGround) {
    sCurrentFrame = 0;
    sAnimTimer = 0;
  }
}

// =====================================================
// KEYBOARD
// =====================================================
void level4Keyboard(unsigned char key) {
  // Code Gate input mode
  if (sCurrentPhase == PHASE_CODE_GATE) {
    if (sCodeGateStep >= 3) return; // All verified

    if (key == 13) { // Enter
      bool match = true;
      const char *expected = sExpectedCodes[sCodeGateStep];
      if (sCodeBufferPos != (int)strlen(expected)) {
        match = false;
      } else {
        for (int i = 0; i < sCodeBufferPos; i++) {
          char a = sCodeBuffer[i];
          char b = expected[i];
          if (a >= 'a' && a <= 'z') a -= 32;
          if (b >= 'a' && b <= 'z') b -= 32;
          if (a != b) { match = false; break; }
        }
      }
      if (match) {
        sCodesVerified[sCodeGateStep] = true;
        sCodeError = false;
        sCodeGateStep++;
        memset(sCodeBuffer, 0, sizeof(sCodeBuffer));
        sCodeBufferPos = 0;

        if (sCodeGateStep >= 3) {
          // All codes entered - start boss fight
          sCurrentPhase = PHASE_BOSS;
          sPlayerX = 100;
          sPlayerY = GROUND_LEVEL;
          sCameraX = 0;
          resetEnemy(0);
        }
      } else {
        sCodeError = true;
        sCodeErrorTimer = 120;
      }
    } else if (key == 8) { // Backspace
      if (sCodeBufferPos > 0) {
        sCodeBufferPos--;
        sCodeBuffer[sCodeBufferPos] = '\0';
      }
      sCodeError = false;
    } else if (sCodeBufferPos < 30) {
      if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') ||
          (key >= '0' && key <= '9') || key == ' ') {
        char c = key;
        if (c >= 'a' && c <= 'z') c -= 32;
        sCodeBuffer[sCodeBufferPos] = c;
        sCodeBufferPos++;
        sCodeBuffer[sCodeBufferPos] = '\0';
        sCodeError = false;
      }
    }
    return;
  }

  if (sIsDying || sIsGameOver)
    return;

  // Phase Skip (debug)
  if (key == '0') {
    for (int i = 0; i < MAX_ENEMIES; i++) {
      sEnemies[i].isAlive = false;
      sEnemies[i].health = 0;
    }
    sCurrentPhase = BOSS_DEFEATED;
    sIsWinning = true;
    sWinFrame = 0;
    sWinTimer = 0;
  }

  if (key == 'a' || key == 'A') {
    sMovingLeft = true;
    sDirection = 1;
  }
  if (key == 'd' || key == 'D') {
    sMovingRight = true;
    sDirection = 0;
  }
  if (key == 'w' || key == 'W' || key == ' ') {
    if (sIsOnGround && !sIsJumping) {
      sPlayerVelY = JUMP_FORCE;
      sIsJumping = true;
      sIsOnGround = false;
      sIsCrouching = false;
    }
  }
  if (key == 's' || key == 'S') {
    if (sIsOnGround)
      sIsCrouching = true;
    else
      sPlayerVelY -= 5.0;
  }
  if (key == 'j' || key == 'J') {
    if (!sIsThrowing) {
      sIsThrowing = true;
      sThrowFrame = 0;
      sThrowTimer = 0;
      sThrowBulletSpawned = false;
    }
  }
  if (key == 'k' || key == 'K') {
    if (!sIsAttacking) {
      sIsAttacking = true;
      sAttackFrame = 0;
      sAttackTimer = 0;
    }
  }
  if (key == 'l' || key == 'L') {
    sIsRunning = true;
  }
  if (key == 'o' || key == 'O' || key == 'm' || key == 'M') {
      if (sPlayerFuel >= 50.0 && !sIsSpecialCombo && !sIsThrowing && !sIsAttacking) {
          sIsSpecialCombo = true;
          sPlayerFuel -= 50.0;
          sSpecialComboTimer = 0;
          sSpecialComboFrame = 0;
      }
  }
}

void level4SpecialKeyboard(unsigned char key) {
  if (sCurrentPhase == PHASE_CODE_GATE || sIsDying || sIsGameOver) return;
  if (key == 100) { sMovingLeft = true; sDirection = 1; }
  if (key == 102) { sMovingRight = true; sDirection = 0; }
  if (key == 101) {
    if (sIsOnGround && !sIsJumping) {
      sPlayerVelY = JUMP_FORCE;
      sIsJumping = true;
      sIsOnGround = false;
      sIsCrouching = false;
    }
  }
  if (key == 103) {
    if (sIsOnGround)
      sIsCrouching = true;
    else
      sPlayerVelY -= 5.0;
  }
}

void level4KeyboardUp(unsigned char key) {
  if (key == 'a' || key == 'A') sMovingLeft = false;
  if (key == 'd' || key == 'D') sMovingRight = false;
  if (key == 's' || key == 'S') sIsCrouching = false;
  if (key == 'l' || key == 'L') sIsRunning = false;
}

void level4SpecialKeyboardUp(unsigned char key) {
  if (key == 100) sMovingLeft = false;
  if (key == 102) sMovingRight = false;
  if (key == 103) sIsCrouching = false;
}

bool isLevel4GameOver() { return sIsGameOver; }
bool isLevel4GameWon() { return sCurrentPhase == BOSS_DEFEATED && sWinDelayTimer >= 3.0; }
