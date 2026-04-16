
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void iClear();
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iText(double x, double y, char *str, void *font = 0);
unsigned int iLoadImage(char filename[]);

// Screen dimensions
static int sScreenW = 0;
static int sScreenH = 0;

// Score and Lives
static int sScore = 0;
static int sPlayerLives = 3;

// Timer
static int sElapsedFrames = 0;       // Frame counter for timer
static double sElapsedSeconds = 0.0; // Elapsed time in seconds

// Backgrounds
static unsigned int sBg2Tex = 0; // bg2.png (Boss phase)
static unsigned int sBgTex = 0;  // bg.png  (Phase 1 & 2)

// Scrolling Camera
static double sCameraX = 0;
static const double sWorldWidth = 3240.0; // 3 * 1080 (3 segments)

// Level Phases
enum LevelPhase { PHASE_1, PHASE_2, PHASE_3, BOSS_DEFEATED };
static LevelPhase sCurrentPhase = PHASE_1;
static bool sPhaseInitiated = false;
static double sWinDelayTimer = 0.0;
static int sPhaseDelayTimer = 0; // 2 sec delay between phases (120 frames)

// Kakashi sprite frames
static const int FRAME_COUNT = 6;
static unsigned int sKakashiRight[FRAME_COUNT];     // Stance Right
static unsigned int sKakashiLeft[FRAME_COUNT];      // Stance Left
static unsigned int sKakashiWalkRight[FRAME_COUNT]; // Walk Right
static unsigned int sKakashiWalkLeft[FRAME_COUNT];  // Walk Left
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

static const int COMBO1_FRAME_COUNT = 13;
static unsigned int sKakashiCombo1Right[COMBO1_FRAME_COUNT];
static unsigned int sKakashiCombo1Left[COMBO1_FRAME_COUNT];

static unsigned int sKunaiRightTex = 0;
static unsigned int sKunaiLeftTex = 0;
static unsigned int sKakashiHudTex = 0;

// Physics constants
static const double GRAVITY =
    -1.2; // Increased for faster, more responsive fall
static const double GROUND_LEVEL = 100;
static const double JUMP_FORCE = 18.0; // Increased for better jump height
static const double FRICTION = 0.4;    // Sharp stop but smooth transition
static const double MAX_HORIZONTAL_SPEED = 10.0; // Increased max speed
static const double ACCELERATION = 2.5; // Very quick ramp-up for responsiveness

// Itachi Boss Sprites
static const int ITA_STANCE_FRAMES = 4;
static unsigned int sItaStanceRight[ITA_STANCE_FRAMES];
static unsigned int sItaStanceLeft[ITA_STANCE_FRAMES];

static const int ITA_RUN_FRAMES = 6;
static unsigned int sItaRunRight[ITA_RUN_FRAMES];
static unsigned int sItaRunLeft[ITA_RUN_FRAMES];

static const int ITA_KICK_FRAMES = 5;
static unsigned int sItaKickRight[ITA_KICK_FRAMES];
static unsigned int sItaKickLeft[ITA_KICK_FRAMES];

static const int ITA_COMBO_FRAMES = 13;
static unsigned int sItaComboRight[ITA_COMBO_FRAMES];
static unsigned int sItaComboLeft[ITA_COMBO_FRAMES];

static const int ITA_DAMAGE_FRAMES = 6;
static unsigned int sItaDamageRight[ITA_DAMAGE_FRAMES];
static unsigned int sItaDamageLeft[ITA_DAMAGE_FRAMES];

static const int ITA_DEATH_FRAMES = 5;
static unsigned int sItaDeathRight[ITA_DEATH_FRAMES];
static unsigned int sItaDeathLeft[ITA_DEATH_FRAMES];

static unsigned int sPuppetVenomTex = 0; // Yellow ball
static unsigned int sPotionTex = 0;
struct Potion {
  double x, y;
  bool isActive;
};
static Potion sPhasePotion;

static bool sLevel2AssetsLoaded = false;
static bool sItachiAssetsLoaded = false;
static bool sItachiLowHealthPotionSpawned = false;

// Structures
// Boss States
enum BossState {
  STATE_STANCE,
  STATE_RUN,
  STATE_WALK,
  STATE_ATTACK1,
  STATE_FIST,
  STATE_SPECIAL,
  STATE_DAMAGE,
  STATE_DEATH,
  STATE_TELEPORT
};

struct Enemy {
  double x, y;
  double velX;
  int direction; // 0 = right, 1 = left
  int health;    // Max 100
  bool isAlive;
  int currentFrame;
  int animTimer;
  int enemyType;   // 0 = Melee, 1 = Ranged, 2 = Boss
  BossState state; // Added state machine for Boss
  bool isAttacking;
  int attackFrame;
  int attackTimer;
  double width, height;
  double velY;
  bool isOnGround;
  bool isJumping;
};

struct Bullet {
  double x, y;
  double velX, velY;
  bool isActive;
  double radius;
};

struct EnemyBullet {
  double x, y;
  double velX;
  bool isActive;
  double radius;
  double damage;
  int type; // 0 = Boss (Purple), 1 = Puppet (Yellow)
};

// Enemy Sprites
static const int OROCHIMARU_FRAME_COUNT = 6;
static unsigned int sOrochimaruWalkRight[OROCHIMARU_FRAME_COUNT];
static unsigned int sOrochimaruWalkLeft[OROCHIMARU_FRAME_COUNT];
static const int OROCHIMARU_ATTACK_FRAME_COUNT = 4;
static unsigned int sOrochimaruAttackRight[OROCHIMARU_ATTACK_FRAME_COUNT];
static unsigned int sOrochimaruAttackLeft[OROCHIMARU_ATTACK_FRAME_COUNT];

static const int ORO_STANCE_FRAMES = 24;
static unsigned int sOroStanceRight[ORO_STANCE_FRAMES];
static unsigned int sOroStanceLeft[ORO_STANCE_FRAMES];

static const int ORO_RUN_FRAMES = 6;
static unsigned int sOroRunRight[ORO_RUN_FRAMES];
static unsigned int sOroRunLeft[ORO_RUN_FRAMES];

static const int ORO_DAMAGE_FRAMES = 2;
static unsigned int sOroDamageRight[ORO_DAMAGE_FRAMES];
static unsigned int sOroDamageLeft[ORO_DAMAGE_FRAMES];

static const int ORO_DEATH_FRAMES = 5;
static unsigned int sOroDeathRight[ORO_DEATH_FRAMES];
static unsigned int sOroDeathLeft[ORO_DEATH_FRAMES];

static const int ORO_FIST_FRAMES = 8;
static unsigned int sOroFistRight[ORO_FIST_FRAMES];
static unsigned int sOroFistLeft[ORO_FIST_FRAMES];

static const int ORO_SPECIAL_FRAMES = 8;
static unsigned int sOroSpecialRight[ORO_SPECIAL_FRAMES];
static unsigned int sOroSpecialLeft[ORO_SPECIAL_FRAMES];

static unsigned int sVenomTex = 0;

static const int PUPPET_FRAME_COUNT = 35;
static unsigned int sPuppetWalkRight[PUPPET_FRAME_COUNT];
static unsigned int sPuppetWalkLeft[PUPPET_FRAME_COUNT];

// Game Entities
static const int MAX_ENEMIES = 7;
static Enemy sEnemies[MAX_ENEMIES];
static const int MAX_BULLETS = 10;
static Bullet sBullets[MAX_BULLETS];

static const int MAX_ENEMY_BULLETS = 20;
static EnemyBullet sEnemyBullets[MAX_ENEMY_BULLETS];

// Player state
static double sPlayerX = 100;
static double sPlayerY = GROUND_LEVEL;
static int sPlayerW = 130;
static int sPlayerH = 130;
static int sPlayerHealth = 100; // Max 100

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
static const int sAttackDelay = 4; // Reduced from 6 for snappier feel
static int sAttackCount = 0;
static int sAttackWindowTimer = 0;
static const int MAX_ATTACK_WINDOW = 35; // Frames to keep the combo count

// Throwing state (J key - projectile with Weapon Throw animation)
static bool sIsThrowing = false;
static int sThrowFrame = 0;
static int sThrowTimer = 0;
static const int sThrowDelay = 6;
static bool sThrowBulletSpawned = false; // Ensure bullet spawns once per throw

// New player states
static bool sIsRunning = false;
static bool sIsWinning = false;
static int sWinFrame = 0;
static int sWinTimer = 0;

static bool sIsCombo1 = false;
static int sCombo1Frame = 0;
static int sCombo1Timer = 0;
static const int sCombo1Delay = 5;

static bool sIsCrouching = false;
static int sCrouchFrame = 0;
static int sCrouchTimer = 0;

static bool sIsTakingDamage = false;
static int sDamageFrame = 0;
static int sDamageTimer = 0;

static bool sIsDying = false;
static int sDeathFrame = 0;
static int sDeathTimer = 0;

// Spawning system
static int sSpawnTimer = 0;
static int sSpawnInterval = 180; // ~3 seconds at 60fps
static int sPhaseEnemiesSpawned = 0;
static int sPhaseEnemiesKilled = 0;

// Story System
static bool sIsStoryPlaying = false;
static int sCurrentStoryGroup = -1; // 1: Phase 3, 2: End
static int sStorySubIndex = 0;
static unsigned int sStoryGroupStart[3] = {0, 0, 0};
static unsigned int sStoryGroupCount[3] = {0, 0, 0};
static unsigned int sStoryTextures[10];

static void triggerStory(int group) {
  sIsStoryPlaying = true;
  sCurrentStoryGroup = group;
  sStorySubIndex = 0;
}

// Helper functions for loading animations
static void loadAnimationSimple(unsigned int *array, const char *pathPattern,
                                int frameCount) {
  for (int i = 0; i < frameCount; i++) {
    char path[256];
    sprintf_s(path, pathPattern, i + 1);
    array[i] = iLoadImage(path);
  }
}

