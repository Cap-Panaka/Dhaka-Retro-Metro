
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

static bool sLevel3AssetsLoaded = false;

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
static unsigned int sBg1Tex = 0; // hehe1.png (Segment 1)
static unsigned int sBg2Tex = 0; // hehe2.png (Segment 2)
static unsigned int sBg3Tex = 0; // hehe3.png (Segment 3)
static unsigned int sBgTex = 0;  // Fallback / legacy

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

static const int SPECIAL_ATTACK_FRAME_COUNT = 30;
static unsigned int sKakashiSpecialRight[SPECIAL_ATTACK_FRAME_COUNT];
static unsigned int sKakashiSpecialLeft[SPECIAL_ATTACK_FRAME_COUNT];

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
static const double FRICTION = 0.0;    // Absolute crisp stop to prevent sliding
static const double MAX_HORIZONTAL_SPEED = 10.0; // Increased max speed
static const double ACCELERATION = 10.0; // Instant ramp-up for responsiveness

// Chidori / Special Combo Constants
static const int SPECIAL_BLAST_LENGTH = 400;
static const int SPECIAL_BLAST_HEIGHT = 60;
static const int SPECIAL_BLAST_DAMAGE = 50; // Nerfed from 80 to 50
static const int SPECIAL_IMPACT_FRAME = 15; // Frame where damage occurs
static const int SPECIAL_DURATION_FRAMES = 5; // How many frames the blast stays on screen after impact
static const int BOSS_MAX_HEALTH = 1000;

// =====================================================
// SAND ENEMY Sprites
// =====================================================
static const int SAND_STANCE_FRAMES = 8;
static unsigned int sSandStanceRight[SAND_STANCE_FRAMES];
static unsigned int sSandStanceLeft[SAND_STANCE_FRAMES];

static const int SAND_WALK_FRAMES = 6;
static unsigned int sSandWalkRight[SAND_WALK_FRAMES];
static unsigned int sSandWalkLeft[SAND_WALK_FRAMES];

// =====================================================
// DEIDARA Boss Sprites
// =====================================================
static const int DEI_STANCE_FRAMES = 4;
static unsigned int sDeiStanceRight[DEI_STANCE_FRAMES];
static unsigned int sDeiStanceLeft[DEI_STANCE_FRAMES];

static const int DEI_RUN_FRAMES = 6;
static unsigned int sDeiRunRight[DEI_RUN_FRAMES];
static unsigned int sDeiRunLeft[DEI_RUN_FRAMES];

static const int DEI_ATTACK1_FRAMES = 13;
static unsigned int sDeiAttack1Right[DEI_ATTACK1_FRAMES];
static unsigned int sDeiAttack1Left[DEI_ATTACK1_FRAMES];

static const int DEI_THROW_FRAMES = 4;
static unsigned int sDeiThrowRight[DEI_THROW_FRAMES];
static unsigned int sDeiThrowLeft[DEI_THROW_FRAMES];

static const int DEI_DAMAGE_FRAMES = 6;
static unsigned int sDeiDamageRight[DEI_DAMAGE_FRAMES];
static unsigned int sDeiDamageLeft[DEI_DAMAGE_FRAMES];

static const int DEI_DEATH_FRAMES = 5;
static unsigned int sDeiDeathRight[DEI_DEATH_FRAMES];
static unsigned int sDeiDeathLeft[DEI_DEATH_FRAMES];

// =====================================================
// CLAY BIRD Sprites
// =====================================================
static const int CLAY_BIRD_IDLE_FRAMES = 6;
static unsigned int sClayBirdIdleRight[CLAY_BIRD_IDLE_FRAMES];
static unsigned int sClayBirdIdleLeft[CLAY_BIRD_IDLE_FRAMES];

static const int CLAY_BIRD_ATTACK_FRAMES = 3;
static unsigned int sClayBirdAttackRight[CLAY_BIRD_ATTACK_FRAMES];
static unsigned int sClayBirdAttackLeft[CLAY_BIRD_ATTACK_FRAMES];

static const int CLAY_BIRD_MOVE_FRAMES = 6;
static unsigned int sClayBirdMoveRight[CLAY_BIRD_MOVE_FRAMES];
static unsigned int sClayBirdMoveLeft[CLAY_BIRD_MOVE_FRAMES];

// =====================================================
// CLAY CENTIPEDE Sprites
// =====================================================
static const int CLAY_CENT_IDLE_FRAMES = 3;
static unsigned int sClayCentIdleRight[CLAY_CENT_IDLE_FRAMES];
static unsigned int sClayCentIdleLeft[CLAY_CENT_IDLE_FRAMES];

static const int CLAY_CENT_ATTACK_FRAMES = 10;
static unsigned int sClayCentAttackRight[CLAY_CENT_ATTACK_FRAMES];
static unsigned int sClayCentAttackLeft[CLAY_CENT_ATTACK_FRAMES];

static const int CLAY_CENT_MOVE_FRAMES = 4;
static unsigned int sClayCentMoveRight[CLAY_CENT_MOVE_FRAMES];
static unsigned int sClayCentMoveLeft[CLAY_CENT_MOVE_FRAMES];

static const int CLAY_CENT_DAMAGE_FRAMES = 5;
static unsigned int sClayCentDamageRight[CLAY_CENT_DAMAGE_FRAMES];
static unsigned int sClayCentDamageLeft[CLAY_CENT_DAMAGE_FRAMES];

// =====================================================
// CLAY BALL Projectile Sprites
// =====================================================
static const int CLAY_BALL_FRAMES = 5;
static unsigned int sClayBallTex[CLAY_BALL_FRAMES];

static const int CLAY_HIT_FRAMES = 3;
static unsigned int sClayHitTex[CLAY_HIT_FRAMES];

// =====================================================
// Structures
// =====================================================
enum BossState {
  STATE_STANCE,
  STATE_RUN,
  STATE_WALK,
  STATE_ATTACK1,
  STATE_WEAPON_THROW,
  STATE_SPECIAL,      // Summon clay bird
  STATE_CENTIPEDE,     // Summon clay centipede
  STATE_DAMAGE,
  STATE_DEATH,
  STATE_BURROWING, // Sinking into sand
  STATE_HIDDEN,    // Moving underground
  STATE_EMERGING   // Rising from sand
};

struct Enemy {
  double x, y;
  double velX;
  int direction; // 0 = right, 1 = left
  int health;    // Max 100
  bool isAlive;
  int currentFrame;
  int animTimer;
  int enemyType;   // 0 = Sand Melee, 1 = Sand Fast, 2 = Boss (Deidara)
  BossState state;
  bool isAttacking;
  int attackFrame;
  int attackTimer;
  double width, height;
  double drawHeight; // For sinking/rising effect
  double targetX;    // Destination for hidden move
  int stateTimer;    // Multi-purpose timer for states
  int hideTimer;     // Counter for underground phase
  double velY;
  bool isOnGround;
  bool isJumping;
  int shockTimer; // Paralyzes enemy when > 0
  int summonCooldown; // Boss cooldown between summons
  int attackCooldown; // Cooldown between attacks
};

struct Bullet {
  double x, y;
  double velX;
  bool isActive;
  double radius;
};

struct EnemyBullet {
  double x, y;
  double velX, velY;
  bool isActive;
  double radius;
  double damage;
  int animFrame;
  int animTimer;
};

// Clay Creature (Bird or Centipede)
enum ClayType { CLAY_BIRD, CLAY_CENTIPEDE };
enum ClayState { CLAY_IDLE, CLAY_MOVING, CLAY_ATTACKING, CLAY_DAMAGE, CLAY_DEAD };

struct ClayCreature {
  double x, y;
  double velX, velY;
  int direction;
  bool isAlive;
  ClayType type;
  ClayState state;
  int currentFrame;
  int animTimer;
  int lifetime;     // Frames until despawn
  int attackTimer;   // Cooldown for attacks
  int health;
  double width, height;
};

// Hit Effect for clay ball explosions
struct HitEffect {
  double x, y;
  int frame;
  int timer;
  bool isActive;
};

struct Obstacle {
  double x, y;
  double width, height;
  unsigned int texture;
  bool isActive;
};

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

static const int MAX_CLAY_CREATURES = 4;
static ClayCreature sClayCreatures[MAX_CLAY_CREATURES];

static const int MAX_HIT_EFFECTS = 10;
static HitEffect sHitEffects[MAX_HIT_EFFECTS];

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

// Fuel and Special Combo System
static double sPlayerFuel = 100.0;
static bool sIsJDown = false;
static bool sIsSpecialCombo = false;
static int sSpecialComboFrame = 0;
static int sSpecialComboTimer = 0;
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

// Boss rage tracking
static bool sBossRageMode = false;

// Code Gate System (unlock Level 4)
static bool sCodeInputMode = false;
static char sCodeInputBuffer[32];
static int sCodeInputPos = 0;
static int sCodeInputStep = 0; // 0 = entering code 1, 1 = entering code 2
static bool sCodesVerified = false;
static bool sCodeError = false;
static int sCodeErrorTimer = 0;
static const char *sExpectedCode1 = "1DC 75UA";
static const char *sExpectedCode2 = "1DC T5UA";

// Story System
static bool sIsStoryPlaying = false;
static int sCurrentStoryGroup = -1; // 0: Start, 1: Phase 3, 2: End
static int sStorySubIndex = 0;
static int sStoryGroupStart[3] = {0, 1, 2};
static int sStoryGroupCount[3] = {1, 1, 2};
static unsigned int sStoryTextures[4]; // 1, 3, 4, 5.jpg

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
  sEnemies[index].health = (type == 2) ? BOSS_MAX_HEALTH : 80; // Standardized boss health
  sEnemies[index].isAlive = true;
  sEnemies[index].currentFrame = 0;
  sEnemies[index].animTimer = 0;
  sEnemies[index].enemyType = type;
  sEnemies[index].state = STATE_STANCE;
  sEnemies[index].isAttacking = false;
  sEnemies[index].attackFrame = 0;
  sEnemies[index].attackTimer = 0;
  if (type == 2) {
    sEnemies[index].width = 140;  // Exact match to Kakashi
    sEnemies[index].height = 140;
  } else {
    sEnemies[index].width = 80;   
    sEnemies[index].height = 80;
  }
  sEnemies[index].drawHeight = sEnemies[index].height;
  sEnemies[index].targetX = sEnemies[index].x;
  sEnemies[index].stateTimer = 0;
  sEnemies[index].hideTimer = 0;
  sEnemies[index].isOnGround = true;
  sEnemies[index].isJumping = false;
  sEnemies[index].shockTimer = 0;
  sEnemies[index].summonCooldown = 0;
  sEnemies[index].attackCooldown = 0;
  sEnemies[index].velY = 0;
}

