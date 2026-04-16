
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

static bool sLevel1AssetsLoaded = false;

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
static double sWinDelayTimer = 0.0; // Timer for 10s wait

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
  STATE_DEATH
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
  double velX;
  bool isActive;
  double radius;
};

struct EnemyBullet {
  double x, y;
  double velX;
  bool isActive;
  double radius;
  double damage;
};

struct Obstacle {
  double x, y;
  double width, height;
  unsigned int texture;
  bool isActive;
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

static unsigned int sPotionTex = 0;
struct Potion {
  double x, y;
  bool isActive;
};
static Potion sPhasePotion;

// Game Entities
static const int MAX_ENEMIES = 6;
static Enemy sEnemies[MAX_ENEMIES];
static const int MAX_BULLETS = 10;
static Bullet sBullets[MAX_BULLETS];

static const int MAX_ENEMY_BULLETS = 20;
static EnemyBullet sEnemyBullets[MAX_ENEMY_BULLETS];

// Obstacles
static const int MAX_OBSTACLES = 4;
static Obstacle sObstacles[MAX_OBSTACLES];
static unsigned int sObstacleCarTex = 0;
static unsigned int sObstacleTrashTex = 0;

// Player state
static double sPlayerX = 100;
static double sPlayerY = GROUND_LEVEL;
static int sPlayerW = 80;       // Increased from 64
static int sPlayerH = 80;       // Increased from 64
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
static const int sAttackDelay = 6;

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
static int sCurrentStoryGroup = -1; // 0: Start, 1: Phase 3, 2: End
static int sStorySubIndex = 0;
static unsigned int sStoryGroupStart[3] = {0, 0, 0};
static unsigned int sStoryGroupCount[3] = {0, 0, 0};
static unsigned int sStoryTextures[10]; // Max storage for level stories

static void triggerStory(int group) {
  sIsStoryPlaying = true;
  sCurrentStoryGroup = group;
  sStorySubIndex = 0;
}

static void resetEnemy(int index, int minX, int maxX, int type = 0) {
  // Fix: Ensure range is positive
  int range = (maxX > minX) ? (maxX - minX) : 100;
  sEnemies[index].x = minX + (rand() % range);
  sEnemies[index].y = GROUND_LEVEL;
  sEnemies[index].velX = 3.0 + (rand() % 3);
  if (rand() % 2 == 0)
    sEnemies[index].velX *= -1;
  sEnemies[index].direction = (sEnemies[index].velX > 0) ? 0 : 1;
  sEnemies[index].health = (type == 2) ? 350 : 100;
  sEnemies[index].isAlive = true;
  sEnemies[index].currentFrame = 0;
  sEnemies[index].animTimer = 0;
  sEnemies[index].enemyType = type;
  sEnemies[index].state = STATE_STANCE;
  sEnemies[index].isAttacking = false;
  sEnemies[index].attackFrame = 0;
  sEnemies[index].attackTimer = 0;
  sEnemies[index].width = (type == 2) ? sPlayerW : 80;
  sEnemies[index].height = (type == 2) ? sPlayerH : 80;
  sEnemies[index].velY = 0;
  sEnemies[index].isOnGround = true;
  sEnemies[index].isJumping = false;
}

void initLevel1(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  // Guard: only load assets once
  if (sLevel1AssetsLoaded) return;

  // Load backgrounds for scrolling
  sBg2Tex = iLoadImage((char *)"Images/03. Level 1/bg2.png");
  sBgTex = iLoadImage((char *)"Images/03. Level 1/bg.png");

  // Load Story Images
  sStoryTextures[0] = iLoadImage((char *)"Images/Stories _/1.1");
  sStoryTextures[1] = iLoadImage((char *)"Images/Stories _/1.2");
  sStoryTextures[2] = iLoadImage((char *)"Images/Stories _/1.3");
  sStoryTextures[3] = iLoadImage((char *)"Images/Stories _/1.4");
  sStoryTextures[4] = iLoadImage((char *)"Images/Stories _/1.5");
  sStoryTextures[5] = iLoadImage((char *)"Images/Stories _/1.6");

  // Group 0: Start (1.1, 1.2, 1.3)
  sStoryGroupStart[0] = 0;
  sStoryGroupCount[0] = 3;
  // Group 1: Phase 3 (1.4, 1.5)
  sStoryGroupStart[1] = 3;
  sStoryGroupCount[1] = 2;
  // Group 2: End (1.6)
  sStoryGroupStart[2] = 5;
  sStoryGroupCount[2] = 1;

  // Set flag that assets are loaded
  sLevel1AssetsLoaded = true;

  sKakashiHudTex = iLoadImage((char *)"Images/Sprites/Kakashi_hud.png");
  sPotionTex =
      iLoadImage((char *)"Images/Sprites/Collectibles/healing_potion.png");

  // Load Obstacle textures
  sObstacleCarTex =
      iLoadImage((char *)"Images/Sprites/Obstacles/broken_car1.png");
  sObstacleTrashTex = iLoadImage(
      (char *)"Images/Sprites/Obstacles/istockphoto-1163503429-612x612.png");

  // Load Kakashi sprite frames
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

  // Load Kakashi Crouch sprites
  sKakashiCrouchRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Right/CrouchRight1.png");
  sKakashiCrouchRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Right/CrouchRight2.png");
  sKakashiCrouchLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Left/CrouchLeft1.png");
  sKakashiCrouchLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Crouch Left/CrouchLeft2.png");

  // Load Kakashi Damage sprites
  sKakashiDamageRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight1.png");
  sKakashiDamageRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight2.png");
  sKakashiDamageRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight3.png");
  sKakashiDamageRight[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight4.png");
  sKakashiDamageRight[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight5.png");
  sKakashiDamageRight[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Right/DamageRight6.png");
  sKakashiDamageLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft1.png");
  sKakashiDamageLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft2.png");
  sKakashiDamageLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft3.png");
  sKakashiDamageLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft4.png");
  sKakashiDamageLeft[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft5.png");
  sKakashiDamageLeft[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Damage Left/DamageLeft6.png");

  // Load Kakashi Death sprites
  sKakashiDeathRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Right/DeathRight1.png");
  sKakashiDeathRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Right/DeathRight2.png");
  sKakashiDeathRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Right/DeathRight3.png");
  sKakashiDeathRight[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Right/DeathRight4.png");
  sKakashiDeathRight[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Right/DeathRight5.png");
  sKakashiDeathLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Left/DeathLeft1.png");
  sKakashiDeathLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Left/DeathLeft2.png");
  sKakashiDeathLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Left/DeathLeft3.png");
  sKakashiDeathLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Left/DeathLeft4.png");
  sKakashiDeathLeft[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Death Left/DeathLeft5.png");

  // Load Kakashi Walk Right frames
  sKakashiWalkRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight1.png");
  sKakashiWalkRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight2.png");
  sKakashiWalkRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight3.png");
  sKakashiWalkRight[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight4.png");
  sKakashiWalkRight[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight5.png");
  sKakashiWalkRight[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Right/WalkRight6.png");

  // Load Kakashi Walk Left frames
  sKakashiWalkLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft1.png");
  sKakashiWalkLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft2.png");
  sKakashiWalkLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft3.png");
  sKakashiWalkLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft4.png");
  sKakashiWalkLeft[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft5.png");
  sKakashiWalkLeft[5] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Walk Left/WalkLeft6.png");

  // Load Kakashi Jump Right frames
  sKakashiJumpRight[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Right/JumpRight1.png");
  sKakashiJumpRight[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Right/JumpRight2.png");
  sKakashiJumpRight[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Right/JumpRight3.png");
  sKakashiJumpRight[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Right/JumpRight4.png");
  sKakashiJumpRight[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Right/JumpRight5.png");

  // Load Kakashi Jump Left frames
  sKakashiJumpLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft1.png");
  sKakashiJumpLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft2.png");
  sKakashiJumpLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft3.png");
  sKakashiJumpLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft4.png");
  sKakashiJumpLeft[4] = iLoadImage(
      (char *)"Images/Sprites/Hatake Kakashi/Jump Left/JumpLeft5.png");

  // Load Kakashi Attack (Weapon Throw) Right frames
  sKakashiAttackRight[0] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Right/WeaponThrow_Right1.png");
  sKakashiAttackRight[1] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Right/WeaponThrow_Right2.png");
  sKakashiAttackRight[2] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Right/WeaponThrow_Right3.png");

  // Load Kakashi Attack (Weapon Throw) Left frames
  sKakashiAttackLeft[0] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Left/WeaponThrow_Left1.png");
  sKakashiAttackLeft[1] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Left/WeaponThrow_Left2.png");
  sKakashiAttackLeft[2] =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/Weapon Throw "
                         "Left/WeaponThrow_Left3.png");

  // Load Kakashi Run frames
  for (int i = 0; i < RUN_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Run Right/RunRight%d.png",
              i + 1);
    sKakashiRunRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Run Left/RunLeft%d.png",
              i + 1);
    sKakashiRunLeft[i] = iLoadImage(path);
  }

  // Load Kakashi Crouch Walk frames
  for (int i = 0; i < CROUCH_WALK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(
        path,
        "Images/Sprites/Hatake Kakashi/Crouch Walk Right/CrouchWalkRight%d.png",
        i + 1);
    sKakashiCrouchWalkRight[i] = iLoadImage(path);
    sprintf_s(
        path,
        "Images/Sprites/Hatake Kakashi/Crouch Walk Left/CrouchWalkLeft%d.png",
        i + 1);
    sKakashiCrouchWalkLeft[i] = iLoadImage(path);
  }

  // Load Kakashi Knife Attack frames
  for (int i = 0; i < KNIFE_ATTACK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path,
              "Images/Sprites/Hatake Kakashi/Knife Attack "
              "Right/KnifeAttackRight%d.png",
              i + 1);
    sKakashiKnifeAttackRight[i] = iLoadImage(path);
    sprintf_s(
        path,
        "Images/Sprites/Hatake Kakashi/Knife Attack Left/KnifeAttackLeft%d.png",
        i + 1);
    sKakashiKnifeAttackLeft[i] = iLoadImage(path);
  }

  // Load Kakashi Win frames
  for (int i = 0; i < WIN_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Win Right/WinRight%d.png",
              i + 1);
    sKakashiWinRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Win Left/WinLeft%d.png",
              i + 1);
    sKakashiWinLeft[i] = iLoadImage(path);
  }

  sKunaiRightTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_right.png");
  sKunaiLeftTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_left.png");

  // Existing Boss Walk/Attack 1 (reusing for Walk and simple attack)
  sOrochimaruWalkRight[0] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight1.png");
  sOrochimaruWalkRight[1] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight2.png");
  sOrochimaruWalkRight[2] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight3.png");
  sOrochimaruWalkRight[3] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight4.png");
  sOrochimaruWalkRight[4] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight5.png");
  sOrochimaruWalkRight[5] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk RIght/WalkRight6.png");

  sOrochimaruWalkLeft[0] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft1.png");
  sOrochimaruWalkLeft[1] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft2.png");
  sOrochimaruWalkLeft[2] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft3.png");
  sOrochimaruWalkLeft[3] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft4.png");
  sOrochimaruWalkLeft[4] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft5.png");
  sOrochimaruWalkLeft[5] =
      iLoadImage((char *)"Images/Sprites/Orochimaru/Walk Left/WalkLeft6.png");

  sOrochimaruAttackRight[0] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Right/Attack_1_Right1.png");
  sOrochimaruAttackRight[1] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Right/Attack_1_Right2.png");
  sOrochimaruAttackRight[2] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Right/Attack_1_Right3.png");
  sOrochimaruAttackRight[3] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Right/Attack_1_Right4.png");

  sOrochimaruAttackLeft[0] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Left/Attack_1_Left1.png");
  sOrochimaruAttackLeft[1] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Left/Attack_1_Left2.png");
  sOrochimaruAttackLeft[2] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Left/Attack_1_Left3.png");
  sOrochimaruAttackLeft[3] = iLoadImage(
      (char *)"Images/Sprites/Orochimaru/Attack 1 Left/Attack_1_Left4.png");

  // Load new Boss animations
  // Stance
  int stanceSprites[] = {77, 78, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
                         73, 74, 75, 76, 79, 80, 81, 84, 82, 83, 85, 86};
  for (int i = 0; i < ORO_STANCE_FRAMES; i++) {
    char path[256];
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Stance Right/Orochimaru_%04d_sprite_%d.png",
        i, stanceSprites[i]);
    sOroStanceRight[i] = iLoadImage(path);
    int leftIdx = (i == 19) ? 24 : i;
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Stance Left/StanceLeft__%04d_sprite_%d.png",
        leftIdx, stanceSprites[i]);
    sOroStanceLeft[i] = iLoadImage(path);
  }
  // Run
  int runSprites[] = {40, 42, 45, 41, 43, 44};
  for (int i = 0; i < ORO_RUN_FRAMES; i++) {
    char path[256];
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Run Right/RunRight__%04d_sprite_%d.png", i,
        runSprites[i]);
    sOroRunRight[i] = iLoadImage(path);
    sprintf_s(path,
              "Images/Sprites/Orochimaru/Run Left/RunLeft__%04d_sprite_%d.png",
              i, runSprites[i]);
    sOroRunLeft[i] = iLoadImage(path);
  }
  // Damage
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

  // Death
  int deathSprites[] = {105, 106, 104, 90, 91};
  for (int i = 0; i < ORO_DEATH_FRAMES; i++) {
    char path[256];
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Death Right/DeathRight__%04d_sprite_%d.png",
        i, deathSprites[i]);
    sOroDeathRight[i] = iLoadImage(path);
    sprintf_s(
        path,
        "Images/Sprites/Orochimaru/Death Left/Orochimaru_%04d_sprite_%d.png", i,
        deathSprites[i]);
    sOroDeathLeft[i] = iLoadImage(path);
  }
  // Fist
  int fistSprites[] = {1, 3, 4, 2, 5, 6, 7, 8};
  for (int i = 0; i < ORO_FIST_FRAMES; i++) {
    char path[256];
    sprintf_s(path,
              "Images/Sprites/Orochimaru/FIst Attack 1 "
              "RIght/FistAttack_1Right__%04d_sprite_%d.png",
              i, fistSprites[i]);
    sOroFistRight[i] = iLoadImage(path);
    sprintf_s(path,
              "Images/Sprites/Orochimaru/FIst Attack 1 "
              "Left/FistAttack_1Left__%04d_sprite_%d.png",
              i, fistSprites[i]);
    sOroFistLeft[i] = iLoadImage(path);
  }
  // Special
  int specSprites[] = {14, 12, 11, 9, 10, 13, 15, 16};
  for (int i = 0; i < ORO_SPECIAL_FRAMES; i++) {
    char path[256];
    sprintf_s(path,
              "Images/Sprites/Orochimaru/Special Attack 1 "
              "Right/SpecialAttack_1Right__%04d_sprite_%d.png",
              i, specSprites[i]);
    sOroSpecialRight[i] = iLoadImage(path);
    sprintf_s(path,
              "Images/Sprites/Orochimaru/Special Attack 1 "
              "Left/SpecialAttack_1Left__%04d_sprite_%d.png",
              i, specSprites[i]);
    sOroSpecialLeft[i] = iLoadImage(path);
  }

  // Venom
  sVenomTex = iLoadImage((char *)"Images/Sprites/Orochimaru/venom_ball.png");

  // Load Puppet Enemy Walk frames
  for (int i = 0; i < PUPPET_FRAME_COUNT; i++) {
    char path[256];
    
    // Correct pattern: Right_0.png, Right_1.png...
    sprintf_s(path, "Images/Sprites/Puppet Enemy/Right/Right_%d.png", i);
    sPuppetWalkRight[i] = iLoadImage(path);
    
    sprintf_s(path, "Images/Sprites/Puppet Enemy/Left/Left_%d.png", i);
    sPuppetWalkLeft[i] = iLoadImage(path);
  }

  sLevel1AssetsLoaded = true;
}

// Game Over State
static bool sIsGameOver = false;

bool isLevel1GameOver() { return sIsGameOver; }
bool isLevel1GameWon() { return sCurrentPhase == BOSS_DEFEATED; }

void resetLevel1() {
  // Lazy-load assets if not loaded yet
  if (!sLevel1AssetsLoaded) {
    initLevel1(sScreenW > 0 ? sScreenW : 1080, sScreenH > 0 ? sScreenH : 635);
  }

  sScore = 0;
  sPlayerHealth = 100;
  sPlayerLives = 3;
  sPlayerX = 100;
  sPlayerY = GROUND_LEVEL;
  sCameraX = 0;
  sCurrentPhase = PHASE_1;
  sPhaseEnemiesSpawned = 0;
  sPhaseEnemiesKilled = 0;
  sElapsedSeconds = 0;
  sElapsedFrames = 0;
  sIsGameOver = false;
  sIsWinning = false;

  triggerStory(0); // Level 1 Start Story
  sWinDelayTimer = 0.0;

  // Reset physics
  sPlayerVelX = 0;
  sPlayerVelY = 0;
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
  sCrouchFrame = 0;
  sDamageFrame = 0;
  sDeathFrame = 0;
  sWinFrame = 0;

  // Reset Phase
  sCurrentPhase = PHASE_1;
  sPhaseInitiated = true;
  sPhaseEnemiesSpawned = 3; // Pre-spawned below
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

  // Reset Obstacles
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    sObstacles[i].isActive = true;
    sObstacles[i].y = GROUND_LEVEL;
  }
  sObstacles[0].x = 700;
  sObstacles[0].width = 150;
  sObstacles[0].height = 60;
  sObstacles[0].texture = sObstacleCarTex;
  sObstacles[1].x = 1500;
  sObstacles[1].width = 60;
  sObstacles[1].height = 70;
  sObstacles[1].texture = sObstacleTrashTex;
  sObstacles[2].x = 2400;
  sObstacles[2].width = 150;
  sObstacles[2].height = 60;
  sObstacles[2].texture = sObstacleCarTex;
  sObstacles[3].x = 2900;
  sObstacles[3].width = 60;
  sObstacles[3].height = 70;
  sObstacles[3].texture = sObstacleTrashTex;
}

void drawLevel1(int screenW, int screenH);
void updateLevel1();

static void drawLevel1Story(int screenW, int screenH) {
  if (!sIsStoryPlaying)
    return;

  int group = sCurrentStoryGroup;
  int sub = sStorySubIndex;
  unsigned int tex = sStoryTextures[sStoryGroupStart[group] + sub];

  // Draw story full screen
  iShowImage(0, 0, screenW, screenH, tex);

  // Draw skip/next instruction
  iSetColor(255, 255, 255);
  iText(screenW - 200, 30, (char *)"Press 'P' to Continue", (void *)2);
}

void drawLevel1(int screenW, int screenH) {
  if (sIsStoryPlaying) {
    drawLevel1Story(screenW, screenH);
    return;
  }

  iClear();

  // Draw background based on camera position
  // Sequence: bg2, bg2, bg
  for (int i = 0; i < 3; i++) {
    double segX = i * screenW - sCameraX;
    if (segX > -screenW && segX < screenW) {
      if (i < 2)
        iShowImage((int)segX, 0, screenW, screenH, sBg2Tex);
      else
        iShowImage((int)segX, 0, screenW, screenH, sBgTex);
    }
  }

  // Draw Obstacles
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (sObstacles[i].isActive) {
      iShowImage((int)(sObstacles[i].x - sCameraX), (int)sObstacles[i].y,
                 (int)sObstacles[i].width, (int)sObstacles[i].height,
                 sObstacles[i].texture);
    }
  }

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
        case STATE_ATTACK1:
          enemyTex = (dir == 0)
                         ? sOrochimaruAttackRight[sEnemies[i].attackFrame %
                                                  OROCHIMARU_ATTACK_FRAME_COUNT]
                         : sOrochimaruAttackLeft[sEnemies[i].attackFrame %
                                                 OROCHIMARU_ATTACK_FRAME_COUNT];
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
          break;
        default:
          enemyTex = (dir == 0) ? sOroStanceRight[0] : sOroStanceLeft[0];
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

      if (sEnemies[i].enemyType == 2) {
        // Visual power-up effect (Rage mode)
        if (sEnemies[i].health < 200) {
            // Pulse red tint based on frame
            int pulse = 100 + (155 * (sElapsedFrames % 60) / 60);
            iSetColor(255, 255 - pulse/2, 255 - pulse/2);
        } else {
            iSetColor(255, 255, 255);
        }
      } else {
        iSetColor(255, 255, 255);
      }

      iShowImage((int)(sEnemies[i].x - sCameraX), (int)sEnemies[i].y, renderW,
                 renderH, enemyTex);
      iSetColor(255, 255, 255); // Reset color

      // Draw Health Bar
      iSetColor(255, 0, 0);
      iFilledRectangle(sEnemies[i].x - sCameraX,
                       sEnemies[i].y + sEnemies[i].height + 5, 60, 5);
      iSetColor(0, 255, 0);
      double maxHp = (sEnemies[i].enemyType == 2) ? 350.0 : 100.0;
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
      iShowImage((int)(sBullets[i].x - sCameraX - 15), (int)(sBullets[i].y - 5),
                 30, 10, kunaiTex);
    }
  }
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (sEnemyBullets[i].isActive) {
      if (sVenomTex) {
        iShowImage((int)(sEnemyBullets[i].x - sCameraX - 20),
                   (int)(sEnemyBullets[i].y - 20), 40, 40, sVenomTex);
      } else {
        iSetColor(148, 0, 211);
        iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y,
                      sEnemyBullets[i].radius);
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
      iText(screenW / 2.0 - 130.0, screenH / 2.0 + 10.0, (char *)"PUPPET MASTER DEFEATED!", (void *)2);
      iSetColor(200, 200, 200); // Silver subtext
      iText(screenW / 2.0 - 120.0, screenH / 2.0 - 20.0, (char *)"The metro is slightly safer...", (void *)2);
    } else {
      iSetColor(0, 255, 255); // Cyan
      iText(screenW / 2.0 - 120.0, screenH / 2.0 + 10.0, (char *)"SECRET CODE UNLOCKED:", (void *)2);
      iSetColor(255, 215, 0); // Gold
      iText(screenW / 2.0 - 60.0, screenH / 2.0 - 20.0, (char *)"1DC 75UA", (void *)2);
    }
  }

  if (sIsStoryPlaying) {
    drawLevel1Story(screenW, screenH);
  }
}

void updateLevel1() {
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
          sBullets[i].y = sPlayerY + sPlayerH / 2.0;
          sBullets[i].radius = 8;
          sBullets[i].velX = (sDirection == 0) ? 15 : -15;
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
          if (fabs(sPlayerX - sEnemies[i].x) < 80.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 60.0) {
            sEnemies[i].health -= 34; // 3 hits
            sScore += 5;              // +5 for hitting puppet
            if (sEnemies[i].health <= 0) {
              if (sEnemies[i].enemyType == 2) {
                sEnemies[i].health = 1; // keep alive for anim
                if (sEnemies[i].state != STATE_DEATH) {
                  sEnemies[i].state = STATE_DEATH;
                  sEnemies[i].currentFrame = 0;
                  sEnemies[i].animTimer = 0;
                  sEnemies[i].isAttacking = false;
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

  // Update Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      sBullets[i].x += sBullets[i].velX;
      if (sBullets[i].x < sCameraX || sBullets[i].x > sCameraX + sScreenW)
        sBullets[i].isActive = false;
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (sEnemies[j].isAlive && fabs(sBullets[i].x - sEnemies[j].x) < 50 &&
            fabs(sBullets[i].y - sEnemies[j].y) < 50) {
          int kunaiDmg =
              (sEnemies[j].enemyType == 2) ? 10 : 25; // Boss resists kunai
          sEnemies[j].health -= kunaiDmg;
          sScore += 5; // +5 for hitting puppet
          sBullets[i].isActive = false;
          if (sEnemies[j].health <= 0) {
            if (sEnemies[j].enemyType == 2) {
              sEnemies[j].health = 1; // keep alive for anim
              if (sEnemies[j].state != STATE_DEATH) {
                sEnemies[j].state = STATE_DEATH;
                sEnemies[j].currentFrame = 0;
                sEnemies[j].animTimer = 0;
                sEnemies[j].isAttacking = false;
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
        sPlayerX = sCameraX + 100; // Respawn inside camera view
        sPlayerY = GROUND_LEVEL;
        sPlayerVelX = 0;
        sPlayerVelY = 0;
      }
    }
    return;
  }

  // Update position X and resolve horizontal collisions
  sPlayerX += sPlayerVelX;
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (sObstacles[i].isActive) {
      if (sPlayerY < sObstacles[i].y + sObstacles[i].height - 5 &&
          sPlayerY + sPlayerH > sObstacles[i].y + 5) {
        if (sPlayerX + sPlayerW > sObstacles[i].x &&
            sPlayerX < sObstacles[i].x + sObstacles[i].width) {
          if (sPlayerVelX > 0)
            sPlayerX = sObstacles[i].x - sPlayerW;
          else if (sPlayerVelX < 0)
            sPlayerX = sObstacles[i].x + sObstacles[i].width;
          sPlayerVelX = 0;
        }
      }
    }
  }

  // Update position Y and resolve vertical collisions
  double oldY = sPlayerY;
  sPlayerY += sPlayerVelY;
  bool playerOnObstacle = false;

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (sObstacles[i].isActive) {
      // Check for landing on top (falling down or already standing)
      if (sPlayerVelY <= 0 &&
          oldY >= sObstacles[i].y + sObstacles[i].height -
                      2 && // Was above or at the level of top
          sPlayerY <= sObstacles[i].y + sObstacles[i].height +
                          5 && // Now at or below top
          sPlayerX + sPlayerW > sObstacles[i].x + 10 &&
          sPlayerX < sObstacles[i].x + sObstacles[i].width - 10) {
        sPlayerY = sObstacles[i].y + sObstacles[i].height;
        sPlayerVelY = 0;
        sIsOnGround = true;
        sIsJumping = false;
        playerOnObstacle = true;
      }
    }
  }

  // Ground collision
  if (sPlayerY <= GROUND_LEVEL) {
    sPlayerY = GROUND_LEVEL;
    sPlayerVelY = 0;
    sIsOnGround = true;
    sIsJumping = false;
  } else if (!playerOnObstacle) {
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

  // Rest of updateLevel1 is already mostly correct, but ensure enemy spawn is
  // world-aware Check for Phase Transition
  // Randomized Enemy Spawning Logic
  sSpawnTimer++;
  if (sSpawnTimer >= sSpawnInterval) {
    sSpawnTimer = 0;

    // Count active enemies
    int activeCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
      if (sEnemies[i].isAlive)
        activeCount++;

    // Deploy if below limit (10 for P1, 15 for P2)
    if (sCurrentPhase == PHASE_1) {
      if (sPhaseEnemiesSpawned < 10 && activeCount < MAX_ENEMIES) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!sEnemies[i].isAlive) {
            double spawnX = sPlayerX + (400 + rand() % 300);
            if (spawnX > 1080 - 100)
              spawnX = 1080 - 200;
            resetEnemy(i, (int)spawnX, (int)spawnX + 50, 0); // Type 0 : Puppet
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    } else if (sCurrentPhase == PHASE_2) {
      if (sPhaseEnemiesSpawned < 15 && activeCount < MAX_ENEMIES) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!sEnemies[i].isAlive) {
            double spawnX = sPlayerX + (400 + rand() % 300);
            if (spawnX > 2160 - 100)
              spawnX = 2160 - 200;
            resetEnemy(i, (int)spawnX, (int)spawnX + 50,
                       1); // Type 1 : Fast Puppet
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    } else if (sCurrentPhase == PHASE_3) {
      // Boss deployment
      if (sPhaseEnemiesSpawned == 0) {
        resetEnemy(0, 2600, 2800, 2); // Boss
        sPhaseEnemiesSpawned++;
      } else if (sPhaseEnemiesSpawned < 6 && activeCount < MAX_ENEMIES) {
        // Spawn up to 5 Phase 3 puppets
        for (int i = 1; i < MAX_ENEMIES;
             i++) { // Skip index 0 to avoid overwriting boss
          if (!sEnemies[i].isAlive) {
            double spawnX = sPlayerX + (400 + rand() % 300);
            if (spawnX > 3240 - 100)
              spawnX = 3240 - 200;
            resetEnemy(i, (int)spawnX, (int)spawnX + 50, 0); // Type 0
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    }
  }

  // Phase Transitions based on position and kills
  if (sCurrentPhase == PHASE_1 && sPlayerX > 1080 &&
      sPhaseEnemiesKilled >= 10) {
    sCurrentPhase = PHASE_2;
    sPhaseEnemiesSpawned = 0;
    sPhaseEnemiesKilled = 0;
    sSpawnInterval = 100;
    sPhasePotion.isActive = true;
    sPhasePotion.x = sCameraX + 100 + (rand() % 880);
    sPhasePotion.y = GROUND_LEVEL;
  } else if (sCurrentPhase == PHASE_2 && sPlayerX > 2160 &&
             sPhaseEnemiesKilled >= 15) {
    sCurrentPhase = PHASE_3;
    sPhaseEnemiesSpawned = 0;
    sPhaseEnemiesKilled = 0;
    sSpawnInterval = 60; // Faster spawn for Phase 3 puppets
    triggerStory(1);     // Level 1 Phase 3 Story (1.4, 1.5)
    sPhasePotion.isActive = true;
    sPhasePotion.x = sCameraX + 100 + (rand() % 880);
    sPhasePotion.y = GROUND_LEVEL;
  }

  // Potion Collection
  if (sPhasePotion.isActive) {
    if (fabs(sPlayerX - sPhasePotion.x) < 50.0 &&
        fabs(sPlayerY - sPhasePotion.y) < 50.0) {
      sPlayerHealth = 100;
      sPhasePotion.isActive = false;
    }
  }

  // Update Enemies
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive) {
      // Enemy AI based on Type
      double dist = sPlayerX - sEnemies[i].x;
      sEnemies[i].direction = (dist > 0) ? 0 : 1;

      if (sEnemies[i].enemyType == 0 || sEnemies[i].enemyType == 1) { // Puppets
        double speed = (sEnemies[i].enemyType == 1) ? 4.0 : 2.5;

        if (sCurrentPhase == PHASE_3) {
          // Shield AI: Position between player and boss, block the path
          double bossX = sEnemies[0].x;
          double playerDir = (sPlayerX > bossX) ? 1.0 : -1.0;
          // Tight shield wall between player and boss
          double shieldDist = 40.0 + (i * 30.0);
          double targetX = bossX + (playerDir * shieldDist);
          double distToTarget = targetX - sEnemies[i].x;
          double guardSpeed =
              7.0; // Fast enough to keep up with buffed Orochimaru
          if (fabs(distToTarget) > 8.0) {
            sEnemies[i].x += (distToTarget > 0) ? guardSpeed : -guardSpeed;
          }
          // Face and attack player aggressively when close
          double distToPlayer = fabs(sPlayerX - sEnemies[i].x);
          sEnemies[i].direction = (sPlayerX > sEnemies[i].x) ? 0 : 1;
          if (distToPlayer < 90.0 && !sEnemies[i].isAttacking &&
              rand() % 12 < 3) {
            sEnemies[i].isAttacking = true;
            sEnemies[i].attackFrame = 0;
            sEnemies[i].attackTimer = 0;
          }
          // Guard separation - prevent stacking
          for (int j = 0; j < MAX_ENEMIES; j++) {
            if (j != i && sEnemies[j].isAlive &&
                (sEnemies[j].enemyType == 0 || sEnemies[j].enemyType == 1)) {
              double dx = sEnemies[i].x - sEnemies[j].x;
              if (fabs(dx) < 45.0) {
                sEnemies[i].x += (dx > 0) ? 3.0 : -3.0;
              }
            }
          }
        } else {
          // Normal Chasing AI
          if (fabs(dist) > 90.0) {
            sEnemies[i].x += (dist > 0) ? speed : -speed;
            sEnemies[i].state = STATE_WALK;
          } else {
            sEnemies[i].state = STATE_STANCE;
            // Normal attack
            if (!sEnemies[i].isAttacking && rand() % 50 < 2) {
              sEnemies[i].isAttacking = true;
              sEnemies[i].state = STATE_ATTACK1;
              sEnemies[i].attackFrame = 0;
              sEnemies[i].attackTimer = 0;
            }
          }
        }
      } else if (sEnemies[i].enemyType == 2) { // Boss - Orochimaru
        if (sEnemies[i].state == STATE_DAMAGE) {
          // Stay damage until anim finishes
        } else if (sEnemies[i].state == STATE_DEATH) {
          // Stay death until anim finishes
        } else if (!sEnemies[i].isAttacking) {
          // Orochimaru is fast & aggressive
          double speedMult = (sEnemies[i].health < 200) ? 2.0 : 1.3;
          if (fabs(dist) > 250.0) {
            // Running speed - very fast
            sEnemies[i].x +=
                (dist > 0) ? (6.0 * speedMult) : (-6.0 * speedMult);
            sEnemies[i].state = STATE_RUN;
          } else if (fabs(dist) > 80.0) {
            // Walking speed - still quick
            sEnemies[i].x +=
                (dist > 0) ? (3.5 * speedMult) : (-3.5 * speedMult);
            sEnemies[i].state = STATE_WALK;
          } else {
            sEnemies[i].state = STATE_STANCE;
            // Boss attacks - much more frequent
            int rageChance = (sEnemies[i].health < 200) ? 8 : 20;
            if (rand() % rageChance < 3) {
              sEnemies[i].isAttacking = true;
              sEnemies[i].attackFrame = 0;
              sEnemies[i].attackTimer = 0;
              int r = rand() % 4; // Weighted toward ranged
              if (r == 0)
                sEnemies[i].state = STATE_ATTACK1;
              else if (r == 1)
                sEnemies[i].state = STATE_FIST;
              else
                sEnemies[i].state = STATE_SPECIAL; // 50% chance ranged
              sEnemies[i].attackFrame = 0;
              sEnemies[i].attackTimer = 0;
            }
          }
        }
      }

      // Handle Enemy Attack State
      if (sEnemies[i].isAttacking) {
        sEnemies[i].attackTimer++;
        int atkDelay = sAnimDelay;
        if (sEnemies[i].enemyType == 2)
          atkDelay = sAnimDelay / 2; // Orochimaru attacks faster
        if (sEnemies[i].attackTimer >= atkDelay) {
          sEnemies[i].attackTimer = 0;
          sEnemies[i].attackFrame++;

          int maxAttackFrames = OROCHIMARU_ATTACK_FRAME_COUNT;
          if (sEnemies[i].enemyType == 2) {
            if (sEnemies[i].state == STATE_FIST)
              maxAttackFrames = ORO_FIST_FRAMES;
            else if (sEnemies[i].state == STATE_SPECIAL)
              maxAttackFrames = ORO_SPECIAL_FRAMES;
          }

          if (sEnemies[i].attackFrame >= maxAttackFrames) {
            sEnemies[i].isAttacking = false;
            sEnemies[i].attackFrame = 0;

            // Boss: Spawn projectile on attack finish
            if (sEnemies[i].enemyType == 2 && sEnemies[i].state != STATE_FIST) {
              for (int k = 0; k < MAX_ENEMY_BULLETS; k++) {
                if (!sEnemyBullets[k].isActive) {
                  sEnemyBullets[k].isActive = true;
                  sEnemyBullets[k].x = sEnemies[i].x + sEnemies[i].width / 2.0;
                  sEnemyBullets[k].y = sEnemies[i].y + sEnemies[i].height / 2.0;
                  sEnemyBullets[k].velX =
                      (sEnemies[i].direction == 0) ? 14.0 : -14.0;
                  sEnemyBullets[k].damage = 30.0; // Venom damage
                  break;
                }
              }
            }
            if (sEnemies[i].enemyType == 2) {
              sEnemies[i].state = STATE_STANCE;
            }
          }
        }

        // Melee damage check
        int damageCheckFrame = 2; // Default for attack 1
        if (sEnemies[i].enemyType == 2 && sEnemies[i].state == STATE_FIST)
          damageCheckFrame = 5;

        if (sEnemies[i].isAttacking &&
            sEnemies[i].attackFrame == damageCheckFrame &&
            sEnemies[i].attackTimer == 0) {
          if (fabs(sPlayerX - sEnemies[i].x) < 85.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 65.0) {
            int dmg = 10;
            if (sEnemies[i].enemyType == 1)
              dmg = 20;
            else if (sEnemies[i].enemyType == 2)
              dmg = 50; // Orochimaru hits hard
            sPlayerHealth -= dmg;
          }
        }
      }

      // Enemy can move across their respective frames/segments
      double minBound = 0, maxBound = sWorldWidth;

      // Enemies can move freely across the entire world to target the player
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

      // Enemy Physics — free movement (no obstacle collision)
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
      }

      // Animate walk (or stance/damage/death)
      if (!sEnemies[i].isAttacking) {
        sEnemies[i].animTimer++;
        if (sEnemies[i].animTimer >= sAnimDelay) {
          sEnemies[i].animTimer = 0;
          int frameCount = 8; // Default for Puppet Stance/Walk
          if (sEnemies[i].enemyType == 2) {
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

          if (sEnemies[i].enemyType == 2 && sEnemies[i].state == STATE_DEATH &&
              sEnemies[i].currentFrame == ORO_DEATH_FRAMES - 1) {
            sEnemies[i].isAlive = false;
            sPhaseEnemiesKilled++; // Progress the phase properly!
            sScore += 20;          // +20 for beating boss
            sCurrentPhase = BOSS_DEFEATED;
            sIsWinning = true;
            sWinFrame = 0;
            sWinTimer = 0;
            triggerStory(2); // Level 1 End Story (1.6)
          } else if (sEnemies[i].state == STATE_DAMAGE) {
            int maxDmgFrames = (sEnemies[i].enemyType == 2) ? ORO_DAMAGE_FRAMES : 7;
            if (sEnemies[i].currentFrame >= maxDmgFrames - 1) {
                sEnemies[i].state = (sEnemies[i].enemyType == 2) ? STATE_STANCE : STATE_WALK;
                sEnemies[i].currentFrame = 0;
            }
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

void level1Keyboard(unsigned char key) {
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
      sPhaseEnemiesKilled = 10;
      sPlayerX = 1100; // Trigger transition
    } else if (sCurrentPhase == PHASE_2) {
      sPhaseEnemiesKilled = 15;
      sPlayerX = 2200; // Trigger transition
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
    if (!sIsAttacking) {
      sIsAttacking = true;
      sAttackFrame = 0;
      sAttackTimer = 0;
    }
  }
  if (key == 'l' || key == 'L') {
    sIsRunning = true;
  }
}

void level1SpecialKeyboard(unsigned char key) {
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

void level1KeyboardUp(unsigned char key) {
  if (key == 'a' || key == 'A')
    sMovingLeft = false;
  if (key == 'd' || key == 'D')
    sMovingRight = false;
  if (key == 's' || key == 'S')
    sIsCrouching = false;
  if (key == 'l' || key == 'L')
    sIsRunning = false;
}

void level1SpecialKeyboardUp(unsigned char key) {
  if (key == 100) // GLUT_KEY_LEFT
    sMovingLeft = false;
  if (key == 102) // GLUT_KEY_RIGHT
    sMovingRight = false;
  if (key == 103) // GLUT_KEY_DOWN
    sIsCrouching = false;
}

bool isLevel1TransitionReady() {
  return (sCurrentPhase == BOSS_DEFEATED && sWinDelayTimer >= 8.0);
}