static void loadAnimationMapped(unsigned int *array, const char *pathPattern,
                                int frameCount, const int *spriteMap) {
  for (int i = 0; i < frameCount; i++) {
    char path[256];
    sprintf_s(path, pathPattern, i, spriteMap[i]);
    array[i] = iLoadImage(path);
  }
}

static void resetEnemy(int index, int minX, int maxX, int type = 0) {
  // Fix: Ensure range is positive
  int range = (maxX > minX) ? (maxX - minX) : 100;
  sEnemies[index].x = minX + (rand() % range);
  sEnemies[index].y = GROUND_LEVEL;
  sEnemies[index].velX = 5.0 + (rand() % 4);
  if (rand() % 2 == 0)
    sEnemies[index].velX *= -1;
  sEnemies[index].direction = (sEnemies[index].velX > 0) ? 0 : 1;
  sEnemies[index].health = (type == 3) ? 1000 : ((type == 2) ? 200 : 100);
  sEnemies[index].isAlive = true;
  sEnemies[index].currentFrame = 0;
  sEnemies[index].animTimer = 0;
  sEnemies[index].enemyType = type;
  sEnemies[index].state = STATE_STANCE;
  sEnemies[index].isAttacking = false;
  sEnemies[index].attackFrame = 0;
  sEnemies[index].attackTimer = 0;
  sEnemies[index].width = (type == 2 || type == 3) ? sPlayerW : 120;
  sEnemies[index].height = (type == 2 || type == 3) ? sPlayerH : 120;
  sEnemies[index].velY = 0;
  sEnemies[index].isOnGround = true;
  sEnemies[index].isJumping = false;
  sEnemies[index].attackTimer = 0;
}

static void loadItachiAssets() {
  if (sItachiAssetsLoaded)
    return;

  // Load Itachi Boss Sprites
  int stanceSprites[] = {3, 4, 5, 6}; // i + 3
  loadAnimationMapped(sItaStanceRight,
                      "Images/Sprites/Itachi Uchiha/Stance "
                      "RIght/StanceRight_%04d_sprite_%d.png",
                      ITA_STANCE_FRAMES, stanceSprites);
  loadAnimationMapped(
      sItaStanceLeft,
      "Images/Sprites/Itachi Uchiha/Stance Left/StanceLeft_%04d_sprite_%d.png",
      ITA_STANCE_FRAMES, stanceSprites);

  int runSprites[] = {9, 10, 13, 12, 11, 14};
  loadAnimationMapped(
      sItaRunRight,
      "Images/Sprites/Itachi Uchiha/Run RIght/RunRight_%04d_sprite_%d.png",
      ITA_RUN_FRAMES, runSprites);
  loadAnimationMapped(
      sItaRunLeft,
      "Images/Sprites/Itachi Uchiha/Run Left/RunLeft_%04d_sprite_%d.png",
      ITA_RUN_FRAMES, runSprites);

  int kickSprites[] = {43, 44, 40, 41, 39};
  loadAnimationMapped(
      sItaKickRight,
      "Images/Sprites/Itachi Uchiha/Kick Right/KickRight_%04d_sprite_%d.png",
      ITA_KICK_FRAMES, kickSprites);
  loadAnimationMapped(
      sItaKickLeft,
      "Images/Sprites/Itachi Uchiha/Kick Left/KickLeft_%04d_sprite_%d.png",
      ITA_KICK_FRAMES, kickSprites);

  int comboSprites[] = {30, 29, 26, 27, 32, 33, 34, 28, 35, 36, 37, 38, 31};
  loadAnimationMapped(sItaComboRight,
                      "Images/Sprites/Itachi Uchiha/Combo Attack 1 "
                      "Right/ComboAttack_1Right_%04d_sprite_%d.png",
                      ITA_COMBO_FRAMES, comboSprites);
  loadAnimationMapped(sItaComboLeft,
                      "Images/Sprites/Itachi Uchiha/Combo Attack 1 "
                      "Left/ComboAttack_1Left_%04d_sprite_%d.png",
                      ITA_COMBO_FRAMES, comboSprites);

  int damageSprites[] = {19, 22, 18, 24, 25, 23};
  loadAnimationMapped(sItaDamageRight,
                      "Images/Sprites/Itachi Uchiha/Damage "
                      "Right/DamageRight_%04d_sprite_%d.png",
                      ITA_DAMAGE_FRAMES, damageSprites);
  loadAnimationMapped(
      sItaDamageLeft,
      "Images/Sprites/Itachi Uchiha/Damage Left/DamageLeft_%04d_sprite_%d.png",
      ITA_DAMAGE_FRAMES, damageSprites);

  int deathSprites[] = {19, 22, 18, 24, 25};
  loadAnimationMapped(
      sItaDeathRight,
      "Images/Sprites/Itachi Uchiha/Death Right/DeathRight_%04d_sprite_%d.png",
      ITA_DEATH_FRAMES, deathSprites);
  loadAnimationMapped(
      sItaDeathLeft,
      "Images/Sprites/Itachi Uchiha/Death Left/DeathLeft_%04d_sprite_%d.png",
      ITA_DEATH_FRAMES, deathSprites);

  sItachiAssetsLoaded = true;
}