static void spawnClayCreature(ClayType type, double spawnX, double spawnY, int dir) {
  for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
    if (!sClayCreatures[i].isAlive) {
      sClayCreatures[i].isAlive = true;
      sClayCreatures[i].type = type;
      sClayCreatures[i].state = CLAY_MOVING;
      sClayCreatures[i].x = spawnX;
      sClayCreatures[i].direction = dir;
      sClayCreatures[i].currentFrame = 0;
      sClayCreatures[i].animTimer = 0;
      sClayCreatures[i].attackTimer = 0;
      sClayCreatures[i].velX = 0;
      sClayCreatures[i].velY = 0;

      if (type == CLAY_BIRD) {
        sClayCreatures[i].y = spawnY + 200; // Start high up
        sClayCreatures[i].health = 60;
        sClayCreatures[i].lifetime = 420; // ~7 seconds
        sClayCreatures[i].width = 70;
        sClayCreatures[i].height = 50;
        sClayCreatures[i].velX = (dir == 0) ? 3.0 : -3.0;
      } else { // CENTIPEDE
        sClayCreatures[i].y = GROUND_LEVEL;
        sClayCreatures[i].health = 80;
        sClayCreatures[i].lifetime = 300; // ~5 seconds
        sClayCreatures[i].width = 90;
        sClayCreatures[i].height = 50;
        sClayCreatures[i].velX = (dir == 0) ? 5.0 : -5.0;
      }
      break;
    }
  }
}

static void spawnClayBall(double x, double y, double vx, double vy, double dmg) {
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!sEnemyBullets[i].isActive) {
      sEnemyBullets[i].isActive = true;
      sEnemyBullets[i].x = x;
      sEnemyBullets[i].y = y;
      sEnemyBullets[i].velX = vx;
      sEnemyBullets[i].velY = vy;
      sEnemyBullets[i].radius = 10;
      sEnemyBullets[i].damage = dmg;
      sEnemyBullets[i].animFrame = 0;
      sEnemyBullets[i].animTimer = 0;
      break;
    }
  }
}

static void spawnHitEffect(double x, double y) {
  for (int i = 0; i < MAX_HIT_EFFECTS; i++) {
    if (!sHitEffects[i].isActive) {
      sHitEffects[i].isActive = true;
      sHitEffects[i].x = x;
      sHitEffects[i].y = y;
      sHitEffects[i].frame = 0;
      sHitEffects[i].timer = 0;
      break;
    }
  }
}

void initLevel3(int screenW, int screenH) {
  sScreenW = screenW;
  sScreenH = screenH;

  // Guard: only load assets once
  if (sLevel3AssetsLoaded) return;

  // Load backgrounds for scrolling
  sBg1Tex = iLoadImage((char *)"Images/Level 3 Background/hehe1.png");
  sBg2Tex = iLoadImage((char *)"Images/Level 3 Background/hehe2.png");
  sBg3Tex = iLoadImage((char *)"Images/Level 3 Background/hehe3.png");
  sBgTex = sBg1Tex; // Fallback

  // Set flag that assets are loaded
  sLevel3AssetsLoaded = true;

  sKakashiHudTex = iLoadImage((char *)"Images/Sprites/Kakashi_hud.png");
  sPotionTex =
      iLoadImage((char *)"Images/Sprites/Collectibles/healing_potion.png");

  // Load Obstacle textures
  sObstacleCarTex =
      iLoadImage((char *)"Images/Sprites/Obstacles/broken_car1.png");
  sObstacleTrashTex = iLoadImage(
      (char *)"Images/Sprites/Obstacles/trash_can.png");

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

  // Load Kakashi Special Attack frames
  for (int i = 0; i < SPECIAL_ATTACK_FRAME_COUNT; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Special Attack 1 Right/SpecialAttack_Right_%d.png",
              i + 1);
    sKakashiSpecialRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Hatake Kakashi/Special Attack 1 Left/SpecialAttack_Left_%d.png",
              i + 1);
    sKakashiSpecialLeft[i] = iLoadImage(path);
  }

  sKunaiRightTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_right.png");
  sKunaiLeftTex =
      iLoadImage((char *)"Images/Sprites/Hatake Kakashi/kunai_left.png");

  // =====================================================
  // LOAD SAND ENEMY SPRITES
  // =====================================================
  // Sand Enemy Stance Right: Right_0000_sprite_5.png through Right_0007_sprite_3.png
  {
    int stanceSprites[] = {5, 6, 7, 8, 4, 1, 2, 3};
    for (int i = 0; i < SAND_STANCE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Sand Enemy/Stance Right/Right_%04d_sprite_%d.png", i, stanceSprites[i]);
      sSandStanceRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Sand Enemy/Stance Left/Left_%04d_sprite_%d.png", i, stanceSprites[i]);
      sSandStanceLeft[i] = iLoadImage(path);
    }
  }

  // Sand Enemy Walk: WalkRight_0000_sprite_1.png through WalkRight_0005_sprite_4.png
  {
    int walkSprites[] = {1, 2, 3, 5, 6, 4};
    for (int i = 0; i < SAND_WALK_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Sand Enemy/Walk Right/WalkRight_%04d_sprite_%d.png", i, walkSprites[i]);
      sSandWalkRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Sand Enemy/Walk Left/WalkLeft_%04d_sprite_%d.png", i, walkSprites[i]);
      sSandWalkLeft[i] = iLoadImage(path);
    }
  }

  // =====================================================
  // LOAD DEIDARA SPRITES
  // =====================================================
  // Deidara Stance Right: StanceRight_0004_sprite_7.png through StanceRight_0007_sprite_4.png
  {
    int stanceRightSprites[] = {7, 3, 1, 4};
    int stanceRightIdx[] = {4, 5, 6, 7};
    for (int i = 0; i < DEI_STANCE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Stance Right/StanceRight_%04d_sprite_%d.png", stanceRightIdx[i], stanceRightSprites[i]);
      sDeiStanceRight[i] = iLoadImage(path);
    }
    // Stance Left: Deidara_0000_sprite_8.png through Deidara_0003_sprite_6.png
    int stanceLeftSprites[] = {8, 5, 2, 6};
    for (int i = 0; i < DEI_STANCE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Stance Left/Deidara_%04d_sprite_%d.png", i, stanceLeftSprites[i]);
      sDeiStanceLeft[i] = iLoadImage(path);
    }
  }

  // Deidara Run Right: RunRight_0006..0011
  {
    int runRightSprites[] = {5, 1, 6, 2, 7, 3};
    int runRightIdx[] = {6, 7, 8, 9, 10, 11};
    for (int i = 0; i < DEI_RUN_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Run Right/RunRight_%04d_sprite_%d.png", runRightIdx[i], runRightSprites[i]);
      sDeiRunRight[i] = iLoadImage(path);
    }
    // Run Left: RunLeft_0000..0005
    int runLeftSprites[] = {12, 8, 10, 4, 9, 11};
    for (int i = 0; i < DEI_RUN_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Run Left/RunLeft_%04d_sprite_%d.png", i, runLeftSprites[i]);
      sDeiRunLeft[i] = iLoadImage(path);
    }
  }

  // Deidara Attack 1: Attack_1Right_0000_Layer-2.png through Attack_1Right_0012_Layer-14.png
  for (int i = 0; i < DEI_ATTACK1_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Deidara/Attack 1 Right/Attack_1Right_%04d_Layer-%d.png", i, i + 2);
    sDeiAttack1Right[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Deidara/Attack 1 Left/Attack_1Left_%04d_Layer-%d.png", i, i + 2);
    sDeiAttack1Left[i] = iLoadImage(path);
  }

  // Deidara Weapon Throw Right: WeaponThrowRight_0004..0007
  {
    int throwRIdx[] = {4, 5, 6, 7};
    int throwRSprites[] = {1, 3, 5, 6};
    for (int i = 0; i < DEI_THROW_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Weapon Throw RIght/WeaponThrowRight_%04d_sprite_%d.png", throwRIdx[i], throwRSprites[i]);
      sDeiThrowRight[i] = iLoadImage(path);
    }
    int throwLSprites[] = {2, 4, 7, 8};
    for (int i = 0; i < DEI_THROW_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Weapon Throw Left/WeaponThrowLeft_%04d_sprite_%d.png", i, throwLSprites[i]);
      sDeiThrowLeft[i] = iLoadImage(path);
    }
  }

  // Deidara Damage Right: DamageRight_0006..0011
  {
    int dmgRIdx[] = {6, 7, 8, 9, 10, 11};
    int dmgRSprites[] = {2, 1, 3, 9, 11, 7};
    for (int i = 0; i < DEI_DAMAGE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Damage Right/DamageRight_%04d_sprite_%d.png", dmgRIdx[i], dmgRSprites[i]);
      sDeiDamageRight[i] = iLoadImage(path);
    }
    int dmgLSprites[] = {5, 4, 6, 10, 12, 8};
    for (int i = 0; i < DEI_DAMAGE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Damage Left/DamageLeft_%04d_sprite_%d.png", i, dmgLSprites[i]);
      sDeiDamageLeft[i] = iLoadImage(path);
    }
  }

  // Deidara Death: reuse first 5 frames of damage (death animation)
  {
    int deathRIdx[] = {6, 7, 8, 9, 10};
    int deathRSprites[] = {2, 1, 3, 9, 11};
    for (int i = 0; i < DEI_DEATH_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Death Right/DamageRight_%04d_sprite_%d.png", deathRIdx[i], deathRSprites[i]);
      sDeiDeathRight[i] = iLoadImage(path);
    }
    int deathLSprites[] = {5, 4, 6, 10, 12};
    for (int i = 0; i < DEI_DEATH_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/Death Left/DamageLeft_%04d_sprite_%d.png", i, deathLSprites[i]);
      sDeiDeathLeft[i] = iLoadImage(path);
    }
  }

  // =====================================================
  // LOAD CLAY BIRD SPRITES
  // =====================================================
  {
    int idleSprites[] = {2, 1, 3, 5, 6, 4};
    for (int i = 0; i < CLAY_BIRD_IDLE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Idle Right/Idle_%04d_sprite_%d.png", i, idleSprites[i]);
      sClayBirdIdleRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Idle Left/Idle_%04d_sprite_%d.png", i, idleSprites[i]);
      sClayBirdIdleLeft[i] = iLoadImage(path);
    }
  }
  {
    int atkSprites[] = {13, 14, 15};
    for (int i = 0; i < CLAY_BIRD_ATTACK_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Attacking Right/Attacking_%04d_sprite_%d.png", i, atkSprites[i]);
      sClayBirdAttackRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Attacking Left/Attacking_%04d_sprite_%d.png", i, atkSprites[i]);
      sClayBirdAttackLeft[i] = iLoadImage(path);
    }
  }
  {
    int moveRSprites[] = {8, 7, 9, 10, 11, 12};
    for (int i = 0; i < CLAY_BIRD_MOVE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Moving Right/Moving_%04d_sprite_%d.png", i, moveRSprites[i]);
      sClayBirdMoveRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Deidara/ClayBird/Moving Left/Moving_%04d_sprite_%d.png", i, moveRSprites[i]);
      sClayBirdMoveLeft[i] = iLoadImage(path);
    }
  }

  // =====================================================
  // LOAD CLAY CENTIPEDE SPRITES
  // =====================================================
  for (int i = 0; i < CLAY_CENT_IDLE_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Idle Right/Clay-Centipede_%04d_Layer-%d.png", i, i + 1);
    sClayCentIdleRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Idle Left/Idle_%04d_Layer-%d.png", i, i + 1);
    sClayCentIdleLeft[i] = iLoadImage(path);
  }
  {
    int centAtkIdxR[] = {1, 2, 4, 5, 6, 7, 8, 9, 10, 11};
    int centAtkLayer[] = {4, 5, 7, 8, 9, 10, 11, 12, 13, 14};
    for (int i = 0; i < CLAY_CENT_ATTACK_FRAMES; i++) {
      char path[256];
      // RIght uses indices 1, 2, 4, 5...
      sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Attack RIght/Attack_%04d_Layer-%d.png", centAtkIdxR[i], centAtkLayer[i]);
      sClayCentAttackRight[i] = iLoadImage(path);
      // Left uses continuous indices 1 through 10
      sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Attack Left/Attack_%04d_Layer-%d.png", i + 1, centAtkLayer[i]);
      sClayCentAttackLeft[i] = iLoadImage(path);
    }
  }
  for (int i = 0; i < CLAY_CENT_MOVE_FRAMES; i++) {
    char path[256];
    sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Moving Right/Moving_%04d_Layer-%d.png", i, i + 1);
    sClayCentMoveRight[i] = iLoadImage(path);
    sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Moving Left/Moving_%04d_Layer-%d.png", i, i + 1);
    sClayCentMoveLeft[i] = iLoadImage(path);
  }
  {
    int centDmgIdx[] = {1, 2, 3, 4, 5};
    int centDmgLayer[] = {12, 13, 14, 15, 11};
    for (int i = 0; i < CLAY_CENT_DAMAGE_FRAMES; i++) {
      char path[256];
      sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Damage Right/Damage_%04d_Layer-%d.png", centDmgIdx[i], centDmgLayer[i]);
      sClayCentDamageRight[i] = iLoadImage(path);
      sprintf_s(path, "Images/Sprites/Deidara/ClayCentipede/Damage Left/Damage_%04d_Layer-%d.png", centDmgIdx[i], centDmgLayer[i]);
      sClayCentDamageLeft[i] = iLoadImage(path);
    }
  }

  // =====================================================
  // LOAD CLAY BALL & HIT EFFECT
  // =====================================================
  sClayBallTex[0] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Layer 6.png");
  sClayBallTex[1] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Layer 7.png");
  sClayBallTex[2] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Layer 8.png");
  sClayBallTex[3] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Layer 9.png");
  sClayBallTex[4] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Layer 10.png");

  sClayHitTex[0] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Hit Effect/Layer 3.png");
  sClayHitTex[1] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Hit Effect/Layer 4.png");
  sClayHitTex[2] = iLoadImage((char *)"Images/Sprites/Deidara/ClayBird/ClayBalls/Hit Effect/Layer 5.png");

  // Load Level 3 Stories
  sStoryTextures[0] = iLoadImage((char *)"Images/Level_3 Stories/1.jpg");
  sStoryTextures[1] = iLoadImage((char *)"Images/Level_3 Stories/3.jpg");
  sStoryTextures[2] = iLoadImage((char *)"Images/Level_3 Stories/4.jpg");
  sStoryTextures[3] = iLoadImage((char *)"Images/Level_3 Stories/5.jpg");

  sLevel3AssetsLoaded = true;
}