static void loadLevel2Assets() {
  if (sLevel2AssetsLoaded)
    return;

  sPotionTex =
      iLoadImage((char *)"Images/Sprites/Collectibles/healing_potion.png");
  loadItachiAssets(); // Pre-load to fix lag

  // Load backgrounds
  sBg2Tex = iLoadImage((char *)"Images/Level 2/bg.png");
  sBgTex = iLoadImage((char *)"Images/Level 2/bg.png");

  // Load Story images for Level 2
  // Phase 3 starting (2.4, 2.5, 2.7)
  sStoryTextures[0] = iLoadImage((char *)"Images/Stories _/2.4");
  sStoryTextures[1] = iLoadImage((char *)"Images/Stories _/2.5");
  sStoryTextures[2] = iLoadImage((char *)"Images/Stories _/2.7");
  // Ending (2.8)
  sStoryTextures[3] = iLoadImage((char *)"Images/Stories _/2.8");

  sStoryGroupStart[1] = 0;
  sStoryGroupCount[1] = 3;
  sStoryGroupStart[2] = 3;
  sStoryGroupCount[2] = 1;

  sKakashiHudTex = iLoadImage((char *)"Images/Sprites/Kakashi_hud.png");

  // Load Kakashi animations
  loadAnimationSimple(
      sKakashiRight,
      "Images/Sprites/Hatake Kakashi/Stance Right/StanceRight%d.png",
      FRAME_COUNT);
  loadAnimationSimple(
      sKakashiLeft,
      "Images/Sprites/Hatake Kakashi/Stance Left/StanceLeft%d.png",
      FRAME_COUNT);
  loadAnimationSimple(
      sKakashiCrouchRight,
      "Images/Sprites/Hatake Kakashi/Crouch Right/CrouchRight%d.png",
      CROUCH_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiCrouchLeft,
      "Images/Sprites/Hatake Kakashi/Crouch Left/CrouchLeft%d.png",
      CROUCH_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiDamageRight,
      "Images/Sprites/Hatake Kakashi/Damage Right/DamageRight%d.png",
      DAMAGE_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiDamageLeft,
      "Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft%d.png",
      DAMAGE_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiDeathRight,
      "Images/Sprites/Hatake Kakashi/Death Right/DeathRight%d.png",
      DEATH_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiDeathLeft,
      "Images/Sprites/Hatake Kakashi/Death Left/DeathLeft%d.png",
      DEATH_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiWalkRight,
      "Images/Sprites/Hatake Kakashi/Walk Right/WalkRight%d.png", FRAME_COUNT);
  loadAnimationSimple(sKakashiWalkLeft,
                      "Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft%d.png",
                      FRAME_COUNT);
  loadAnimationSimple(
      sKakashiJumpRight,
      "Images/Sprites/Hatake Kakashi/Jump Right/JumpRight%d.png",
      JUMP_FRAME_COUNT);
  loadAnimationSimple(sKakashiJumpLeft,
                      "Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft%d.png",
                      JUMP_FRAME_COUNT);
  loadAnimationSimple(sKakashiAttackRight,
                      "Images/Sprites/Hatake Kakashi/Weapon Throw "
                      "Right/WeaponThrow_Right%d.png",
                      ATTACK_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiAttackLeft,
      "Images/Sprites/Hatake Kakashi/Weapon Throw Left/WeaponThrow_Left%d.png",
      ATTACK_FRAME_COUNT);
  loadAnimationSimple(sKakashiRunRight,
                      "Images/Sprites/Hatake Kakashi/Run Right/RunRight%d.png",
                      RUN_FRAME_COUNT);
  loadAnimationSimple(sKakashiRunLeft,
                      "Images/Sprites/Hatake Kakashi/Run Left/RunLeft%d.png",
                      RUN_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiCrouchWalkRight,
      "Images/Sprites/Hatake Kakashi/Crouch Walk Right/CrouchWalkRight%d.png",
      CROUCH_WALK_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiCrouchWalkLeft,
      "Images/Sprites/Hatake Kakashi/Crouch Walk Left/CrouchWalkLeft%d.png",
      CROUCH_WALK_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiKnifeAttackRight,
      "Images/Sprites/Hatake Kakashi/Knife Attack Right/KnifeAttackRight%d.png",
      KNIFE_ATTACK_FRAME_COUNT);
  loadAnimationSimple(
      sKakashiKnifeAttackLeft,
      "Images/Sprites/Hatake Kakashi/Knife Attack Left/KnifeAttackLeft%d.png",
      KNIFE_ATTACK_FRAME_COUNT);
  loadAnimationSimple(sKakashiWinRight,
                      "Images/Sprites/Hatake Kakashi/Win Right/WinRight%d.png",
                      WIN_FRAME_COUNT);
  loadAnimationSimple(sKakashiWinLeft,
                      "Images/Sprites/Hatake Kakashi/Win Left/WinLeft%d.png",
                      WIN_FRAME_COUNT);
  loadAnimationSimple(sKakashiCombo1Right,
                      "Images/Sprites/Hatake Kakashi/Combo Attack 1 "
                      "Right/ComboAttack_1Right%d.png",
                      COMBO1_FRAME_COUNT);
  loadAnimationSimple(sKakashiCombo1Left,
                      "Images/Sprites/Hatake Kakashi/Combo Attack 1 "
                      "Left/ComboAttack_1Left%d.png",
                      COMBO1_FRAME_COUNT);

  sPuppetVenomTex =
      iLoadImage((char *)"Images/Sprites/Orochimaru/venom_ball.png");
  sKunaiRightTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_right.png");
  sKunaiLeftTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_left.png");

  // Orochimaru (Boss) animations
  loadAnimationSimple(sOrochimaruWalkRight,
                      "Images/Sprites/Orochimaru/Walk RIght/WalkRight%d.png",
                      OROCHIMARU_FRAME_COUNT);
  loadAnimationSimple(sOrochimaruWalkLeft,
                      "Images/Sprites/Orochimaru/Walk Left/WalkLeft%d.png",
                      OROCHIMARU_FRAME_COUNT);
  loadAnimationSimple(
      sOrochimaruAttackRight,
      "Images/Sprites/Orochimaru/Attack 1 Right/Attack_1_Right%d.png",
      OROCHIMARU_ATTACK_FRAME_COUNT);
  loadAnimationSimple(
      sOrochimaruAttackLeft,
      "Images/Sprites/Orochimaru/Attack 1 Left/Attack_1_Left%d.png",
      OROCHIMARU_ATTACK_FRAME_COUNT);

  int oroStanceSprites[] = {77, 78, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
                            73, 74, 75, 76, 79, 80, 81, 84, 82, 83, 85, 86};
  loadAnimationMapped(
      sOroStanceRight,
      "Images/Sprites/Orochimaru/Stance Right/Orochimaru_%04d_sprite_%d.png",
      ORO_STANCE_FRAMES, oroStanceSprites);
  // Manual fix for Orochimaru stance left index inconsistency
  for (int i = 0; i < ORO_STANCE_FRAMES; i++) {
    char path[256];
    int leftIdx = (i == 19) ? 24 : i;
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Stance Left/StanceLeft__%04d_sprite_%d.png",
        leftIdx, oroStanceSprites[i]);
    sOroStanceLeft[i] = iLoadImage(path);
  }

  int oroRunSprites[] = {40, 42, 45, 41, 43, 44};
  loadAnimationMapped(
      sOroRunRight,
      "Images/Sprites/Orochimaru/Run Right/RunRight__%04d_sprite_%d.png",
      ORO_RUN_FRAMES, oroRunSprites);
  loadAnimationMapped(
      sOroRunLeft,
      "Images/Sprites/Orochimaru/Run Left/RunLeft__%04d_sprite_%d.png",
      ORO_RUN_FRAMES, oroRunSprites);

  sOroDamageRight[0] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Damage Right/DamageRight_1.png");
  sOroDamageRight[1] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Damage Right/DamageRight_22.png");
  sOroDamageLeft[0] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Damage "
                         "Left/DamageLeft__0000_sprite_105.png");
  sOroDamageLeft[1] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Damage "
                         "Left/DamageLeft__0001_sprite_106.png");

  int oroDeathSprites[] = {105, 106, 104, 90, 91};
  loadAnimationMapped(
      sOroDeathRight,
      "Images/Sprites/Orochimaru/Death Right/DeathRight__%04d_sprite_%d.png",
      ORO_DEATH_FRAMES, oroDeathSprites);
  // Special naming for Oro death left
  for (int i = 0; i < ORO_DEATH_FRAMES; i++) {
    char path[256];
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Death Left/Orochimaru_%04d_sprite_%d.png", i,
        oroDeathSprites[i]);
    sOroDeathLeft[i] = iLoadImage(path);
  }

  int oroFistSprites[] = {1, 3, 4, 2, 5, 6, 7, 8};
  loadAnimationMapped(sOroFistRight,
                      "Images/Sprites/Orochimaru/FIst Attack 1 "
                      "RIght/FistAttack_1Right__%04d_sprite_%d.png",
                      ORO_FIST_FRAMES, oroFistSprites);
  loadAnimationMapped(sOroFistLeft,
                      "Images/Sprites/Orochimaru/FIst Attack 1 "
                      "Left/FistAttack_1Left__%04d_sprite_%d.png",
                      ORO_FIST_FRAMES, oroFistSprites);

  int oroSpecSprites[] = {14, 12, 11, 9, 10, 13, 15, 16};
  loadAnimationMapped(sOroSpecialRight,
                      "Images/Sprites/Orochimaru/Special Attack 1 "
                      "Right/SpecialAttack_1Right__%04d_sprite_%d.png",
                      ORO_SPECIAL_FRAMES, oroSpecSprites);
  loadAnimationMapped(sOroSpecialLeft,
                      "Images/Sprites/Orochimaru/Special Attack 1 "
                      "Left/SpecialAttack_1Left__%04d_sprite_%d.png",
                      ORO_SPECIAL_FRAMES, oroSpecSprites);

  sVenomTex = iLoadImage((char *)"Images/Sprites/Orochimaru/venom_ball.png");

  // Puppet Enemy
  for (int i = 0; i < PUPPET_FRAME_COUNT; i++) {
    char path[256];
    
    // Correct pattern: Right_0.png, Right_1.png...
    sprintf_s(path, "Images/Sprites/Puppet Enemy/Right/Right_%d.png", i);
    sPuppetWalkRight[i] = iLoadImage(path);
    
    sprintf_s(path, "Images/Sprites/Puppet Enemy/Left/Left_%d.png", i);
    sPuppetWalkLeft[i] = iLoadImage(path);
  }

  sLevel2AssetsLoaded = true;
}

void initLevel2(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  // Guard: only load assets once
  if (sLevel2AssetsLoaded) return;

  loadLevel2Assets();
}

// Game Over State
static bool sIsGameOver = false;

bool isLevel2GameOver() { return sIsGameOver; }
bool isLevel2GameWon() { return sCurrentPhase == BOSS_DEFEATED; }

void resetLevel2() {
  // Lazy-load assets if not loaded yet
  if (!sLevel2AssetsLoaded) {
    initLevel2(sScreenW > 0 ? sScreenW : 1080, sScreenH > 0 ? sScreenH : 635);
  }

  sIsGameOver = false;
  sPlayerHealth = 100;
  sPlayerX = 100;
  sPlayerY = GROUND_LEVEL;
  sPlayerVelX = 0;
  sPlayerVelY = 0;
  sScore = 0;
  sPlayerLives = 3;
  sElapsedFrames = 0;
  sElapsedSeconds = 0.0;
  sCameraX = 0;
  sWinDelayTimer = 0.0;
  sPhaseDelayTimer = 0;

  // Reset physics
  sIsOnGround = true;
  sIsJumping = false;

  // Reset input
  sMovingLeft = false;
  sMovingRight = false;

  // Reset animation
  sDirection = 0;
  sCurrentFrame = 0;
  sAnimTimer = 0;

  // Reset throw state
  sIsThrowing = false;
  sThrowFrame = 0;
  sThrowTimer = 0;
  sThrowBulletSpawned = false;

  // Reset attack state
  sIsAttacking = false;
  sAttackFrame = 0;
  sAttackTimer = 0;

  // Reset new states
  sIsCrouching = false;
  sIsTakingDamage = false;
  sIsDying = false;
  sIsRunning = false;
  sIsWinning = false;
  sCrouchFrame = 0;
  sDamageFrame = 0;
  sDeathFrame = 0;
  sWinFrame = 0;
  sAttackCount = 0;
  sAttackWindowTimer = 0;
  sIsCombo1 = false;
  sCombo1Frame = 0;
  sCombo1Timer = 0;

  sPhasePotion.isActive = false;
  sItachiLowHealthPotionSpawned = false;

  // Reset Phase
  sCurrentPhase = PHASE_1;
  sPhaseInitiated = true;
  sPhaseEnemiesSpawned = MAX_ENEMIES; // Pre-spawned below
  sPhaseEnemiesKilled = 0;

  // Reset Enemies for Phase 1 (Segment 0)
  srand((unsigned int)time(0));
  for (int i = 0; i < MAX_ENEMIES; i++) {
    resetEnemy(i, 400, 1080, 0);
  }

  // Reset Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    sBullets[i].isActive = false;
    sBullets[i].radius = 10;
  }
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    sEnemyBullets[i].isActive = false;
    sEnemyBullets[i].radius = 8;
  }
}

static void drawLevel2Story(int screenW, int screenH) {
  if (!sIsStoryPlaying)
    return;

  int group = sCurrentStoryGroup;
  int sub = sStorySubIndex;
  unsigned int tex = sStoryTextures[sStoryGroupStart[group] + sub];

  iShowImage(0, 0, screenW, screenH, tex);

  iSetColor(255, 255, 255);
  iText(screenW - 200, 30, (char *)"Press 'P' to Continue", (void *)2);
}