// Game Over State
static bool sIsGameOver = false;

bool isLevel3GameOver() { return sIsGameOver; }
bool isLevel3GameWon() { return sCurrentPhase == BOSS_DEFEATED; }

void resetLevel3() {
  // Lazy-load assets if not loaded yet
  if (!sLevel3AssetsLoaded) {
    initLevel3(sScreenW > 0 ? sScreenW : 1080, sScreenH > 0 ? sScreenH : 635);
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
  sWinTimer = 0;
  sBossRageMode = false;

  triggerStory(0); // Level 3 Start Story (1)

  // Reset code gate
  sCodeInputMode = false;
  memset(sCodeInputBuffer, 0, sizeof(sCodeInputBuffer));
  sCodeInputPos = 0;
  sCodeInputStep = 0;
  sCodesVerified = false;
  sCodeError = false;
  sCodeErrorTimer = 0;

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

  sPlayerFuel = 100.0;
  sIsJDown = false;
  sIsSpecialCombo = false;
  sSpecialComboFrame = 0;
  sSpecialComboTimer = 0;

  // Reset Phase
  sCurrentPhase = PHASE_1;
  sPhaseInitiated = true;
  sPhaseEnemiesSpawned = 3; // Pre-spawned below
  sPhaseEnemiesKilled = 0;

  // Reset Enemies for Phase 1 - Sand Enemies
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

  // Reset Clay Creatures
  for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
    sClayCreatures[i].isAlive = false;
  }

  // Reset Hit Effects
  for (int i = 0; i < MAX_HIT_EFFECTS; i++) {
    sHitEffects[i].isActive = false;
  }

  // Reset Obstacles - Level 3 has NO obstacles (flat combat zone)
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    sObstacles[i].isActive = false;
  }
  sLevel3AssetsLoaded = true;
}

void drawLevel3(int screenW, int screenH);
void updateLevel3();

void drawLevel3(int screenW, int screenH) {

  iClear();

  // Draw background based on camera position
  // Segmented Background: hehe1, hehe2, hehe3
  unsigned int segTextures[] = { sBg1Tex, sBg2Tex, sBg3Tex };
  for (int i = 0; i < 3; i++) {
    double segX = i * screenW - sCameraX;
    if (segX > -screenW && segX < screenW) {
      if (segTextures[i]) {
        iShowImage((int)segX, 0, screenW, screenH, segTextures[i]);
      } else {
        // Fallback if texture failed to load
        iShowImage((int)segX, 0, screenW, screenH, sBgTex);
      }
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
  } else if (sIsSpecialCombo) {
    tex = (sDirection == 0) ? sKakashiSpecialRight[sSpecialComboFrame]
                            : sKakashiSpecialLeft[sSpecialComboFrame];
    kakashiW = sPlayerW + 60; // Chidori has a massive visual reach
    if (sDirection == 1) {
      offsetX = -60; // Offset Left when facing left
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

  // =====================================================
  // Draw Clay Creatures (behind enemies for layering)
  // =====================================================
  for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
    if (sClayCreatures[i].isAlive) {
      unsigned int clayTex = 0;
      int cw = (int)sClayCreatures[i].width;
      int ch = (int)sClayCreatures[i].height;
      int dir = sClayCreatures[i].direction;
      int frame = sClayCreatures[i].currentFrame;

      if (sClayCreatures[i].type == CLAY_BIRD) {
        switch (sClayCreatures[i].state) {
        case CLAY_IDLE:
          clayTex = (dir == 0) ? sClayBirdIdleRight[frame % CLAY_BIRD_IDLE_FRAMES]
                               : sClayBirdIdleLeft[frame % CLAY_BIRD_IDLE_FRAMES];
          break;
        case CLAY_MOVING:
          clayTex = (dir == 0) ? sClayBirdMoveRight[frame % CLAY_BIRD_MOVE_FRAMES]
                               : sClayBirdMoveLeft[frame % CLAY_BIRD_MOVE_FRAMES];
          break;
        case CLAY_ATTACKING:
          clayTex = (dir == 0) ? sClayBirdAttackRight[frame % CLAY_BIRD_ATTACK_FRAMES]
                               : sClayBirdAttackLeft[frame % CLAY_BIRD_ATTACK_FRAMES];
          break;
        default:
          clayTex = (dir == 0) ? sClayBirdIdleRight[0] : sClayBirdIdleLeft[0];
          break;
        }
      } else { // CENTIPEDE
        switch (sClayCreatures[i].state) {
        case CLAY_IDLE:
          clayTex = (dir == 0) ? sClayCentIdleRight[frame % CLAY_CENT_IDLE_FRAMES]
                               : sClayCentIdleLeft[frame % CLAY_CENT_IDLE_FRAMES];
          break;
        case CLAY_MOVING:
          clayTex = (dir == 0) ? sClayCentMoveRight[frame % CLAY_CENT_MOVE_FRAMES]
                               : sClayCentMoveLeft[frame % CLAY_CENT_MOVE_FRAMES];
          break;
        case CLAY_ATTACKING:
          clayTex = (dir == 0) ? sClayCentAttackRight[frame % CLAY_CENT_ATTACK_FRAMES]
                               : sClayCentAttackLeft[frame % CLAY_CENT_ATTACK_FRAMES];
          break;
        case CLAY_DAMAGE:
          clayTex = (dir == 0) ? sClayCentDamageRight[frame % CLAY_CENT_DAMAGE_FRAMES]
                               : sClayCentDamageLeft[frame % CLAY_CENT_DAMAGE_FRAMES];
          break;
        default:
          clayTex = (dir == 0) ? sClayCentIdleRight[0] : sClayCentIdleLeft[0];
          break;
        }
      }

      iShowImage((int)(sClayCreatures[i].x - sCameraX), (int)sClayCreatures[i].y,
                 cw, ch, clayTex);

      // Draw creature health bar
      if (sClayCreatures[i].health > 0) {
        double maxHp = (sClayCreatures[i].type == CLAY_BIRD) ? 60.0 : 80.0;
        iSetColor(80, 0, 0);
        iFilledRectangle(sClayCreatures[i].x - sCameraX, sClayCreatures[i].y + ch + 3, 40, 4);
        iSetColor(255, 180, 0); // Orange health bar for clay creatures
        iFilledRectangle(sClayCreatures[i].x - sCameraX, sClayCreatures[i].y + ch + 3,
                         40 * (sClayCreatures[i].health / maxHp), 4);
      }
    }
  }

  // =====================================================
  // Draw Enemies (relative to camera)
  // =====================================================
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive && sEnemies[i].state != STATE_HIDDEN) {
      unsigned int enemyTex = 0;
      int renderW = (int)sEnemies[i].width;
      int renderH = (int)sEnemies[i].drawHeight; // Pulse size for burrowing
      int offsetX = 0;
      int offsetY = (int)(sEnemies[i].height - sEnemies[i].drawHeight); // Keeps feet on ground as they sink

      if (sEnemies[i].enemyType == 2) {
        // ========================
        // DEIDARA BOSS RENDERING
        // ========================
        int frame = sEnemies[i].currentFrame;
        int dir = sEnemies[i].direction;
        switch (sEnemies[i].state) {
        case STATE_STANCE:
          enemyTex = (dir == 0) ? sDeiStanceRight[frame % DEI_STANCE_FRAMES]
                                : sDeiStanceLeft[frame % DEI_STANCE_FRAMES];
          break;
        case STATE_RUN:
        case STATE_WALK:
          enemyTex = (dir == 0) ? sDeiRunRight[frame % DEI_RUN_FRAMES]
                                : sDeiRunLeft[frame % DEI_RUN_FRAMES];
          break;
        case STATE_ATTACK1:
          enemyTex = (dir == 0) ? sDeiAttack1Right[sEnemies[i].attackFrame % DEI_ATTACK1_FRAMES]
                                : sDeiAttack1Left[sEnemies[i].attackFrame % DEI_ATTACK1_FRAMES];
          renderW += 30; // Attack combo is wider
          if (dir == 1) offsetX = -30;
          break;
        case STATE_WEAPON_THROW:
          enemyTex = (dir == 0) ? sDeiThrowRight[sEnemies[i].attackFrame % DEI_THROW_FRAMES]
                                : sDeiThrowLeft[sEnemies[i].attackFrame % DEI_THROW_FRAMES];
          break;
        case STATE_SPECIAL:
        case STATE_CENTIPEDE:
          // Summon animation uses weapon throw sprites with glow effect
          enemyTex = (dir == 0) ? sDeiThrowRight[sEnemies[i].attackFrame % DEI_THROW_FRAMES]
                                : sDeiThrowLeft[sEnemies[i].attackFrame % DEI_THROW_FRAMES];
          break;
        case STATE_DAMAGE:
          enemyTex = (dir == 0) ? sDeiDamageRight[frame % DEI_DAMAGE_FRAMES]
                                : sDeiDamageLeft[frame % DEI_DAMAGE_FRAMES];
          break;
        case STATE_DEATH:
          enemyTex = (dir == 0) ? sDeiDeathRight[frame % DEI_DEATH_FRAMES]
                                : sDeiDeathLeft[frame % DEI_DEATH_FRAMES];
          break;
        default:
          enemyTex = (dir == 0) ? sDeiStanceRight[0] : sDeiStanceLeft[0];
          break;
        }

        // Rage mode visual: small pulsing spark particles (no box)
        if (sBossRageMode && sEnemies[i].state != STATE_DEATH && sEnemies[i].state != STATE_DAMAGE) {
          iSetColor(255, 120, 0);
          for (int p = 0; p < 3; p++) {
            int px = (int)(sEnemies[i].x - sCameraX) + (rand() % renderW);
            int py = (int)sEnemies[i].y + renderH + 5 + (rand() % 15);
            iFilledCircle(px, py, 1 + (rand() % 2));
          }
        }

      } else {
        // ========================
        // SAND ENEMY RENDERING
        // ========================
        int frame = sEnemies[i].currentFrame;
        int dir = sEnemies[i].direction;

        if (sEnemies[i].isAttacking) {
          // Use stance frames 4-7 for attack windup visual
          int atkFrame = 4 + (sEnemies[i].attackFrame % 4);
          enemyTex = (dir == 0) ? sSandStanceRight[atkFrame % SAND_STANCE_FRAMES]
                                : sSandStanceLeft[atkFrame % SAND_STANCE_FRAMES];
        } else if (sEnemies[i].state == STATE_DAMAGE) {
          // Flash during damage — use stance frame 0 with color overlay
          enemyTex = (dir == 0) ? sSandStanceRight[frame % SAND_STANCE_FRAMES]
                                : sSandStanceLeft[frame % SAND_STANCE_FRAMES];
        } else if (fabs(sEnemies[i].velX) > 0.1 || sEnemies[i].state == STATE_WALK || sEnemies[i].state == STATE_RUN) {
          enemyTex = (dir == 0) ? sSandWalkRight[frame % SAND_WALK_FRAMES]
                                : sSandWalkLeft[frame % SAND_WALK_FRAMES];
        } else {
          enemyTex = (dir == 0) ? sSandStanceRight[frame % SAND_STANCE_FRAMES]
                                : sSandStanceLeft[frame % SAND_STANCE_FRAMES];
        }

        // Sand Particles for burrowing/emerging
        if (sEnemies[i].state == STATE_BURROWING || sEnemies[i].state == STATE_EMERGING) {
            iSetColor(220, 180, 100); // Sand color
            for (int p = 0; p < 5; p++) {
                double px = (sEnemies[i].x - sCameraX) + (rand() % (int)sEnemies[i].width);
                double py = sEnemies[i].y + (rand() % 20);
                iFilledCircle(px, py, 1 + (rand() % 3));
            }
        }
      }

      // Chidori impact: small spark particles only (no box overlay)
      if (sIsSpecialCombo && sSpecialComboFrame >= SPECIAL_IMPACT_FRAME && sSpecialComboFrame <= SPECIAL_IMPACT_FRAME + 5 && sEnemies[i].state == STATE_DAMAGE) {
          iSetColor(200, 240, 255);
          for (int p = 0; p < 3; p++) {
            int px = (int)(sEnemies[i].x - sCameraX) + (rand() % renderW);
            int py = (int)sEnemies[i].y + (rand() % renderH);
            iFilledCircle(px, py, 2 + (rand() % 3));
          }
      }
      
      // Shock Visual Effect for paralyzed enemies (Electro-stun crackle)
      if (sEnemies[i].shockTimer > 0) {
          iSetColor(180, 240, 255);
          for(int s=0; s<4; s++) {
             int sx = (int)(sEnemies[i].x - sCameraX) + (rand() % renderW);
             int sy = (int)sEnemies[i].y + (rand() % (renderH + 20)) - 10;
             int sw = 5 + (rand() % 15);
             int sh = 2 + (rand() % 4);
             iFilledRectangle(sx, sy, sw, sh); // Horizontal sparks
             iFilledRectangle(sx + sw/2 - 1, sy - sw/2, sh, sw); // Vertical cross sparks
          }
      }

      // Damage visual: no box, sprites only

      if (enemyTex > 0 && renderH > 0) {
        iShowImage((int)(sEnemies[i].x - sCameraX) + offsetX, (int)sEnemies[i].y + offsetY, renderW,
                   renderH, enemyTex);
      }

      // Draw Health Bar (only if not hidden and mostly visible)
      if (sEnemies[i].drawHeight > 20) {
          iSetColor(255, 0, 0);
          double barWidth = (sEnemies[i].enemyType == 2) ? 100.0 : 60.0;
          iFilledRectangle(sEnemies[i].x - sCameraX,
                           sEnemies[i].y + sEnemies[i].height + 5, barWidth, 4);
          if (sEnemies[i].enemyType == 2) {
            iSetColor(255, 100, 0); // Orange for Deidara
          } else {
            iSetColor(0, 255, 0);
          }
          double maxHp = (sEnemies[i].enemyType == 2) ? (double)BOSS_MAX_HEALTH : 100.0;
          iFilledRectangle(sEnemies[i].x - sCameraX,
                           sEnemies[i].y + sEnemies[i].height + 5,
                           barWidth * (sEnemies[i].health / maxHp), 4);
      }

      // Boss name tag
      if (sEnemies[i].enemyType == 2 && sEnemies[i].drawHeight > 50) {
        iSetColor(255, 200, 50);
        iText(sEnemies[i].x - sCameraX, sEnemies[i].y + sEnemies[i].height + 14, (char *)"DEIDARA");
      }
    }
  }

  // =====================================================
  // Draw Bullets (relative to camera)
  // =====================================================
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      unsigned int kunaiTex =
          (sBullets[i].velX > 0) ? sKunaiRightTex : sKunaiLeftTex;
      iShowImage((int)(sBullets[i].x - sCameraX - 15), (int)(sBullets[i].y - 5),
                 30, 10, kunaiTex);
    }
  }

  // Draw Enemy Bullets (Clay Balls)
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (sEnemyBullets[i].isActive) {
      int cbFrame = sEnemyBullets[i].animFrame % CLAY_BALL_FRAMES;
      if (sClayBallTex[cbFrame]) {
        iShowImage((int)(sEnemyBullets[i].x - sCameraX - 12),
                   (int)(sEnemyBullets[i].y - 12), 24, 24, sClayBallTex[cbFrame]);
      } else {
        iSetColor(200, 150, 50); // Tan/clay color fallback
        iFilledCircle(sEnemyBullets[i].x - sCameraX, sEnemyBullets[i].y,
                      sEnemyBullets[i].radius);
      }
    }
  }

  // Draw Hit Effects (Clay Explosions)
  for (int i = 0; i < MAX_HIT_EFFECTS; i++) {
    if (sHitEffects[i].isActive) {
      int hf = sHitEffects[i].frame % CLAY_HIT_FRAMES;
      if (sClayHitTex[hf]) {
        iShowImage((int)(sHitEffects[i].x - sCameraX - 20),
                   (int)(sHitEffects[i].y - 20), 40, 40, sClayHitTex[hf]);
      }
      // Orange explosion particles
      iSetColor(255, 150, 0);
      for (int p = 0; p < 4; p++) {
        int px = (int)(sHitEffects[i].x - sCameraX) + (rand() % 30) - 15;
        int py = (int)sHitEffects[i].y + (rand() % 30) - 15;
        iFilledCircle(px, py, 2 + (rand() % 4));
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

  // Draw Fuel Bar
  iSetColor(0, 50, 100);
  iFilledRectangle(70, screenH - 65, 200, 10);
  iSetColor(0, 200, 255); // Cyan color for fuel
  iFilledRectangle(70, screenH - 65, 200 * (sPlayerFuel / 100.0), 10);
  iSetColor(255, 255, 255);
  iText(70, screenH - 77, (char *)"Fuel");

  char livesStr[20];
  sprintf_s(livesStr, "Lives: %d", sPlayerLives);
  iText(70, screenH - 75, livesStr);
  char scoreStr[20];
  sprintf_s(scoreStr, "Score: %d", sScore);
  iSetColor(0, 255, 0); // Green for visibility
  iText(70, screenH - 95, scoreStr);

  // Chidori Visual (More electric/jagged aesthetic)
  if (sIsSpecialCombo) {
      if (sSpecialComboFrame >= SPECIAL_IMPACT_FRAME && sSpecialComboFrame <= SPECIAL_IMPACT_FRAME + SPECIAL_DURATION_FRAMES) {
          int blastLen = SPECIAL_BLAST_LENGTH;
          int xStart = (sDirection == 0) ? (int)(sPlayerX - sCameraX + sPlayerW/2 + 20) : (int)(sPlayerX - sCameraX + sPlayerW/2 - 20 - blastLen);
          int yMid = (int)sPlayerY + sPlayerH/2 - 10;
          
          // Draw multiple jagged paths instead of a solid block
          for (int layer = 0; layer < 4; layer++) {
              if (layer == 0) iSetColor(255, 255, 255); // White inner
              else if (layer == 1) iSetColor(0, 255, 255); // Cyan
              else if (layer == 2) iSetColor(0, 100, 255); // Blue
              else iSetColor(150, 50, 255); // Purple hint
              
              int segmentW = 50;
              int lastY = yMid;
              for (int x = 0; x < blastLen; x += segmentW) {
                  int thickness = 15 - (layer * 3);
                  if (thickness < 3) thickness = 3;
                  
                  iFilledRectangle(xStart + x, lastY - thickness/2, segmentW + 2, thickness);
                  lastY += (rand() % 30) - 15; // Jitter the path
              }
          }
          
          // Extra chaotic sparks around the source
          iSetColor(255, 255, 255);
          for (int p = 0; p < 8; p++) {
              int px = xStart + (rand() % 60) - 30;
              int py = yMid + (rand() % 60) - 30;
              iFilledCircle(px, py, 3 + (rand() % 4));
          }
      }
  }

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
  else if (sCurrentPhase == PHASE_3) {
    if (sBossRageMode)
      sprintf_s(phaseStr, "DEIDARA - RAGE!");
    else
      sprintf_s(phaseStr, "BOSS: DEIDARA");
  }
  else
    sprintf_s(phaseStr, "ARENA CLEAR");
  iText(screenW - 180, screenH - 30, phaseStr);

  // Boss HP Bar at top of screen (Phase 3 only)
  if (sCurrentPhase == PHASE_3 && sEnemies[0].isAlive) {
    int bossBarW = 400;
    int bossBarH = 12;
    int bossBarX = (screenW - bossBarW) / 2;
    int bossBarY = screenH - 55;

    // Background
    iSetColor(40, 20, 20);
    iFilledRectangle(bossBarX - 2, bossBarY - 2, bossBarW + 4, bossBarH + 4);

    // Red empty
    iSetColor(100, 0, 0);
    iFilledRectangle(bossBarX, bossBarY, bossBarW, bossBarH);

    // Orange fill
    double hpRatio = sEnemies[0].health / (double)BOSS_MAX_HEALTH;
    if (hpRatio < 0) hpRatio = 0;
    if (sBossRageMode) {
      iSetColor(255, 50, 0); // Red when raging
    } else {
      iSetColor(255, 140, 0); // Orange normally
    }
    iFilledRectangle(bossBarX, bossBarY, bossBarW * hpRatio, bossBarH);

    // Boss name
    iSetColor(255, 220, 100);
    iText(bossBarX, bossBarY + bossBarH + 4, (char *)"DEIDARA - The Artist");
  }

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
      iText(screenW / 2.0 - 130.0, screenH / 2.0 + 10.0, (char *)"TRUE ART IS AN EXPLOSION!", (void *)2);
      iSetColor(200, 200, 200); // Silver subtext
      iText(screenW / 2.0 - 110.0, screenH / 2.0 - 20.0, (char *)"The sky is finally clear...", (void *)2);
    } else if (!sCodeInputMode && !sCodesVerified) {
      iSetColor(0, 255, 255); // Cyan
      iText(screenW / 2.0 - 120.0, screenH / 2.0 + 10.0, (char *)"SECRET CODE UNLOCKED:", (void *)2);
      iSetColor(255, 215, 0); // Gold
      iText(screenW / 2.0 - 60.0, screenH / 2.0 - 20.0, (char *)"1AC T5VB", (void *)2);
      iSetColor(200, 200, 200);
      iText(screenW / 2.0 - 100.0, screenH / 2.0 - 45.0, (char *)"Press ENTER to unlock Level 4");
    } else if (sCodeInputMode && !sCodesVerified) {
      // Code input UI
      // Dark overlay
      iSetColor(0, 0, 0);
      iFilledRectangle(screenW / 2.0 - 200, screenH / 2.0 - 80, 400, 160);
      iSetColor(218, 165, 32);
      iRectangle(screenW / 2.0 - 200, screenH / 2.0 - 80, 400, 160);
      iRectangle(screenW / 2.0 - 199, screenH / 2.0 - 79, 398, 158);

      if (sCodeInputStep == 0) {
        iSetColor(0, 255, 255);
        iText(screenW / 2.0 - 130, screenH / 2.0 + 40, (char *)"ENTER CODE FROM LEVEL 1:", (void *)2);
      } else {
        iSetColor(50, 255, 50);
        iText(screenW / 2.0 - 80, screenH / 2.0 + 55, (char *)"CODE 1: CORRECT!", (void *)2);
        iSetColor(0, 255, 255);
        iText(screenW / 2.0 - 130, screenH / 2.0 + 30, (char *)"ENTER CODE FROM LEVEL 2:", (void *)2);
      }

      // Input field
      iSetColor(30, 30, 30);
      iFilledRectangle(screenW / 2.0 - 100, screenH / 2.0 - 10, 200, 30);
      iSetColor(255, 255, 255);
      iRectangle(screenW / 2.0 - 100, screenH / 2.0 - 10, 200, 30);
      iSetColor(255, 255, 255);
      iText(screenW / 2.0 - 90, screenH / 2.0 - 2, sCodeInputBuffer, (void *)2);

      // Cursor blink
      if ((sElapsedFrames / 20) % 2 == 0) {
        double cursorX = screenW / 2.0 - 90 + sCodeInputPos * 12;
        iSetColor(255, 255, 255);
        iFilledRectangle(cursorX, screenH / 2.0 - 8, 2, 24);
      }

      // Error message
      if (sCodeError) {
        iSetColor(255, 0, 0);
        iText(screenW / 2.0 - 60, screenH / 2.0 - 50, (char *)"WRONG CODE!", (void *)2);
      }

      iSetColor(150, 150, 150);
      iText(screenW / 2.0 - 110, screenH / 2.0 - 70, (char *)"Type code and press ENTER");
    } else if (sCodesVerified) {
      iSetColor(50, 255, 50);
      iText(screenW / 2.0 - 120, screenH / 2.0 + 10, (char *)"LEVEL 4 UNLOCKED!", (void *)2);
      iSetColor(255, 215, 0);
      iText(screenW / 2.0 - 130, screenH / 2.0 - 20, (char *)"Transitioning to final arena...", (void *)2);
    }
  }

  // Render Story
  if (sIsStoryPlaying && sCurrentStoryGroup != -1) {
    int texIdx = sStoryGroupStart[sCurrentStoryGroup] + sStorySubIndex;
    if (texIdx < 4 && sStoryTextures[texIdx]) {
      iShowImage(0, 0, screenW, screenH, sStoryTextures[texIdx]);

      // Story overlay prompt
      iSetColor(0, 0, 0);
      iFilledRectangle(screenW / 2.0 - 150, 20, 300, 30);
      iSetColor(255, 255, 255);
      iRectangle(screenW / 2.0 - 150, 20, 300, 30);
      iText(screenW / 2.0 - 100, 30, (char *)"Press SPACE to continue", (void *)1);
    }
  }
}