void drawLevel2(int screenW, int screenH) {
  if (sIsStoryPlaying) {
    drawLevel2Story(screenW, screenH);
    return;
  }
  iClear();

  // Draw panorama background for Level 2
  // We draw the single panorama image stretched across the 3 segments
  // Or better, draw it segment by segment if it's not actually 3x width?
  // User said "It's a panorama image", which usually means it's wide.
  // If it's a single image for the whole level, we draw it starting at
  // -sCameraX with width 3240.
  iShowImage((int)(-sCameraX), 0, (int)sWorldWidth, screenH, sBgTex);

  // Draw Potion
  if (sPhasePotion.isActive) {
    iShowImage((int)(sPhasePotion.x - sCameraX), (int)sPhasePotion.y, 50, 50,
               sPotionTex);
  }

  // Draw Kakashi sprite
  unsigned int tex;
  int kakashiW = sPlayerW;
  int kakashiH = sPlayerH;
  int offsetX = 0;
  int offsetY = 0;
  bool isMoving = (sPlayerVelX > 0.5 || sPlayerVelX < -0.5);

  if (sIsWinning) {
    tex = (sDirection == 0) ? sKakashiWinRight[sWinFrame]
                            : sKakashiWinLeft[sWinFrame];
  } else if (sIsDying) {
    tex = (sDirection == 0) ? sKakashiDeathRight[sDeathFrame]
                            : sKakashiDeathLeft[sDeathFrame];
  } else if (sIsCombo1) {
    tex = (sDirection == 0) ? sKakashiCombo1Right[sCombo1Frame]
                            : sKakashiCombo1Left[sCombo1Frame];
    kakashiW = sPlayerW + 30; // Slightly wider for the combo
    if (sDirection == 1) {
      offsetX = -30;
    }
  } else if (sIsTakingDamage) {
    tex = (sDirection == 0) ? sKakashiDamageRight[sDamageFrame]
                            : sKakashiDamageLeft[sDamageFrame];
  } else if (sIsCrouching) {
    if (isMoving) {
      tex =
          (sDirection == 0)
              ? sKakashiCrouchWalkRight[sCurrentFrame % CROUCH_WALK_FRAME_COUNT]
              : sKakashiCrouchWalkLeft[sCurrentFrame % CROUCH_WALK_FRAME_COUNT];
      kakashiW = sPlayerW - 10;               // Make it slightly narrower
      kakashiH = (int)(sPlayerH / 2.0 + 5.0); // Make it slightly shorter
      offsetX = 5;                            // Center it horizontally
    } else {
      tex = (sDirection == 0) ? sKakashiCrouchRight[sCrouchFrame]
                              : sKakashiCrouchLeft[sCrouchFrame];
      kakashiH = (int)(sPlayerH / 2.0 +
                       10.0); // Slightly taller than half for better visual
    }
  } else if (sIsThrowing) {
    tex = (sDirection == 0) ? sKakashiAttackRight[sThrowFrame]
                            : sKakashiAttackLeft[sThrowFrame];
  } else if (sIsAttacking) {
    tex = (sDirection == 0) ? sKakashiKnifeAttackRight[sAttackFrame]
                            : sKakashiKnifeAttackLeft[sAttackFrame];
    kakashiW = sPlayerW + 20; // Make it wider to fit the knife
    if (sDirection == 1) {
      offsetX = -20; // Shift left when facing left so the body stays in place
    }
  } else if (sDirection == 0) { // Right
    if (!sIsOnGround)
      tex = sKakashiJumpRight[sCurrentFrame % JUMP_FRAME_COUNT];
    else if (isMoving)
      tex = sIsRunning ? sKakashiRunRight[sCurrentFrame % RUN_FRAME_COUNT]
                       : sKakashiWalkRight[sCurrentFrame];
    else
      tex = sKakashiRight[sCurrentFrame];
  } else { // Left
    if (!sIsOnGround)
      tex = sKakashiJumpLeft[sCurrentFrame % JUMP_FRAME_COUNT];
    else if (isMoving)
      tex = sIsRunning ? sKakashiRunLeft[sCurrentFrame % RUN_FRAME_COUNT]
                       : sKakashiWalkLeft[sCurrentFrame];
    else
      tex = sKakashiLeft[sCurrentFrame];
  }

  iShowImage((int)(sPlayerX - sCameraX) + offsetX, (int)sPlayerY + offsetY,
             kakashiW, kakashiH, tex);

  // Draw Enemies (relative to camera)
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive) {
      unsigned int enemyTex = 0;
      int renderW = (int)sEnemies[i].width;
      int renderH = (int)sEnemies[i].height;
      int offsetX = 0;
      int offsetY = 0;

      if (sEnemies[i].enemyType == 2) {
        // Boss logic
        int frame = sEnemies[i].currentFrame;
        int dir = sEnemies[i].direction;
        switch (sEnemies[i].state) {
        case STATE_STANCE:
          enemyTex = (dir == 0) ? sOroStanceRight[frame % ORO_STANCE_FRAMES]
                                : sOroStanceLeft[frame % ORO_STANCE_FRAMES];
          break;
        case STATE_RUN:
          enemyTex = (dir == 0) ? sOroRunRight[frame % ORO_RUN_FRAMES]
                                : sOroRunLeft[frame % ORO_RUN_FRAMES];
          break;
        case STATE_WALK:
          enemyTex = (dir == 0)
                         ? sOrochimaruWalkRight[frame % OROCHIMARU_FRAME_COUNT]
                         : sOrochimaruWalkLeft[frame % OROCHIMARU_FRAME_COUNT];
          break;
        case STATE_DAMAGE:
          enemyTex = (dir == 0) ? sOroDamageRight[frame % ORO_DAMAGE_FRAMES]
                                : sOroDamageLeft[frame % ORO_DAMAGE_FRAMES];
          break;
        case STATE_DEATH:
          enemyTex = (dir == 0) ? sOroDeathRight[frame % ORO_DEATH_FRAMES]
                                : sOroDeathLeft[frame % ORO_DEATH_FRAMES];
          break;
        case STATE_TELEPORT:
          enemyTex = (dir == 0) ? sOroStanceRight[0] : sOroStanceLeft[0];
          break;
        case STATE_ATTACK1:
          enemyTex = (dir == 0)
                         ? sOrochimaruAttackRight[sEnemies[i].attackFrame %
                                                  OROCHIMARU_ATTACK_FRAME_COUNT]
                         : sOrochimaruAttackLeft[sEnemies[i].attackFrame %
                                                 OROCHIMARU_ATTACK_FRAME_COUNT];
          renderW += 40;
          if (dir == 1)
            offsetX = -40;
          break;
        case STATE_FIST:
          enemyTex =
              (dir == 0)
                  ? sOroFistRight[sEnemies[i].attackFrame % ORO_FIST_FRAMES]
                  : sOroFistLeft[sEnemies[i].attackFrame % ORO_FIST_FRAMES];
          break;
        case STATE_SPECIAL:
          enemyTex = (dir == 0) ? sOroSpecialRight[sEnemies[i].attackFrame %
                                                   ORO_SPECIAL_FRAMES]
                                : sOroSpecialLeft[sEnemies[i].attackFrame %
                                                  ORO_SPECIAL_FRAMES];
          renderW += 60;
          if (dir == 1)
            offsetX = -60;
          break;
        }
      } else if (sEnemies[i].enemyType == 3) {
        // Itachi Boss logic
        int frame = sEnemies[i].currentFrame;
        int dir = sEnemies[i].direction;
        switch (sEnemies[i].state) {
        case STATE_STANCE:
          enemyTex = (dir == 0) ? sItaStanceRight[frame % ITA_STANCE_FRAMES]
                                : sItaStanceLeft[frame % ITA_STANCE_FRAMES];
          break;
        case STATE_RUN:
          enemyTex = (dir == 0) ? sItaRunRight[frame % ITA_RUN_FRAMES]
                                : sItaRunLeft[frame % ITA_RUN_FRAMES];
          break;
        case STATE_WALK:
          enemyTex = (dir == 0) ? sItaRunRight[frame % ITA_RUN_FRAMES]
                                : sItaRunLeft[frame % ITA_RUN_FRAMES];
          break;
        case STATE_DAMAGE:
          enemyTex = (dir == 0) ? sItaDamageRight[frame % ITA_DAMAGE_FRAMES]
                                : sItaDamageLeft[frame % ITA_DAMAGE_FRAMES];
          break;
        case STATE_DEATH:
          enemyTex = (dir == 0) ? sItaDeathRight[frame % ITA_DEATH_FRAMES]
                                : sItaDeathLeft[frame % ITA_DEATH_FRAMES];
          break;
        case STATE_ATTACK1: // Kick
          enemyTex =
              (dir == 0)
                  ? sItaKickRight[sEnemies[i].attackFrame % ITA_KICK_FRAMES]
                  : sItaKickLeft[sEnemies[i].attackFrame % ITA_KICK_FRAMES];
          renderW += 50;
          if (dir == 1)
            offsetX = -50;
          break;
        case STATE_SPECIAL: // Combo
          enemyTex =
              (dir == 0)
                  ? sItaComboRight[sEnemies[i].attackFrame % ITA_COMBO_FRAMES]
                  : sItaComboLeft[sEnemies[i].attackFrame % ITA_COMBO_FRAMES];
          renderW += 70;
          if (dir == 1)
            offsetX = -70;
          break;
        case STATE_TELEPORT:
          // Just don't draw or draw a specific frame (first frame of death or
          // damage)
          enemyTex = (dir == 0) ? sItaDeathRight[0] : sItaDeathLeft[0];
          break;
        default:
          enemyTex = (dir == 0) ? sItaStanceRight[0] : sItaStanceLeft[0];
          break;
        }
      } else {
        // Puppet logic: State-based rendering using all 35 frames
        int frame = sEnemies[i].currentFrame;
        int dir = sEnemies[i].direction;
        
        switch(sEnemies[i].state) {
          case STATE_STANCE:
            enemyTex = (dir == 0) ? sPuppetWalkRight[frame % 8] : sPuppetWalkLeft[frame % 8];
            break;
          case STATE_RUN:
          case STATE_WALK:
            enemyTex = (dir == 0) ? sPuppetWalkRight[8 + (frame % 8)] : sPuppetWalkLeft[8 + (frame % 8)];
            break;
          case STATE_ATTACK1:
            enemyTex = (dir == 0) ? sPuppetWalkRight[16 + (sEnemies[i].attackFrame % 8)] 
                                  : sPuppetWalkLeft[16 + (sEnemies[i].attackFrame % 8)];
            break;
          case STATE_DAMAGE:
            enemyTex = (dir == 0) ? sPuppetWalkRight[24 + (frame % 7)] : sPuppetWalkLeft[24 + (frame % 7)];
            break;
          case STATE_DEATH:
            enemyTex = (dir == 0) ? sPuppetWalkRight[31 + (frame % 4)] : sPuppetWalkLeft[31 + (frame % 4)];
            break;
          default:
            enemyTex = (dir == 0) ? sPuppetWalkRight[0] : sPuppetWalkLeft[0];
            break;
        }
      }

      iShowImage((int)(sEnemies[i].x - sCameraX) + offsetX,
                 (int)sEnemies[i].y + offsetY, renderW, renderH, enemyTex);

      // Draw Health Bar
      iSetColor(255, 0, 0);
      iFilledRectangle(sEnemies[i].x - sCameraX,
                       sEnemies[i].y + sEnemies[i].height + 5, 60, 5);
      iSetColor(0, 255, 0);
      double maxHp = (sEnemies[i].enemyType == 2) ? 200.0 : 100.0;
      if (sEnemies[i].enemyType == 3)
        maxHp = 1000.0; // Buffed Itachi health
      iFilledRectangle(sEnemies[i].x - sCameraX,
                       sEnemies[i].y + sEnemies[i].height + 5,
                       60 * (sEnemies[i].health / maxHp), 5);
    }
  }

  // Draw Bullets (relative to camera)
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      unsigned int kunaiTex =
          (sBullets[i].velX > 0) ? sKunaiRightTex : sKunaiLeftTex;
      iShowImage((int)(sBullets[i].x - sCameraX - 25), (int)(sBullets[i].y - 9),
                 50, 18, kunaiTex);
    }
  }
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (sEnemyBullets[i].isActive) {
      if (sEnemyBullets[i].type == 1) { // Puppet venom
        iSetColor(255, 255, 0);         // Yellow for venom
        iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y,
                      sEnemyBullets[i].radius);
      } else {
        // Boss Projectiles (Orochimaru Venom or Itachi Fireball)
        if (sEnemyBullets[i].damage > 25.0) {
          // Itachi Fireball - Procedural sprite effect
          iSetColor(255, 69, 0); // Red-Orange outer
          iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y, sEnemyBullets[i].radius);
          iSetColor(255, 165, 0); // Orange middle
          iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y, sEnemyBullets[i].radius * 0.7);
          iSetColor(255, 255, 0); // Yellow core
          iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y, sEnemyBullets[i].radius * 0.4);
        } else if (sVenomTex) {
          // Orochimaru Venom sprite
          double renderSize = sEnemyBullets[i].radius * 5.0;
          iShowImage((int)(sEnemyBullets[i].x - sCameraX - renderSize / 2.0),
                     (int)(sEnemyBullets[i].y - renderSize / 2.0), (int)renderSize,
                     (int)renderSize, sVenomTex);
        } else {
          // Fallback
          iSetColor(148, 0, 211); // Purple
          iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y, sEnemyBullets[i].radius);
        }
      }
    }
  }

  // HUD (Screen absolute)
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
  iSetColor(0, 255, 0); // Green for visibility
  iText(70, screenH - 95, scoreStr);

  // Timer at the top center
  int t = (int)sElapsedSeconds;
  char timeStr[20];
  sprintf_s(timeStr, "TIME: %d:%02d", t / 60, t % 60);
  iSetColor(255, 255, 255);
  iText(screenW / 2.0 - 50.0, screenH - 30.0, timeStr, (void *)2);

  // HUD Phase
  iSetColor(255, 200, 100);
  char phaseStr[30];
  if (sCurrentPhase == PHASE_1)
    sprintf_s(phaseStr, "ARENA 1");
  else if (sCurrentPhase == PHASE_2)
    sprintf_s(phaseStr, "ARENA 2");
  else if (sCurrentPhase == PHASE_3)
    sprintf_s(phaseStr, "BOSS BATTLE");
  else
    sprintf_s(phaseStr, "ARENA CLEAR");
  iText(screenW - 150, screenH - 30, phaseStr);

  // Game Over Screen
  if (sIsGameOver) {
    iSetColor(255, 0, 0);
    iText(screenW / 2.0 - 50.0, screenH / 2.0 + 20.0, (char *)"YOU DIED",
          (void *)2);
    iSetColor(255, 255, 255);
    iText(screenW / 2.0 - 80.0, screenH / 2.0 - 20.0,
          (char *)"Press M for Main Menu");
  } else if (sCurrentPhase == BOSS_DEFEATED) {
    // Cinematic UI Banner
    iSetColor(15, 15, 15); // Deep almost-black banner
    iFilledRectangle(0, screenH / 2.0 - 60.0, screenW, 120.0);
    
    // Gold borders
    iSetColor(218, 165, 32); 
    iFilledRectangle(0, screenH / 2.0 + 58.0, screenW, 4.0);
    iFilledRectangle(0, screenH / 2.0 - 62.0, screenW, 4.0);

    if (sWinDelayTimer < 5.0) {
      iSetColor(50, 255, 50); // Vibrant Green
      iText(screenW / 2.0 - 120.0, screenH / 2.0 + 10.0, (char *)"ITACHI'S SHADOW FALLS!", (void *)2);
      iSetColor(200, 200, 200); // Silver subtext
      iText(screenW / 2.0 - 130.0, screenH / 2.0 - 20.0, (char *)"One step closer to the truth...", (void *)2);
    } else {
      iSetColor(0, 255, 255); // Cyan
      iText(screenW / 2.0 - 120.0, screenH / 2.0 + 10.0, (char *)"SECRET CODE UNLOCKED:", (void *)2);
      iSetColor(255, 215, 0); // Gold
      iText(screenW / 2.0 - 60.0, screenH / 2.0 - 20.0, (char *)"1DC T5UA", (void *)2);
    }
  }
}