void updateLevel3() {
  if (sIsStoryPlaying)
    return; // Pause game while story is playing

  if (sIsGameOver || sIsDying)
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

  // Fuel Refill
  if (sPlayerFuel < 100.0) {
    sPlayerFuel += 0.05; // Refill slowly
    if (sPlayerFuel > 100.0) sPlayerFuel = 100.0;
  }

  // Handle Combo Chidori Attack
  if (sIsSpecialCombo) {
     sSpecialComboTimer++;
     if (sSpecialComboTimer >= 2) { // 2 ticks per animation frame -> 60 frames runtime (1s)
         sSpecialComboTimer = 0;
         
         if (sSpecialComboFrame == SPECIAL_IMPACT_FRAME) { // Impact frame
             int blastLen = SPECIAL_BLAST_LENGTH;
             // Damage enemies
             for (int i = 0; i < MAX_ENEMIES; i++) {
                 if (sEnemies[i].isAlive && sEnemies[i].state != STATE_HIDDEN && sEnemies[i].drawHeight > 20) {
                     bool inBlast = false;
                     if (sDirection == 0 && sEnemies[i].x > sPlayerX && sEnemies[i].x < sPlayerX + blastLen) inBlast = true;
                     if (sDirection == 1 && sEnemies[i].x < sPlayerX && sEnemies[i].x > sPlayerX - blastLen) inBlast = true;
                     
                     if (inBlast) {
                         sEnemies[i].health -= SPECIAL_BLAST_DAMAGE;
                         sEnemies[i].shockTimer = 50; // Shock for ~0.8 seconds
                         if (sEnemies[i].health <= 0) {
                             if (sEnemies[i].enemyType == 2) {
                                 sEnemies[i].health = 1; 
                                 sEnemies[i].state = STATE_DEATH;
                                 sEnemies[i].currentFrame = 0;
                                 sEnemies[i].animTimer = 0;
                                 sEnemies[i].isAttacking = false;
                             } else {
                                 sEnemies[i].isAlive = false;
                                 sPhaseEnemiesKilled++;
                             }
                         } else {
                             sEnemies[i].state = STATE_DAMAGE;
                             sEnemies[i].currentFrame = 0;
                             sEnemies[i].animTimer = 0;
                             sEnemies[i].isAttacking = false;
                         }
                     }
                 }
             }
             // Damage clay creatures too
             for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
                 if (sClayCreatures[i].isAlive) {
                     bool inBlast = false;
                     if (sDirection == 0 && sClayCreatures[i].x > sPlayerX && sClayCreatures[i].x < sPlayerX + blastLen) inBlast = true;
                     if (sDirection == 1 && sClayCreatures[i].x < sPlayerX && sClayCreatures[i].x > sPlayerX - blastLen) inBlast = true;
                     if (inBlast) {
                         sClayCreatures[i].health -= SPECIAL_BLAST_DAMAGE;
                         if (sClayCreatures[i].health <= 0) {
                             sClayCreatures[i].isAlive = false;
                             sScore += 10;
                         }
                     }
                 }
             }
         }
         
         sSpecialComboFrame++;
         if (sSpecialComboFrame >= SPECIAL_ATTACK_FRAME_COUNT) {
             sIsSpecialCombo = false;
             sSpecialComboFrame = 0;
         }
     }
  }

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
        if (sEnemies[i].isAlive && sEnemies[i].state != STATE_HIDDEN && sEnemies[i].drawHeight > 20) {
          if (fabs(sPlayerX - sEnemies[i].x) < 80.0 &&
              fabs(sPlayerY - sEnemies[i].y) < 60.0) {
            sEnemies[i].health -= 34; // 3 hits to kill sand enemy
            sScore += 5;
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
      // Also damage clay creatures with melee
      for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
        if (sClayCreatures[i].isAlive) {
          if (fabs(sPlayerX - sClayCreatures[i].x) < 80.0 &&
              fabs(sPlayerY - sClayCreatures[i].y) < 60.0) {
            sClayCreatures[i].health -= 30;
            if (sClayCreatures[i].health <= 0) {
              sClayCreatures[i].isAlive = false;
              sScore += 10;
              spawnHitEffect(sClayCreatures[i].x, sClayCreatures[i].y);
            } else {
              if (sClayCreatures[i].type == CLAY_CENTIPEDE) {
                sClayCreatures[i].state = CLAY_DAMAGE;
                sClayCreatures[i].currentFrame = 0;
              }
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

  // Update Bullets - Player kunai
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (sBullets[i].isActive) {
      sBullets[i].x += sBullets[i].velX;
      if (sBullets[i].x < sCameraX || sBullets[i].x > sCameraX + sScreenW)
        sBullets[i].isActive = false;
      // Hit enemies
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (sEnemies[j].isAlive && sEnemies[j].state != STATE_HIDDEN && sEnemies[j].drawHeight > 20 && 
            fabs(sBullets[i].x - sEnemies[j].x) < 50 &&
            fabs(sBullets[i].y - sEnemies[j].y) < 50) {
          int kunaiDmg =
              (sEnemies[j].enemyType == 2) ? 8 : 25; // Boss resists kunai more
          sEnemies[j].health -= kunaiDmg;
          sScore += 5;
          sBullets[i].isActive = false;
          if (sEnemies[j].health <= 0) {
            if (sEnemies[j].enemyType == 2) {
              sEnemies[j].health = 1;
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
      // Hit clay creatures
      if (sBullets[i].isActive) {
        for (int j = 0; j < MAX_CLAY_CREATURES; j++) {
          if (sClayCreatures[j].isAlive &&
              fabs(sBullets[i].x - sClayCreatures[j].x) < 50 &&
              fabs(sBullets[i].y - sClayCreatures[j].y) < 50) {
            sClayCreatures[j].health -= 20;
            sBullets[i].isActive = false;
            if (sClayCreatures[j].health <= 0) {
              sClayCreatures[j].isAlive = false;
              sScore += 10;
              spawnHitEffect(sClayCreatures[j].x, sClayCreatures[j].y);
            }
            break;
          }
        }
      }
    }
  }

  // Update Enemy Bullets (Clay Balls)
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (sEnemyBullets[i].isActive) {
      sEnemyBullets[i].x += sEnemyBullets[i].velX;
      sEnemyBullets[i].y += sEnemyBullets[i].velY;

      // Apply gravity to clay balls for arc trajectory
      sEnemyBullets[i].velY -= 0.3;

      // Animate clay ball
      sEnemyBullets[i].animTimer++;
      if (sEnemyBullets[i].animTimer >= 4) {
        sEnemyBullets[i].animTimer = 0;
        sEnemyBullets[i].animFrame = (sEnemyBullets[i].animFrame + 1) % CLAY_BALL_FRAMES;
      }

      // Off screen or hit ground
      if (sEnemyBullets[i].x < sCameraX - 100 ||
          sEnemyBullets[i].x > sCameraX + sScreenW + 100 ||
          sEnemyBullets[i].y < GROUND_LEVEL - 10) {
        spawnHitEffect(sEnemyBullets[i].x, fmax(sEnemyBullets[i].y, GROUND_LEVEL));
        sEnemyBullets[i].isActive = false;
        continue;
      }

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
        spawnHitEffect(sEnemyBullets[i].x, sEnemyBullets[i].y);
        sEnemyBullets[i].isActive = false;
      }
    }
  }

  // Update Hit Effects
  for (int i = 0; i < MAX_HIT_EFFECTS; i++) {
    if (sHitEffects[i].isActive) {
      sHitEffects[i].timer++;
      if (sHitEffects[i].timer >= 5) {
        sHitEffects[i].timer = 0;
        sHitEffects[i].frame++;
        if (sHitEffects[i].frame >= CLAY_HIT_FRAMES) {
          sHitEffects[i].isActive = false;
        }
      }
    }
  }

  // Update Physics
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
      if (sPlayerVelY <= 0 &&
          oldY >= sObstacles[i].y + sObstacles[i].height - 2 &&
          sPlayerY <= sObstacles[i].y + sObstacles[i].height + 5 &&
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

  // =====================================================
  // Enemy Spawning Logic
  // =====================================================
  sSpawnTimer++;
  if (sSpawnTimer >= sSpawnInterval) {
    sSpawnTimer = 0;

    int activeCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
      if (sEnemies[i].isAlive)
        activeCount++;

    if (sCurrentPhase == PHASE_1) {
      if (sPhaseEnemiesSpawned < 10 && activeCount < MAX_ENEMIES) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!sEnemies[i].isAlive) {
            double spawnX = sPlayerX + (400 + rand() % 300);
            if (spawnX > 1080 - 100)
              spawnX = 1080 - 200;
            resetEnemy(i, (int)spawnX, (int)spawnX + 50, 0); // Sand Enemy
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
            resetEnemy(i, (int)spawnX, (int)spawnX + 50, 1); // Fast Sand Enemy
            sPhaseEnemiesSpawned++;
            break;
          }
        }
      }
    } else if (sCurrentPhase == PHASE_3) {
      // Boss-only phase: spawn Deidara once, NO sand guards
      if (sPhaseEnemiesSpawned == 0) {
        // Kill all remaining sand enemies for clean arena
        for (int i = 0; i < MAX_ENEMIES; i++) {
          sEnemies[i].isAlive = false;
        }
        resetEnemy(0, 2600, 2800, 2); // Deidara boss at index 0
        sPhaseEnemiesSpawned++;
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

    // Spawn 2 target birds immediately for this phase
    for (int i = 0; i < 2; i++) {
        spawnClayCreature(CLAY_BIRD, sPlayerX + 400 + (i * 200), GROUND_LEVEL, 1);
    }
    
    // Make them permanent so they last until destroyed / boss phase
    for (int i=0; i<MAX_CLAY_CREATURES; i++) {
       if (sClayCreatures[i].isAlive) sClayCreatures[i].lifetime = 9999;
    }
  } else if (sCurrentPhase == PHASE_2 && sPlayerX > 2160 &&
             sPhaseEnemiesKilled >= 15) {
    // Moved to Phase 3
    sCurrentPhase = PHASE_3;
    sPhaseEnemiesSpawned = 0;
    sPhaseEnemiesKilled = 0;
    sSpawnInterval = 60;
    sPhasePotion.isActive = true;
    sPhasePotion.y = GROUND_LEVEL;
    sBossRageMode = false;
    
    // Disappear clay bird from phase 2 cleanly before boss logic takes over
    for (int i=0; i<MAX_CLAY_CREATURES; i++) {
        sClayCreatures[i].isAlive = false;
    }
    triggerStory(1); // Deidara Encounter Story (3)
  }

  // Potion Collection
  if (sPhasePotion.isActive) {
    if (fabs(sPlayerX - sPhasePotion.x) < 50.0 &&
        fabs(sPlayerY - sPhasePotion.y) < 50.0) {
      sPlayerHealth = 100;
      sPhasePotion.isActive = false;
    }
  }

  // =====================================================
  // Update Clay Creatures
  // =====================================================
  for (int i = 0; i < MAX_CLAY_CREATURES; i++) {
    if (sClayCreatures[i].isAlive) {
      sClayCreatures[i].lifetime--;
      if (sClayCreatures[i].lifetime <= 0) {
        sClayCreatures[i].isAlive = false;
        continue;
      }

      double dist = sPlayerX - sClayCreatures[i].x;
      sClayCreatures[i].direction = (dist > 0) ? 0 : 1;

      if (sClayCreatures[i].type == CLAY_BIRD) {
        // Clay Bird: flies in a sine-wave pattern overhead, drops bombs
        double targetY = GROUND_LEVEL + 280 + 40 * sin(sElapsedFrames * 0.05);
        sClayCreatures[i].velY = (targetY - sClayCreatures[i].y) * 0.05;
        sClayCreatures[i].y += sClayCreatures[i].velY;

        // Move toward player horizontally (hyper speed to match Kakashi)
        double birdSpeed = 8.0;
        if (fabs(dist) > 20) {
          sClayCreatures[i].x += (dist > 0) ? birdSpeed : -birdSpeed;
          sClayCreatures[i].state = CLAY_MOVING;
        } else {
          sClayCreatures[i].state = CLAY_IDLE;
        }

        // Drop clay bombs only when Kakashi is directly underneath or close
        sClayCreatures[i].attackTimer++;
        int bombInterval = (sCurrentPhase == PHASE_2) ? 40 : (sBossRageMode ? 60 : 100); 
        if (sClayCreatures[i].attackTimer >= bombInterval) {
          // Check range (only bomb if player is close horizontally)
          if (fabs(dist) < 150.0) {
            sClayCreatures[i].attackTimer = 0;
            sClayCreatures[i].state = CLAY_ATTACKING;
            
            // Drop straight down with slight momentum of the bird
            double aimVelY = -8.0; 
            double aimVelX = (dist > 0) ? 1.0 : -1.0; 
            if (fabs(dist) < 30.0) aimVelX = 0; // Pure vertical drop if directly under
            
            spawnClayBall(sClayCreatures[i].x + sClayCreatures[i].width / 2,
                          sClayCreatures[i].y, aimVelX, aimVelY, 20.0);
          }
        }

      } else { // CENTIPEDE
        // Clay Centipede: charges on the ground toward player
        double centSpeed = sBossRageMode ? 7.0 : 5.0;

        if (sClayCreatures[i].state == CLAY_DAMAGE) {
          // Stay in damage state briefly
          sClayCreatures[i].animTimer++;
          if (sClayCreatures[i].animTimer >= 4) {
            sClayCreatures[i].animTimer = 0;
            sClayCreatures[i].currentFrame++;
            if (sClayCreatures[i].currentFrame >= CLAY_CENT_DAMAGE_FRAMES) {
              sClayCreatures[i].state = CLAY_MOVING;
              sClayCreatures[i].currentFrame = 0;
            }
          }
        } else if (fabs(dist) > 60) {
          sClayCreatures[i].x += (dist > 0) ? centSpeed : -centSpeed;
          sClayCreatures[i].state = CLAY_MOVING;
        } else {
          // Attack when close
          sClayCreatures[i].state = CLAY_ATTACKING;
          sClayCreatures[i].attackTimer++;
          if (sClayCreatures[i].attackTimer >= 5 &&
              sClayCreatures[i].currentFrame == 5) { // Damage on frame 5
            if (fabs(sPlayerX - sClayCreatures[i].x) < 80 &&
                fabs(sPlayerY - sClayCreatures[i].y) < 60) {
              sPlayerHealth -= 15;
              sIsTakingDamage = true;
              sDamageFrame = 0;
              sDamageTimer = 0;
            }
          }
        }

        // Ground level
        sClayCreatures[i].y = GROUND_LEVEL;
      }

      // Animate
      if (sClayCreatures[i].state != CLAY_DAMAGE) {
        sClayCreatures[i].animTimer++;
        int animSpeed = 6;
        if (sClayCreatures[i].animTimer >= animSpeed) {
          sClayCreatures[i].animTimer = 0;
          int maxFrames = 1;
          if (sClayCreatures[i].type == CLAY_BIRD) {
            switch (sClayCreatures[i].state) {
            case CLAY_IDLE: maxFrames = CLAY_BIRD_IDLE_FRAMES; break;
            case CLAY_MOVING: maxFrames = CLAY_BIRD_MOVE_FRAMES; break;
            case CLAY_ATTACKING: maxFrames = CLAY_BIRD_ATTACK_FRAMES; break;
            default: maxFrames = CLAY_BIRD_IDLE_FRAMES; break;
            }
          } else {
            switch (sClayCreatures[i].state) {
            case CLAY_IDLE: maxFrames = CLAY_CENT_IDLE_FRAMES; break;
            case CLAY_MOVING: maxFrames = CLAY_CENT_MOVE_FRAMES; break;
            case CLAY_ATTACKING: maxFrames = CLAY_CENT_ATTACK_FRAMES; break;
            default: maxFrames = CLAY_CENT_IDLE_FRAMES; break;
            }
          }
          sClayCreatures[i].currentFrame = (sClayCreatures[i].currentFrame + 1) % maxFrames;

          // Reset attacking state after animation completes
          if (sClayCreatures[i].state == CLAY_ATTACKING && sClayCreatures[i].currentFrame == 0) {
            sClayCreatures[i].state = CLAY_MOVING;
          }
        }
      }
    }
  }

  // =====================================================
  // Update Enemies
  // =====================================================
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (sEnemies[i].isAlive) {
      if (sEnemies[i].shockTimer > 0) {
          sEnemies[i].shockTimer--;
          continue; // Paralyzed: Skip AI and movement
      }

      double dist = sPlayerX - sEnemies[i].x;
      sEnemies[i].direction = (dist > 0) ? 0 : 1;

      // Decrement cooldowns
      if (sEnemies[i].attackCooldown > 0) sEnemies[i].attackCooldown--;
      if (sEnemies[i].summonCooldown > 0) sEnemies[i].summonCooldown--;

      if (sEnemies[i].enemyType == 0 || sEnemies[i].enemyType == 1) {
        // ======================
        // SAND ENEMY AI (Burrowing)
        // ======================
        double speed = (sEnemies[i].enemyType == 1) ? 6.0 : 4.0; // Faster move when hidden

        switch (sEnemies[i].state) {
        case STATE_BURROWING:
            sEnemies[i].drawHeight -= 4.0;
            if (sEnemies[i].drawHeight <= 0) {
                sEnemies[i].drawHeight = 0;
                sEnemies[i].state = STATE_HIDDEN;
                sEnemies[i].hideTimer = 20 + (rand() % 20); // Faster reappear: ~0.5s underground
                // Set targetX to be near player (behind or in front)
                if (rand() % 2 == 0) sEnemies[i].targetX = sPlayerX - 60;
                else sEnemies[i].targetX = sPlayerX + 60;
            }
            break;

        case STATE_HIDDEN:
            sEnemies[i].hideTimer--;
            // Move toward targetX
            if (sEnemies[i].x < sEnemies[i].targetX) sEnemies[i].x += speed;
            else if (sEnemies[i].x > sEnemies[i].targetX) sEnemies[i].x -= speed;

            if (sEnemies[i].hideTimer <= 0 && fabs(sEnemies[i].x - sEnemies[i].targetX) < 10) {
                sEnemies[i].state = STATE_EMERGING;
                sEnemies[i].drawHeight = 0;
            }
            break;

        case STATE_EMERGING:
            sEnemies[i].drawHeight += 4.0;
            if (sEnemies[i].drawHeight >= sEnemies[i].height) {
                sEnemies[i].drawHeight = sEnemies[i].height;
                sEnemies[i].state = STATE_STANCE;
                // Immediate attack chance upon emerging
                if (sEnemies[i].attackCooldown <= 0) {
                    sEnemies[i].isAttacking = true;
                    sEnemies[i].attackFrame = 0;
                    sEnemies[i].attackTimer = 0;
                }
            }
            break;

        case STATE_DAMAGE:
            // Recover from damage back into burrow loop
            sEnemies[i].animTimer++;
            if (sEnemies[i].animTimer >= 30) {
                sEnemies[i].state = STATE_BURROWING;
            }
            break;

        default: // STANCE/WALK/RUN
            if (!sEnemies[i].isAttacking) {
                if (fabs(dist) > 400.0) {
                    // Sprint toward player if far away
                    sEnemies[i].x += (dist > 0) ? speed * 1.5 : -speed * 1.5;
                } else if (fabs(dist) > 100.0) {
                    sEnemies[i].x += (dist > 0) ? speed : -speed;
                } else {
                    // When close, attack then burrow
                    if (sEnemies[i].attackCooldown <= 0) {
                        sEnemies[i].isAttacking = true;
                        sEnemies[i].attackFrame = 0;
                        sEnemies[i].attackTimer = 0;
                    } else if (rand() % 100 < 5) {
                        sEnemies[i].state = STATE_BURROWING;
                    }
                }
            }
            break;
        }

      } else if (sEnemies[i].enemyType == 2) {
        // ======================
        // DEIDARA BOSS AI
        // ======================

        // Check rage mode
        if (sEnemies[i].health <= (BOSS_MAX_HEALTH / 2) && !sBossRageMode) {
          sBossRageMode = true;
          // Heal potion for player on rage transition
          sPhasePotion.isActive = true;
          sPhasePotion.x = sPlayerX + 100;
          sPhasePotion.y = GROUND_LEVEL;
        }

        if (sEnemies[i].state == STATE_DAMAGE) {
          // Stay damage until anim finishes
        } else if (sEnemies[i].state == STATE_DEATH) {
          // Stay death until anim finishes
        } else if (!sEnemies[i].isAttacking) {

          // Speed slowed down significantly (6.0 walk, 9.0 run/rage)
          double moveSpeed = sBossRageMode ? 9.0 : 6.0;

          if (fabs(dist) > 300.0) {
            // Far: Run toward player
            sEnemies[i].x += (dist > 0) ? moveSpeed : -moveSpeed;
            sEnemies[i].state = STATE_RUN;
          } else if (fabs(dist) > 100.0) {
            // Medium: Walk/approach
            sEnemies[i].x += (dist > 0) ? moveSpeed : -moveSpeed;
            sEnemies[i].state = STATE_WALK;
          } else {
            sEnemies[i].state = STATE_STANCE;

            // Attack selection - CHAOTIC in rage
            int attackChance = sBossRageMode ? 2 : 15;
            if (sEnemies[i].attackCooldown <= 0 && rand() % attackChance < 3) {
              sEnemies[i].isAttacking = true;
              sEnemies[i].attackFrame = 0;
              sEnemies[i].attackTimer = 0;

              // Attack selection - STRICT PHASE FILTERING
              bool lowHealth = (sEnemies[i].health <= (BOSS_MAX_HEALTH / 2));
              int r = rand() % 10;
              
              if (!lowHealth) {
                // Phase 1 (51-100%): Only Bombs & Bugs
                if (r < 6) {
                    sEnemies[i].state = STATE_WEAPON_THROW; // 60% Bombs
                    sEnemies[i].attackCooldown = 120; // Long recovery
                } else {
                    sEnemies[i].state = STATE_CENTIPEDE;   // 40% Bugs
                    sEnemies[i].attackCooldown = 140; // Even longer recovery
                }
              } else {
                // Phase 2 (0-50%): Birds Unlocked, Double Attacks
                if (r < 3) {
                    sEnemies[i].state = STATE_ATTACK1;     // Melee combo
                    sEnemies[i].attackCooldown = 60;
                } else if (r < 6) {
                    sEnemies[i].state = STATE_WEAPON_THROW; // "Double Bombs"
                    sEnemies[i].attackCooldown = 70;
                } else if (r < 8 && sEnemies[i].summonCooldown <= 0) {
                    sEnemies[i].state = STATE_SPECIAL;      // Birds enabled!
                    sEnemies[i].summonCooldown = 240; // Double summon CD
                    sEnemies[i].attackCooldown = 60;
                } else {
                    sEnemies[i].state = STATE_CENTIPEDE;
                    sEnemies[i].summonCooldown = 240;
                    sEnemies[i].attackCooldown = 60;
                }
              }
            }
          }

          // In rage mode, occasionally jump toward player
          if (sBossRageMode && sEnemies[i].isOnGround && fabs(dist) > 150 && fabs(dist) < 400 && rand() % 60 == 0) {
            sEnemies[i].velY = 15.0;
            sEnemies[i].isOnGround = false;
            sEnemies[i].isJumping = true;
          }
        }
      }

      // =====================================================
      // Handle Enemy Attack State
      // =====================================================
      if (sEnemies[i].isAttacking) {
        sEnemies[i].attackTimer++;
        int atkDelay = 8; // Normal attack speed
        if (sEnemies[i].enemyType == 2)
          atkDelay = sBossRageMode ? 5 : 8; // Deidara attacks much slower now for balance

        if (sEnemies[i].attackTimer >= atkDelay) {
          sEnemies[i].attackTimer = 0;
          sEnemies[i].attackFrame++;

          int maxAttackFrames = 4; // Default for sand enemies (stance attack)
          if (sEnemies[i].enemyType == 2) {
            switch (sEnemies[i].state) {
            case STATE_ATTACK1: maxAttackFrames = DEI_ATTACK1_FRAMES; break;
            case STATE_WEAPON_THROW: maxAttackFrames = DEI_THROW_FRAMES; break;
            case STATE_SPECIAL: maxAttackFrames = DEI_THROW_FRAMES; break;
            case STATE_CENTIPEDE: maxAttackFrames = DEI_THROW_FRAMES; break;
            default: maxAttackFrames = DEI_ATTACK1_FRAMES; break;
            }
          }

          if (sEnemies[i].attackFrame >= maxAttackFrames) {
            sEnemies[i].isAttacking = false;
            sEnemies[i].attackFrame = 0;

            // Boss attack finishers
            if (sEnemies[i].enemyType == 2) {
              if (sEnemies[i].state == STATE_WEAPON_THROW) {
                // Throw clay bomb projectile (slower speed for balance)
                double throwVelX = (sEnemies[i].direction == 0) ? 9.0 : -9.0;
                double throwVelY = 5.0; // Arc upward
                spawnClayBall(sEnemies[i].x + sEnemies[i].width / 2.0,
                              sEnemies[i].y + sEnemies[i].height / 2.0,
                              throwVelX, throwVelY, 25.0);
                
                // In Phase 2 (Rage), throw "Double Bombs" with non-overlapping trajectories
                if (sBossRageMode) {
                  // Trajectory 1: Faster and more horizontal
                  spawnClayBall(sEnemies[i].x + sEnemies[i].width / 2.0 + (sEnemies[i].direction == 0 ? 20 : -20),
                                sEnemies[i].y + sEnemies[i].height / 2.0 + 10,
                                throwVelX * 1.2, throwVelY + 2.0, 20.0);
                  // Trajectory 2: Slower and higher arc
                  spawnClayBall(sEnemies[i].x + sEnemies[i].width / 2.0 + (sEnemies[i].direction == 0 ? -20 : 20),
                                sEnemies[i].y + sEnemies[i].height / 2.0 - 10,
                                throwVelX * 0.8, throwVelY + 6.0, 15.0);
                }
              } else if (sEnemies[i].state == STATE_SPECIAL) {
                // Summon clay bird
                spawnClayCreature(CLAY_BIRD, sEnemies[i].x,
                                  sEnemies[i].y, sEnemies[i].direction);
                // In rage, also summon centipede simultaneously
                if (sBossRageMode) {
                  spawnClayCreature(CLAY_CENTIPEDE, sEnemies[i].x,
                                    GROUND_LEVEL, sEnemies[i].direction);
                }
              } else if (sEnemies[i].state == STATE_CENTIPEDE) {
                // Summon clay centipede
                spawnClayCreature(CLAY_CENTIPEDE, sEnemies[i].x,
                                  GROUND_LEVEL, sEnemies[i].direction);
                // In rage, also summon bird
                if (sBossRageMode) {
                  spawnClayCreature(CLAY_BIRD, sEnemies[i].x,
                                    sEnemies[i].y, sEnemies[i].direction);
                }
              }
              sEnemies[i].state = STATE_STANCE;
            }
          }
        }

        // Melee damage check
        if (sEnemies[i].enemyType == 2) {
          // Deidara melee: damage on frame 8 of attack1
          if (sEnemies[i].state == STATE_ATTACK1 &&
              sEnemies[i].attackFrame == 8 && sEnemies[i].attackTimer == 0) {
            if (fabs(sPlayerX - sEnemies[i].x) < 100.0 &&
                fabs(sPlayerY - sEnemies[i].y) < 80.0) {
              int dmg = sBossRageMode ? 35 : 25;
              sPlayerHealth -= dmg;
              sIsTakingDamage = true;
              sDamageFrame = 0;
              sDamageTimer = 0;
            }
          }
        } else {
          // Sand Enemy melee: damage on frame 2
          if (sEnemies[i].isAttacking &&
              sEnemies[i].attackFrame == 2 && sEnemies[i].attackTimer == 0) {
            if (fabs(sPlayerX - sEnemies[i].x) < 65.0 &&
                fabs(sPlayerY - sEnemies[i].y) < 60.0) {
              int dmg = (sEnemies[i].enemyType == 1) ? 15 : 10;
              sPlayerHealth -= dmg;
              sIsTakingDamage = true;
              sDamageFrame = 0;
              sDamageTimer = 0;
            }
          }
        }
      }

      // Enemy bounds
      double minBound = 0, maxBound = sWorldWidth;
      if (sEnemies[i].x < minBound)
        sEnemies[i].x = minBound;
      if (sEnemies[i].x > maxBound - sEnemies[i].width)
        sEnemies[i].x = maxBound - sEnemies[i].width;

      // Enemy Physics
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

      // Animate walk/stance/damage/death
      if (!sEnemies[i].isAttacking) {
        sEnemies[i].animTimer++;
        int animDelay = sAnimDelay;
        if (sEnemies[i].enemyType == 2 && sBossRageMode) animDelay = sAnimDelay / 2;

        if (sEnemies[i].animTimer >= animDelay) {
          sEnemies[i].animTimer = 0;
          int frameCount = SAND_WALK_FRAMES;

          if (sEnemies[i].enemyType == 2) {
            switch (sEnemies[i].state) {
            case STATE_STANCE: frameCount = DEI_STANCE_FRAMES; break;
            case STATE_RUN:
            case STATE_WALK: frameCount = DEI_RUN_FRAMES; break;
            case STATE_DAMAGE: frameCount = DEI_DAMAGE_FRAMES; break;
            case STATE_DEATH: frameCount = DEI_DEATH_FRAMES; break;
            default: frameCount = DEI_STANCE_FRAMES; break;
            }
          } else {
            // Sand enemy frame counts
            if (sEnemies[i].state == STATE_DAMAGE) {
              frameCount = SAND_STANCE_FRAMES; // Use stance frames during damage
            } else if (sEnemies[i].state == STATE_WALK || sEnemies[i].state == STATE_RUN) {
              frameCount = SAND_WALK_FRAMES;
            } else {
              frameCount = SAND_STANCE_FRAMES;
            }
          }

          // Boss death handling
          if (sEnemies[i].enemyType == 2 && sEnemies[i].state == STATE_DEATH &&
              sEnemies[i].currentFrame == DEI_DEATH_FRAMES - 1) {
            sEnemies[i].isAlive = false;
            sPhaseEnemiesKilled++;
            sScore += 50; // +50 for beating Deidara
            // Kill all clay creatures when boss dies
            for (int c = 0; c < MAX_CLAY_CREATURES; c++) {
              if (sClayCreatures[c].isAlive) {
                sClayCreatures[c].isAlive = false;
                spawnHitEffect(sClayCreatures[c].x, sClayCreatures[c].y);
              }
            }
            sCurrentPhase = BOSS_DEFEATED;
            sIsWinning = true;
            sWinFrame = 0;
            sWinTimer = 0;
            triggerStory(2); // Deidara Defeated Story (4, 5)
          } else if (sEnemies[i].state == STATE_DAMAGE) {
            int recoverFrame = (sEnemies[i].enemyType == 2) ? (DEI_DAMAGE_FRAMES - 1) : 6;
            if (sEnemies[i].currentFrame >= recoverFrame) {
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

void level3Keyboard(unsigned char key) {
  if (sIsStoryPlaying) {
    if (key == ' ' || key == 13 || key == 'p' || key == 'P') {
      sStorySubIndex++;
      if (sStorySubIndex >= sStoryGroupCount[sCurrentStoryGroup]) {
        sIsStoryPlaying = false;
        sCurrentStoryGroup = -1;
      }
    }
    return;
  }

  if (sIsDying || sIsGameOver)
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
      // Kill all clay creatures
      for (int c = 0; c < MAX_CLAY_CREATURES; c++)
        sClayCreatures[c].isAlive = false;
      sCurrentPhase = BOSS_DEFEATED;
      sIsWinning = true;
      sWinFrame = 0;
      sWinTimer = 0;
    }
  }

  // Code input mode intercepts keyboard
  if (sCodeInputMode && !sCodesVerified) {
    if (key == 13) { // Enter
      bool match = true;
      const char *expected = (sCodeInputStep == 0) ? sExpectedCode1 : sExpectedCode2;
      if (sCodeInputPos != (int)strlen(expected)) {
        match = false;
      } else {
        for (int i = 0; i < sCodeInputPos; i++) {
          char a = sCodeInputBuffer[i];
          char b = expected[i];
          if (a >= 'a' && a <= 'z') a -= 32;
          if (b >= 'a' && b <= 'z') b -= 32;
          if (a != b) { match = false; break; }
        }
      }
      if (match) {
        sCodeError = false;
        if (sCodeInputStep == 0) {
          sCodeInputStep = 1;
          memset(sCodeInputBuffer, 0, sizeof(sCodeInputBuffer));
          sCodeInputPos = 0;
        } else {
          sCodesVerified = true;
          sCodeInputMode = false;
        }
      } else {
        sCodeError = true;
        sCodeErrorTimer = 120;
      }
    } else if (key == 8) { // Backspace
      if (sCodeInputPos > 0) {
        sCodeInputPos--;
        sCodeInputBuffer[sCodeInputPos] = '\0';
      }
      sCodeError = false;
    } else if (key == 27) { // ESC to cancel
      sCodeInputMode = false;
      memset(sCodeInputBuffer, 0, sizeof(sCodeInputBuffer));
      sCodeInputPos = 0;
      sCodeInputStep = 0;
    } else if (sCodeInputPos < 30) {
      if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') ||
          (key >= '0' && key <= '9') || key == ' ') {
        char c = key;
        if (c >= 'a' && c <= 'z') c -= 32;
        sCodeInputBuffer[sCodeInputPos] = c;
        sCodeInputPos++;
        sCodeInputBuffer[sCodeInputPos] = '\0';
        sCodeError = false;
      }
    }
    return;
  }

  // Enter key to start code input after win
  if (sCurrentPhase == BOSS_DEFEATED && sWinDelayTimer >= 5.0 && !sCodesVerified && !sCodeInputMode) {
    if (key == 13) {
      sCodeInputMode = true;
      memset(sCodeInputBuffer, 0, sizeof(sCodeInputBuffer));
      sCodeInputPos = 0;
      sCodeInputStep = 0;
      return;
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
  if (key == 'o' || key == 'O') {
      if (sPlayerFuel >= 50.0 && !sIsSpecialCombo && !sIsThrowing && !sIsAttacking) {
          sIsSpecialCombo = true;
          sPlayerFuel -= 50.0;
          sSpecialComboTimer = 0;
          sSpecialComboFrame = 0; // Ensure sequence resets
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

void level3SpecialKeyboard(unsigned char key) {
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

void level3KeyboardUp(unsigned char key) {
  if (key == 'a' || key == 'A')
    sMovingLeft = false;
  if (key == 'd' || key == 'D')
    sMovingRight = false;
  if (key == 's' || key == 'S')
    sIsCrouching = false;
  if (key == 'l' || key == 'L')
    sIsRunning = false;
  if (key == 'j' || key == 'J') {
      // Released J, do nothing specific since it's just tap to throw now
  }
}

void level3SpecialKeyboardUp(unsigned char key) {
  if (key == 100) // GLUT_KEY_LEFT
    sMovingLeft = false;
  if (key == 102) // GLUT_KEY_RIGHT
    sMovingRight = false;
  if (key == 103) // GLUT_KEY_DOWN
    sIsCrouching = false;
}

bool isLevel3TransitionReady() {
  return (sCurrentPhase == BOSS_DEFEATED && sCodesVerified && sWinDelayTimer >= 8.0);
}