void updateLevel2() {
  if (sIsGameOver || sIsDying || sIsStoryPlaying)
    return;

  if (sIsWinning) {
    sWinTimer++;
    if (sWinTimer >= 8) {
      sWinTimer = 0;
      sWinFrame = (sWinFrame + 1) % WIN_FRAME_COUNT;
    }
    // Increment transition timer
    sWinDelayTimer += 0.016;
    return; // Stop other updates while winning
  }

  // Update Timer
  sElapsedFrames++;
  sElapsedSeconds = sElapsedFrames * 0.016;

  // Update Camera (follow player)
  double targetCamX = sPlayerX - 400; // Keep player 400px from left
  if (targetCamX < 0)
    targetCamX = 0;
  if (targetCamX > sWorldWidth - sScreenW)
    targetCamX = sWorldWidth - sScreenW;
  sCameraX = targetCamX;

  // Player animation timers
  if (sIsTakingDamage) {
    sDamageTimer++;
    if (sDamageTimer >= 4) {
      sDamageTimer = 0;
      sDamageFrame++;
      if (sDamageFrame >= DAMAGE_FRAME_COUNT) {
        sIsTakingDamage = false;
        sDamageFrame = 0;
      }
    }
  }
  if (sIsCrouching) {
    sCrouchTimer++;
    if (sCrouchTimer >= 10) {
      sCrouchTimer = 0;
      sCrouchFrame = (sCrouchFrame + 1) % CROUCH_FRAME_COUNT;
    }
  }

  // Handle Throw (J)
  if (sIsThrowing) {
    sThrowTimer++;
    if (sThrowTimer >= sThrowDelay) {
      sThrowTimer = 0;
      sThrowFrame++;
      if (sThrowFrame >= ATTACK_FRAME_COUNT) {
        sIsThrowing = false;
        sThrowFrame = 0;
        sThrowBulletSpawned = false;
      }
    }
    if (sThrowFrame == 1 && !sThrowBulletSpawned) {
      sThrowBulletSpawned = true;
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!sBullets[i].isActive) {
          sBullets[i].isActive = true;
          sBullets[i].x = sPlayerX + sPlayerW / 2.0;
          sBullets[i].y = sPlayerY + sPlayerH * 0.7; // Spawn near hand
          sBullets[i].radius = 8;
          sBullets[i].velX = (sDirection == 0) ? 20 : -20; // Increased speed
          sBullets[i].velY = 0; // Straight horizontal
          break;
        }
      }
    }
  }

  // Handle Melee (K)
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
    if (sAttackFrame == 3 && sAttackTimer == 0) { // Hit on frame 3
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (sEnemies[i].isAlive) {
          if (fabs(sPlayerX - sEnemies[i].x) < 140.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 100.0) {
            sEnemies[i].health -= 34; // 3 hits
            sScore += 5;              // +5 for hitting puppet
            if (sEnemies[i].health <= 0) {
              if (sEnemies[i].enemyType == 2 || sEnemies[i].enemyType == 3) {
                sEnemies[i].health = 1; // keep alive for anim
                if (sEnemies[i].state != STATE_DEATH) {
                  sEnemies[i].state = STATE_DEATH;
                  sEnemies[i].currentFrame = 0;
                  sEnemies[i].animTimer = 0;
                  sEnemies[i].isAttacking = false;
                  sPhaseEnemiesKilled++; // Count boss kill
                }
              } else {
                sEnemies[i].isAlive = false;
                sPhaseEnemiesKilled++;
              }
            } else if (sEnemies[i].state != STATE_DEATH) {
              // Trigger damage state without resetting if already in it
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

  // Handle Attack Window
  if (sAttackCount > 0) {
    sAttackWindowTimer++;
    if (sAttackWindowTimer >= MAX_ATTACK_WINDOW) {
      sAttackCount = 0;
      sAttackWindowTimer = 0;
    }
  }

  // Handle Combo Attack 1 (KKK Triggered)
  if (sIsCombo1) {
    sCombo1Timer++;
    if (sCombo1Timer >= sCombo1Delay) {
      sCombo1Timer = 0;
      sCombo1Frame++;
      if (sCombo1Frame >= COMBO1_FRAME_COUNT) {
        sIsCombo1 = false;
        sCombo1Frame = 0;
      }
    }
    // Damage logic (hits on frames 4, 8, 12)
    if ((sCombo1Frame == 4 || sCombo1Frame == 8 || sCombo1Frame == 12) &&
        sCombo1Timer == 0) {
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (sEnemies[i].isAlive) {
          if (fabs(sPlayerX - sEnemies[i].x) < 140.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 100.0) {
            sEnemies[i].health -= 75; // 75% damage as requested
            sScore += 5;              // +5 for hitting puppet
            if (sEnemies[i].health <= 0) {
              if (sEnemies[i].enemyType == 2 || sEnemies[i].enemyType == 3) {
                sEnemies[i].health = 1;
                if (sEnemies[i].state != STATE_DEATH) {
                  sEnemies[i].state = STATE_DEATH;
                  sEnemies[i].currentFrame = 0;
                  sEnemies[i].animTimer = 0;
                  sEnemies[i].isAttacking = false;
                  sPhaseEnemiesKilled++; // Count boss kill
                }
              } else {
                sEnemies[i].isAlive = false;
                sPhaseEnemiesKilled++;
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

  // Player Health / Lives check
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

  // Update Bullets (Knives)
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      sBullets[i].x += sBullets[i].velX;
      // Removed gravity for horizontal movement

      // Deactivate if goes off screen
      if (sBullets[i].x < sCameraX || sBullets[i].x > sCameraX + sScreenW)
        sBullets[i].isActive = false;

      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (sEnemies[j].isAlive &&
            fabs(sBullets[i].x - (sEnemies[j].x + sEnemies[j].width / 2.0)) <
                sEnemies[j].width / 2.0 &&
            sBullets[i].y > sEnemies[j].y &&
            sBullets[i].y < sEnemies[j].y + sEnemies[j].height) {
          sEnemies[j].health -= 40;
          sScore += 5; // +5 for hitting puppet
          sBullets[i].isActive = false;
          if (sEnemies[j].health <= 0) {
            if (sEnemies[j].enemyType == 2 || sEnemies[j].enemyType == 3) {
              sEnemies[j].health = 1; // keep alive for anim
              if (sEnemies[j].state != STATE_DEATH) {
                sEnemies[j].state = STATE_DEATH;
                sEnemies[j].currentFrame = 0;
                sEnemies[j].animTimer = 0;
                sEnemies[j].isAttacking = false;
                sPhaseEnemiesKilled++; // Count boss kill
              }
            } else {
              sEnemies[j].isAlive = false;
              sPhaseEnemiesKilled++;
            }
          } else if (sEnemies[j].state != STATE_DEATH) {
            // Trigger damage state without resetting if already in it
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

  // Update Enemy Bullets (Boss effectiveness)
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (sEnemyBullets[i].isActive) {
      sEnemyBullets[i].x += sEnemyBullets[i].velX;
      if (sEnemyBullets[i].x < sCameraX ||
          sEnemyBullets[i].x > sCameraX + sScreenW)
        sEnemyBullets[i].isActive = false;

      // Effective collision with hero
      double heroH = sIsCrouching ? 32 : 64;
      if (sEnemyBullets[i].x > sPlayerX &&
          sEnemyBullets[i].x < sPlayerX + sPlayerW &&
          sEnemyBullets[i].y > sPlayerY &&
          sEnemyBullets[i].y < sPlayerY + heroH) {
        sPlayerHealth -= (int)sEnemyBullets[i].damage;
        sIsTakingDamage = true;
        sDamageFrame = 0;
        sDamageTimer = 0;
        sEnemyBullets[i].isActive = false;
      }
    }
  }

  // Update Physics
  // Apply horizontal acceleration based on input
  double currentMaxSpeed =
      sIsRunning ? MAX_HORIZONTAL_SPEED * 1.5 : MAX_HORIZONTAL_SPEED;
  if (sIsCrouching) {
    currentMaxSpeed = MAX_HORIZONTAL_SPEED * 0.5;
  }

  if (sMovingRight) {
    sPlayerVelX += ACCELERATION;
    sDirection = 0;
  } else if (sMovingLeft) {
    sPlayerVelX -= ACCELERATION;
    sDirection = 1;
  } else {
    // Apply friction when no input
    sPlayerVelX *= FRICTION;
  }

  // Clamp horizontal velocity
  if (sPlayerVelX > currentMaxSpeed)
    sPlayerVelX = currentMaxSpeed;
  if (sPlayerVelX < -currentMaxSpeed)
    sPlayerVelX = -currentMaxSpeed;

  // Stop if velocity is very small
  if (sPlayerVelX > -0.1 && sPlayerVelX < 0.1)
    sPlayerVelX = 0;

  // Apply gravity
  if (!sIsOnGround) {
    sPlayerVelY += GRAVITY;
  }

  // Death animation logic
  if (sIsDying) {
    sDeathTimer++;
    if (sDeathTimer >= 8) {
      sDeathTimer = 0;
      sDeathFrame++;
      if (sDeathFrame >= DEATH_FRAME_COUNT) {
        sIsDying = false;
        sDeathFrame = 0;
        sPlayerHealth = 100;
        sPlayerX = 100; // Respawn at start of screen
        sPlayerY = GROUND_LEVEL;
        sPlayerVelX = 0;
        sPlayerVelY = 0;
      }
    }
    return;
  }

  // Update position
  sPlayerX += sPlayerVelX;
  sPlayerY += sPlayerVelY;

  // Ground collision
  if (sPlayerY <= GROUND_LEVEL) {
    sPlayerY = GROUND_LEVEL;
    sPlayerVelY = 0;
    sIsOnGround = true;
    sIsJumping = false;
  } else {
    sIsOnGround = false;
  }

  // World bounds - horizontal
  if (sPlayerX < 0) {
    sPlayerX = 0;
    sPlayerVelX = 0;
  }
  if (sPlayerX > sWorldWidth - sPlayerW) {
    sPlayerX = sWorldWidth - sPlayerW;
    sPlayerVelX = 0;
  }

  // Handle Phase Transitions (Kill-Based)
  if (sPhaseDelayTimer > 0) {
    sPhaseDelayTimer--;
    return; // Pause logic during delay
  }

  if (sCurrentPhase == PHASE_1 && sPhaseEnemiesKilled >= 5) {
    sCurrentPhase = PHASE_2;
    sPhaseEnemiesSpawned = 0;
    sPhaseEnemiesKilled = 0;
    sPhaseDelayTimer = 30; // Brief pause (~0.5s)
    sPhasePotion.isActive = true;
    sPhasePotion.x = sCameraX + 100 + (rand() % 880);
    sPhasePotion.y = GROUND_LEVEL;
  } else if (sCurrentPhase == PHASE_2 &&
             sPhaseEnemiesKilled >= 6) { // 6 Puppets
    sCurrentPhase = PHASE_3;
    sPhaseEnemiesSpawned = 0;
    sPhaseEnemiesKilled = 0;
    sPhaseDelayTimer = 30; // Brief pause (~0.5s)
    triggerStory(1);        // Level 2 Phase 3 Story (2.4, 2.5, 2.7)
    sPhasePotion.isActive = true;
    sPhasePotion.x = sCameraX + 100 + (rand() % 880);
    sPhasePotion.y = GROUND_LEVEL;
  } else if (sCurrentPhase == PHASE_3 && sPhaseEnemiesKilled >= 1) {
    // Only set BOSS_DEFEATED after Itachi's death animation finishes
    // This is handled in the enemy update loop now
  }

  // Potion Collection
  if (sPhasePotion.isActive) {
    if (fabs(sPlayerX - sPhasePotion.x) < 50.0 &&
        fabs(sPlayerY - sPhasePotion.y) < 50.0) {
      sPlayerHealth = 100;
      sPhasePotion.isActive = false;
    }
  }

  // Mid-boss potion: spawn when Itachi health drops below 50
  if (sCurrentPhase == PHASE_3 && !sItachiLowHealthPotionSpawned) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (sEnemies[i].isAlive && sEnemies[i].enemyType == 3 &&
          sEnemies[i].health <= 500 && sEnemies[i].health > 0) {
        sPhasePotion.isActive = true;
        sPhasePotion.x = sCameraX + 100 + (rand() % 880);
        sPhasePotion.y = GROUND_LEVEL;
        sItachiLowHealthPotionSpawned = true;
        break;
      }
    }
  }

  // Randomized Enemy Spawning Logic
  sSpawnTimer++;
  if (sSpawnTimer >= sSpawnInterval) {
    sSpawnTimer = 0;

    // Count active enemies
    int activeCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
      if (sEnemies[i].isAlive)
        activeCount++;

    // Deploy if below limit
    if (sCurrentPhase == PHASE_1) {
      if (sPhaseEnemiesSpawned < 5 && activeCount < MAX_ENEMIES) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!sEnemies[i].isAlive) {
            double spawnX = (rand() % 2 == 0) ? sCameraX - 200.0
                                              : sCameraX + sScreenW + 200.0;
            resetEnemy(i, (int)spawnX, (int)(spawnX + 50.0), 0);
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    } else if (sCurrentPhase == PHASE_2) {
      // Phase 2: 6 Ranged Puppets
      if (sPhaseEnemiesSpawned < 6 && activeCount < MAX_ENEMIES) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!sEnemies[i].isAlive) {
            double spawnX = (rand() % 2 == 0) ? sCameraX - 200.0
                                              : sCameraX + sScreenW + 200.0;
            resetEnemy(i, (int)spawnX, (int)(spawnX + 50.0),
                       1); // Type 1 = Ranged
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    } else if (sCurrentPhase == PHASE_3) {
      // Phase 3: Itachi Boss
      bool itachiAlive = false;
      for (int i = 0; i < MAX_ENEMIES; i++)
        if (sEnemies[i].isAlive && sEnemies[i].enemyType == 3)
          itachiAlive = true;

      if (!itachiAlive && sPhaseEnemiesSpawned == 0) {
        resetEnemy(0, (int)(sCameraX + sScreenW + 200),
                   (int)(sCameraX + sScreenW + 250), 3);
        sPhaseEnemiesSpawned++;
      }
    }
  }

  // Update Enemies
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive) {
      // Enemy AI based on Type
      double dist = sPlayerX - sEnemies[i].x;
      sEnemies[i].direction = (dist > 0) ? 0 : 1;

      if (sEnemies[i].enemyType == 2 || sEnemies[i].enemyType == 3) { // Bosses
        if (sEnemies[i].state == STATE_DAMAGE) {
          // Damage animation
        } else if (sEnemies[i].state == STATE_DEATH) {
          // Death animation
        } else if (!sEnemies[i].isAttacking) {
          // Buffed Itachi Stats & AI
          double speedMult =
              (sEnemies[i].enemyType == 3) ? 1.8 : 1.0; // Itachi is base 1.8x
          if (sEnemies[i].health < 300)
            speedMult = (sEnemies[i].enemyType == 3) ? 2.5 : 1.5; // Rage mode

          if (sEnemies[i].enemyType == 3) {
            // Teleport logic: if player is attacking or too close/far
            if (rand() % 100 < 2 && sEnemies[i].state != STATE_TELEPORT) {
              sEnemies[i].state = STATE_TELEPORT;
              sEnemies[i].currentFrame = 0;
              sEnemies[i].animTimer = 0;
              // Teleport behind player
              double offset = (rand() % 2 == 0) ? -150.0 : 150.0;
              sEnemies[i].x = sPlayerX + offset;
            }
          }

          if (fabs(dist) > 350.0) {
            sEnemies[i].x +=
                (dist > 0) ? (5.0 * speedMult) : (-5.0 * speedMult);
            sEnemies[i].state = STATE_RUN;
          } else if (fabs(dist) > 100.0) {
            sEnemies[i].x +=
                (dist > 0) ? (3.0 * speedMult) : (-3.0 * speedMult);
            sEnemies[i].state = STATE_WALK;
          } else {
            sEnemies[i].state = STATE_STANCE;
            int attackChance = (sEnemies[i].enemyType == 3) ? 15 : 40;
            if (rand() % attackChance < 3) {
              sEnemies[i].isAttacking = true;
              sEnemies[i].attackFrame = 0;
              sEnemies[i].attackTimer = 0;
              if (sEnemies[i].enemyType == 3) {
                int r = rand() % 3;
                if (r == 0)
                  sEnemies[i].state = STATE_ATTACK1;
                else if (r == 1)
                  sEnemies[i].state = STATE_SPECIAL;
                else {
                  // Fireball / Projectile pressure
                  for (int k = 0; k < MAX_ENEMY_BULLETS; k++) {
                    if (!sEnemyBullets[k].isActive) {
                      sEnemyBullets[k].isActive = true;
                      sEnemyBullets[k].x =
                          sEnemies[i].x + sEnemies[i].width / 2.0;
                      sEnemyBullets[k].y =
                          sEnemies[i].y + sEnemies[i].height / 2.0;
                      sEnemyBullets[k].velX = (dist > 0) ? 15.0 : -15.0;
                      sEnemyBullets[k].damage = 30.0;
                      sEnemyBullets[k].radius = 12.0;
                      sEnemyBullets[k].type = 0; // Thematic black/purple
                      break;
                    }
                  }
                  sEnemies[i].isAttacking = false; // Instant shot
                }
              } else {
                int r = rand() % 3;
                if (r == 0)
                  sEnemies[i].state = STATE_ATTACK1;
                else if (r == 1)
                  sEnemies[i].state = STATE_FIST;
                else
                  sEnemies[i].state = STATE_SPECIAL;
              }
            }
          }
        }
      } else { // Puppets
        double speed = (sEnemies[i].enemyType == 1) ? 8.0 : 6.0;
        if (sCurrentPhase == PHASE_2) {
          // Ranged AI: keep distance and shoot
          if (fabs(dist) > 400.0) {
            sEnemies[i].x += (dist > 0) ? speed : -speed;
          } else if (fabs(dist) < 200.0) {
            sEnemies[i].x += (dist > 0) ? -speed : speed; // Back away
          }

          // Occasional jumping
          if (sEnemies[i].isOnGround && rand() % 100 < 2) {
            sEnemies[i].velY = JUMP_FORCE;
            sEnemies[i].isJumping = true;
            sEnemies[i].isOnGround = false;
          }
          // Ranged attack - moderate frequency
          if (!sEnemies[i].isAttacking && rand() % 80 < 1) {
            for (int k = 0; k < MAX_ENEMY_BULLETS; k++) {
              if (!sEnemyBullets[k].isActive) {
                sEnemyBullets[k].isActive = true;
                sEnemyBullets[k].x = sEnemies[i].x + sEnemies[i].width / 2.0;
                sEnemyBullets[k].y = sEnemies[i].y + sEnemies[i].height / 2.0;
                sEnemyBullets[k].velX =
                    (sEnemies[i].direction == 0) ? 10.0 : -10.0;
                sEnemyBullets[k].damage = 15.0; // Puppet venom
                sEnemyBullets[k].type = 1;
                break;
              }
            }
          }
        } else {
          // Phase 1 chasing - spread out to avoid stacking
          double offset =
              (i - MAX_ENEMIES / 2.0) * 60.0; // Each puppet has unique offset
          double targetX = sPlayerX + offset;
          if (fabs(sEnemies[i].x - targetX) > 15.0) {
            sEnemies[i].x += (targetX > sEnemies[i].x) ? speed : -speed;
            sEnemies[i].state = STATE_WALK;
          } else {
            sEnemies[i].state = STATE_STANCE;
          }
          // Attack when close enough
          if (fabs(dist) < 90.0 && !sEnemies[i].isAttacking &&
              rand() % 30 < 2) {
            sEnemies[i].isAttacking = true;
            sEnemies[i].state = STATE_ATTACK1;
            sEnemies[i].attackFrame = 0;
            sEnemies[i].attackTimer = 0;
          }
        }
        // Puppet separation - push apart if too close to each other
        for (int j = 0; j < MAX_ENEMIES; j++) {
          if (j != i && sEnemies[j].isAlive && sEnemies[j].enemyType != 2 &&
              sEnemies[j].enemyType != 3) {
            double dx = sEnemies[i].x - sEnemies[j].x;
            if (fabs(dx) < 50.0) {
              sEnemies[i].x += (dx > 0) ? 2.0 : -2.0; // Push apart
            }
          }
        }
      }

      // Handle Enemy Attack State
      if (sEnemies[i].isAttacking) {
        sEnemies[i].attackTimer++;
        int atkDelay = sAnimDelay;
        if (sEnemies[i].enemyType == 3)
          atkDelay = sAnimDelay / 2; // Itachi attacks faster

        if (sEnemies[i].attackTimer >= atkDelay) {
          sEnemies[i].attackTimer = 0;
          sEnemies[i].attackFrame++;

          int maxAttackFrames = OROCHIMARU_ATTACK_FRAME_COUNT;
          if (sEnemies[i].enemyType == 2) {
            if (sEnemies[i].state == STATE_FIST)
              maxAttackFrames = ORO_FIST_FRAMES;
            else if (sEnemies[i].state == STATE_SPECIAL)
              maxAttackFrames = ORO_SPECIAL_FRAMES;
          } else if (sEnemies[i].enemyType == 3) {
            if (sEnemies[i].state == STATE_ATTACK1)
              maxAttackFrames = ITA_KICK_FRAMES;
            else if (sEnemies[i].state == STATE_SPECIAL)
              maxAttackFrames = ITA_COMBO_FRAMES;
          }

          if (sEnemies[i].attackFrame >= maxAttackFrames) {
            sEnemies[i].isAttacking = false;

            // Save attack state before resetting to STANCE
            BossState finishedState = sEnemies[i].state;
            sEnemies[i].state =
                STATE_STANCE; // Bosses return to stance after attack

            // Boss: Spawn projectile on attack finish
            if ((sEnemies[i].enemyType == 2 && finishedState != STATE_FIST) ||
                (sEnemies[i].enemyType == 3 && finishedState == STATE_SPECIAL)) {
              for (int k = 0; k < MAX_ENEMY_BULLETS; k++) {
                if (!sEnemyBullets[k].isActive) {
                  sEnemyBullets[k].isActive = true;
                  sEnemyBullets[k].x = sEnemies[i].x + sEnemies[i].width / 2.0;
                  sEnemyBullets[k].y = sEnemies[i].y + sEnemies[i].height / 2.0;
                  sEnemyBullets[k].velX =
                      (sEnemies[i].direction == 0) ? 12.0 : -12.0;
                  sEnemyBullets[k].damage = (sEnemies[i].enemyType == 3) ? 30.0 : 20.0;
                  sEnemyBullets[k].type = 0;
                  sEnemyBullets[k].radius = 12.0;
                  break;
                }
              }
            }
          }
        }

        // Melee damage check
        int damageCheckFrame = 2;
        if (sEnemies[i].enemyType == 2 && sEnemies[i].state == STATE_FIST)
          damageCheckFrame = 5;
        if (sEnemies[i].enemyType == 3 && sEnemies[i].state == STATE_ATTACK1)
          damageCheckFrame = 3;
        if (sEnemies[i].enemyType == 3 && sEnemies[i].state == STATE_SPECIAL)
          damageCheckFrame = 10;

        if (sEnemies[i].isAttacking &&
            sEnemies[i].attackFrame == (int)damageCheckFrame &&
            sEnemies[i].attackTimer == 0) {
          double rangeX = (sEnemies[i].enemyType >= 2) ? 95.0 : 85.0;
          double rangeY = (sEnemies[i].enemyType >= 2) ? 80.0 : 65.0;
          if (fabs(sPlayerX - sEnemies[i].x) < rangeX &&
              fabs(sPlayerY - sEnemies[i].y) < rangeY) {
            int dmg = (sEnemies[i].enemyType == 3)
                          ? 10
                          : ((sEnemies[i].enemyType == 2) ? 40 : 10);
            sPlayerHealth -= dmg;
            sIsTakingDamage = true;
            sDamageFrame = 0;
            sDamageTimer = 0;
          }
        }
      }

      // Enemy can move across their respective frames/segments
      double minBound = 0, maxBound = sWorldWidth;

      // Enemies can move freely across the entire world to target the
      // player
      if (sCurrentPhase == PHASE_1 || sCurrentPhase == PHASE_2 ||
          sCurrentPhase == PHASE_3) {
        minBound = 0;
        maxBound = sWorldWidth;
      } else {
        minBound = 0;
        maxBound = sWorldWidth;
      }

      if (sEnemies[i].x < minBound)
        sEnemies[i].x = minBound;
      if (sEnemies[i].x > maxBound - sEnemies[i].width)
        sEnemies[i].x = maxBound - sEnemies[i].width;

      // Apply gravity to enemies
      if (!sEnemies[i].isOnGround) {
        sEnemies[i].velY += GRAVITY;
      }
      sEnemies[i].y += sEnemies[i].velY;

      // Ground collision
      if (sEnemies[i].y <= GROUND_LEVEL) {
        sEnemies[i].y = GROUND_LEVEL;
        sEnemies[i].velY = 0;
        sEnemies[i].isOnGround = true;
        sEnemies[i].isJumping = false;
      } else {
        sEnemies[i].isOnGround = false;
      }

      // Horizontal bounds check (after physics/collision)
      if (sEnemies[i].x < minBound)
        sEnemies[i].x = minBound;
      if (sEnemies[i].x > maxBound - sEnemies[i].width)
        sEnemies[i].x = maxBound - sEnemies[i].width;

      // Animate walk (or stance/damage/death)
      if (!sEnemies[i].isAttacking) {
        sEnemies[i].animTimer++;
        int delay = sAnimDelay;
        if (sEnemies[i].enemyType == 3)
          delay = sAnimDelay / 2; // Itachi is twice as fast/smooth

        if (sEnemies[i].animTimer >= delay) {
          sEnemies[i].animTimer = 0;
          int frameCount = 1;

          if (sEnemies[i].enemyType ==
              2) { // Orochimaru (Mini-boss or Phase 2 remnant)
            switch (sEnemies[i].state) {
            case STATE_STANCE:
              frameCount = ORO_STANCE_FRAMES;
              break;
            case STATE_RUN:
            case STATE_WALK:
              frameCount = ORO_RUN_FRAMES;
              break;
            case STATE_DAMAGE:
              frameCount = ORO_DAMAGE_FRAMES;
              break;
            case STATE_DEATH:
              frameCount = ORO_DEATH_FRAMES;
              break;
            default:
              frameCount = OROCHIMARU_FRAME_COUNT;
              break;
            }
          } else if (sEnemies[i].enemyType == 3) { // Itachi (Final Boss)
            switch (sEnemies[i].state) {
            case STATE_STANCE:
              frameCount = ITA_STANCE_FRAMES;
              break;
            case STATE_RUN:
            case STATE_WALK:
              frameCount = ITA_RUN_FRAMES;
              break;
            case STATE_DAMAGE:
              frameCount = ITA_DAMAGE_FRAMES;
              break;
            case STATE_DEATH:
              frameCount = ITA_DEATH_FRAMES;
              break;
            case STATE_TELEPORT:
              frameCount = 6;
              break; // Stay in "vanish" for 6 updates at half-delay (3
                     // frames worth)
            default:
              frameCount = ITA_STANCE_FRAMES;
              break;
            }
          } else {
            // Puppet limits
            if (sEnemies[i].state == STATE_DAMAGE)
              frameCount = 7;
            else if (sEnemies[i].state == STATE_DEATH)
              frameCount = 4;
            else
              frameCount = 8;
          }

          sEnemies[i].currentFrame = (sEnemies[i].currentFrame + 1) % frameCount;

          if (sEnemies[i].enemyType < 2 && sEnemies[i].state == STATE_DAMAGE &&
              sEnemies[i].currentFrame == 0) {
            sEnemies[i].state = STATE_STANCE;
          }

          // Handle state transitions for bosses
          if ((sEnemies[i].enemyType == 2 || sEnemies[i].enemyType == 3) &&
              sEnemies[i].state == STATE_DEATH &&
              sEnemies[i].currentFrame == frameCount - 1) {
            sEnemies[i].isAlive = false;
            sScore += 20;                     // +20 for beating boss
            if (sEnemies[i].enemyType == 3) { // WIN condition
              sCurrentPhase = BOSS_DEFEATED;
              sIsWinning = true;
              sWinFrame = 0;
              sWinTimer = 0;
              sWinDelayTimer = 0;
              triggerStory(2); // Level 2 End Story (2.8)
            }
          } else if (sEnemies[i].state == STATE_DAMAGE &&
                     sEnemies[i].currentFrame >= frameCount - 1) {
            sEnemies[i].state = STATE_STANCE;
            sEnemies[i].currentFrame = 0;
          } else if (sEnemies[i].state == STATE_TELEPORT &&
                     sEnemies[i].currentFrame >= frameCount - 1) {
            sEnemies[i].state = STATE_STANCE; // Transition back after teleport
            sEnemies[i].currentFrame = 0;
          }

          sEnemies[i].currentFrame++;
          if (sEnemies[i].currentFrame >= frameCount) {
            if (sEnemies[i].state == STATE_DEATH)
              sEnemies[i].currentFrame = frameCount - 1;
            else
              sEnemies[i].currentFrame = 0;
          }
        }
      }
    }
  }

  // Final bounds and frame animation
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

void level2Keyboard(unsigned char key) {
  if (sIsStoryPlaying && (key == 'p' || key == 'P')) {
    sStorySubIndex++;
    if (sStorySubIndex >= (int)sStoryGroupCount[sCurrentStoryGroup]) {
      sIsStoryPlaying = false;
    }
    return;
  }

  if (sIsDying || sIsGameOver || sIsStoryPlaying)
    return;

  // Phase Skip
  if (key == '0') {
    for (int i = 0; i < MAX_ENEMIES; i++) {
      sEnemies[i].isAlive = false;
      sEnemies[i].health = 0;
    }
    if (sCurrentPhase == PHASE_1) {
      sPhaseEnemiesKilled = 5;
    } else if (sCurrentPhase == PHASE_2) {
      sPhaseEnemiesKilled = 6;
    } else if (sCurrentPhase == PHASE_3) {
      sCurrentPhase = BOSS_DEFEATED;
      sIsWinning = true;
      sWinFrame = 0;
      sWinTimer = 0;
    }
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
    if (sIsOnGround) {
      sIsCrouching = true;
    } else
      sPlayerVelY -= 5.0; // Fast fall
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
    if (!sIsCombo1 && !sIsThrowing) {
      sAttackCount++;
      sAttackWindowTimer = 0; // Reset window timer on every tap

      if (sAttackCount >= 3) {
        sIsCombo1 = true;
        sCombo1Frame = 0;
        sCombo1Timer = 0;
        sIsAttacking = false; // Cancel normal attack
        sAttackCount = 0;
      } else {
        sIsAttacking = true;
        sAttackFrame = 0;
        sAttackTimer = 0;
      }
    }
  }
  if (key == 'l' || key == 'L') {
    sIsRunning = true;
  }
}

void level2SpecialKeyboard(unsigned char key) {
  if (sIsDying || sIsGameOver)
    return;
  if (key == 100) { // GLUT_KEY_LEFT
    sMovingLeft = true;
    sDirection = 1;
  }
  if (key == 102) { // GLUT_KEY_RIGHT
    sMovingRight = true;
    sDirection = 0;
  }
  if (key == 101) { // GLUT_KEY_UP
    if (sIsOnGround && !sIsJumping) {
      sPlayerVelY = JUMP_FORCE;
      sIsJumping = true;
      sIsOnGround = false;
      sIsCrouching = false;
    }
  }
  if (key == 103) { // GLUT_KEY_DOWN
    if (sIsOnGround) {
      sIsCrouching = true;
    } else
      sPlayerVelY -= 5.0;
  }
}

void level2KeyboardUp(unsigned char key) {
  if (key == 'a' || key == 'A')
    sMovingLeft = false;
  if (key == 'd' || key == 'D')
    sMovingRight = false;
  if (key == 's' || key == 'S')
    sIsCrouching = false;
  if (key == 'l' || key == 'L')
    sIsRunning = false;
}

void level2SpecialKeyboardUp(unsigned char key) {
  if (key == 100) // GLUT_KEY_LEFT
    sMovingLeft = false;
  if (key == 102) // GLUT_KEY_RIGHT
    sMovingRight = false;
  if (key == 103) // GLUT_KEY_DOWN
    sIsCrouching = false;
}

bool isLevel2TransitionReady() {
  return (sCurrentPhase == BOSS_DEFEATED && sWinDelayTimer >= 8.0);
}
