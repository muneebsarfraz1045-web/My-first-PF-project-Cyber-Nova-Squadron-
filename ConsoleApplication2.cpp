#include <raylib.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

const int screenWidth = 1600;
const int screenHeight = 900;

const int STAR_COUNT = 300;
const int MAX_BULLETS = 20;
const int MAX_ENEMY_BULLETS = 10;

int starX[STAR_COUNT];
int starY[STAR_COUNT];
int starSpeed1[STAR_COUNT];
float starSpeedMultiplier = 1.0f;
float enemyWaveTime = 0;
float level2Time = 0;
float level3Time = 0;
float spiralAmplitude = 35;
float level4ZigzagDir = 1;
float level4Speed = 4.0f;
float level4Time = 0;
float level5Time = 0;
float homingStrength = 0.6f;
int shipX = 700, shipY = 600, shipSpeed = 7;
float shipScale = 0.4f, bulletScale = 0.1f, bulletSpeed = 10.0f, enemyScale = 0.2f;
float fireCooldown = 0.0f;  // Global

float boosterOffsetX = 0;      // horizontal movement
float boosterOffsetY = 0;      // vertical flicker
float boosterTimer = 0;        // timer for animation
int bossMaxHealth = 200;

bool gamePaused = false;

bool level2 = false;
bool level2Started = false;

bool level3 = false;
bool level3Started = false;

bool level4 = false;
bool level4Started = false;

bool level5 = false;
bool level5Started = false;


// Level 1
Texture2D spaceship;
Texture2D bullet;
Texture2D enemy;
Texture2D blast;

// Level 2
Texture2D spaceship2;
Texture2D enemy2;
Texture2D bullet2;

// Level 3
Texture2D spaceship3;
Texture2D enemy3;
Texture2D bullet3;

// Level 4
Texture2D spaceship4;
Texture2D enemy4;
Texture2D bullet4;

// Level 5
Texture2D spaceship5;
Texture2D enemy5;
Texture2D bullet5;
Texture2D boss;
float bossX = screenWidth - 200;
float bossY = 100;
float bossScale = 0.4f;
int bossHealth = 200;
bool bossAlive = false;

float bossSpeedX = 3.0f;
float bossSpeedY = 1.0f; 
float bossSpeedModifier = 1.0f;
int level6 = false;

// Token
Texture2D token;
Texture2D booster; 


int shipHealth = 5;
bool shipAlive = true;

struct Bullet
{
    float x, y;
    float speed;
    bool active;
    float dx;
    float dy;
};
Bullet bullets[MAX_BULLETS];

// Enemy
const int MAX_ROWS = 10;
const int MAX_COLS = 20;
float enemyX[MAX_ROWS][MAX_COLS];
float enemyY[MAX_ROWS][MAX_COLS];
bool enemyAlive[MAX_ROWS][MAX_COLS];

int currentEnemyRows = 2;
int currentEnemyCols = 12;
Texture2D currentEnemyTexture;

Bullet enemyBullets[MAX_ENEMY_BULLETS];
float enemyFireTimer = 0;

struct Explosion
{
    float x, y;
    float scale;
    int frames;
    bool active;
};
const int MAX_EXPLOSIONS = 50;
Explosion explosions[MAX_EXPLOSIONS];

int score = 0;

//  Power-up token 
struct PowerToken
{
    float x, y;
    float speed;
    int type;
    bool active;
    bool collected;
};

int multiShotTimer = 0;
int shieldTimer = 0;
int speedBoostTimer = 0;
int slowDownTimer = 0;

bool powerActive = false;

const int MAX_TOKENS = 1;
PowerToken tokens[MAX_TOKENS];

int multiShot = 0;
int shield = 0;
int speedBoost = 0;

//  Stars 
void InitStars()
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        starX[i] = rand() % screenWidth;
        starY[i] = rand() % screenHeight;
        starSpeed1[i] = 2 + rand() % 5;
    }
}

void UpdateStars()
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        starY[i] += starSpeed1[i] * starSpeedMultiplier;
        if (starY[i] > screenHeight)
        {
            starY[i] = 0;
            starX[i] = rand() % screenWidth;
        }
    }
}

void DrawStars()
{
    for (int i = 0; i < STAR_COUNT; i++)
        DrawPixel(starX[i], starY[i], WHITE);
}

void LoadTextures()
{

    // Level 1
    spaceship = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\spaceship1.png");
    bullet = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\bullet.png");
    enemy = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\alien1.png");
    blast = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\blast1.png");

    // Level 2
    spaceship2 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\ship2.png");
    bullet2 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\bullet2.png");
    enemy2 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\alien2.png");

    // Level 3
    spaceship3 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\ship3.png");
    bullet3 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\bullet3.png");
    enemy3 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\alien3.png");

    // Level 4
    spaceship4 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\ship4.png");
    bullet4 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\bullet4.png");
    enemy4 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\enemy4.png");

    // Level 5
    spaceship5 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\ship5.png");
    bullet5 = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\bullet5.png");
    boss = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\boss.png");

    // token
    token = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\token.png");
    booster = LoadTexture("C:\\spaceship\\ConsoleApplication2\\Graphics\\flame.png");

    currentEnemyTexture = enemy;

    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        enemyBullets[i].active = false;
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        explosions[i].active = false;
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens[i].active = false;
}

int waveCounter = 0;
int waveDelay = 90; 

void ActivateEnemyWave()
{
    waveCounter++;

    if (waveCounter >= waveDelay) {
        int count = 0;
        for (int r = 0; r < MAX_ROWS && count < 1; r++) {
            for (int c = 0; c < MAX_COLS && count < 5; c++) {
                if (!enemyAlive[r][c]) { 
                    enemyAlive[r][c] = true;
                    enemyY[r][c] = -40;  
                    enemyX[r][c] = rand() % (screenWidth - 60);
                    count++;
                }
            }
        }
        waveCounter = 0; 
    }
}


void DrawSpaceship(int x, int y, float scale)
{
    if (shipAlive)
    {
        if (speedBoost > 0)
        {
            float boosterScale = 0.25f + ((rand() % 10) / 100.0f); 
            DrawTextureEx(
                booster,
                { x + (spaceship.width * scale) / 2 - (booster.width * boosterScale) / 2 + boosterOffsetX,
                  y + spaceship.height * scale - 5 + boosterOffsetY },
                0.0f,
                boosterScale,
                WHITE
            );

        }

        DrawTextureEx(spaceship, { (float)x, (float)y }, 0.0f, scale, WHITE);
    }
}


void MoveSpaceship(int& shipX, int& shipY, int speed, float shipScale)
{
    if (!shipAlive) return;

    int actualSpeed = speed;

    if (speedBoost > 0)
        actualSpeed += 3;
    if (slowDownTimer > 0)
        actualSpeed = max(1, speed - 3);

    if (IsKeyDown(KEY_UP))
        shipY -= actualSpeed;
    if (IsKeyDown(KEY_DOWN))
        shipY += actualSpeed;
    if (IsKeyDown(KEY_LEFT))
        shipX -= actualSpeed;
    if (IsKeyDown(KEY_RIGHT))
        shipX += actualSpeed;

    if (shipX < 0)
        shipX = 0;
    if (shipX > screenWidth - spaceship.width * shipScale)
        shipX = screenWidth - spaceship.width * shipScale;
    if (shipY < 0)
        shipY = 0;
    if (shipY > screenHeight - spaceship.height * shipScale)
        shipY = screenHeight - spaceship.height * shipScale;
}

void ShootBullet(int shipX, int shipY, float shipScale, float bulletScale)
{
    if (multiShot > 0)
    {
        for (int dx = 0; dx <= 1; dx++)

        {
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active)
                {
                    bullets[i].active = true;
                    bullets[i].x = shipX + (spaceship.width * shipScale / 2) - (bullet.width * bulletScale / 2) + dx * 15;
                    bullets[i].y = shipY - bullet.height * bulletScale;
                    break;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].x = shipX + (spaceship.width * shipScale / 2) - (bullet.width * bulletScale / 2);
                bullets[i].y = shipY - bullet.height * bulletScale;
                break;
            }
        }
    }
}

void UpdateBullets(float bulletSpeed)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].y -= bulletSpeed;
            if (bullets[i].y < -bullet.height) bullets[i].active = false;
        }
    }
}

void DrawBullets(float bulletScale)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
            DrawTextureEx(bullet, { bullets[i].x, bullets[i].y }, 0.0f, bulletScale, WHITE);
    }
}

void InitEnemies(int rows, int cols, int startY)
{
    currentEnemyRows = rows;
    currentEnemyCols = cols;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            enemyX[row][col] = col * 110;
            enemyY[row][col] = startY + row * 90;
            enemyAlive[row][col] = true;
        }
    }

}

void UpdateEnemies()
{
    static float level3SpawnTimer = 0;
    static float level4BurstTimer = 0;

    level2Time += 0.05f;
    level3Time += 0.02f;
    level4Time += 0.05f;
    level5Time += 0.05f;

    for (int r = 0; r < currentEnemyRows; r++)
    {
        for (int c = 0; c < currentEnemyCols; c++)
        {
            if (!enemyAlive[r][c])
                continue;

            //  LEVEL 1 
            if (!level2 && !level3 && !level4 && !level5)
            {
                enemyX[r][c] += 2.5f;
                if (enemyX[r][c] > screenWidth + 50)
                    enemyX[r][c] = -50;

            }

            //  LEVEL 2 
            else if (level2 && !level3)
            {
                enemyX[r][c] += 3.0f;

                if (enemyX[r][c] > screenWidth)
                    enemyX[r][c] = 0;

                enemyY[r][c] += 0.4f;

                if (enemyY[r][c] > screenHeight)
                    enemyY[r][c] = 0;
            }

            //  LEVEL 3 
            else if (level3 && !level4)
            {
                static int level3Dir[MAX_ROWS][MAX_COLS]; 
                for (int r = 0; r < currentEnemyRows; r++)
                {
                    for (int c = 0; c < currentEnemyCols; c++)
                    {
                        if (level3Dir[r][c] == 0) level3Dir[r][c] = 1;

                        enemyX[r][c] += 0.1f * level3Dir[r][c]; 

                        if (enemyX[r][c] > screenWidth - 60) level3Dir[r][c] = -1;
                        if (enemyX[r][c] < 0) level3Dir[r][c] = 1;
                    }
                }
            }



            //  LEVEL 4 
            else if (level4 && !level5)
            {
                enemyX[r][c] += 2.5f;
                if (enemyX[r][c] > screenWidth + 50)
                    enemyX[r][c] = -50;

               
            }




        }
    }
}


void DrawEnemies(float scale)
{
    for (int row = 0; row < currentEnemyRows; row++)
    {
        for (int col = 0; col < currentEnemyCols; col++)
        {
            if (enemyAlive[row][col])
                DrawTextureEx(currentEnemyTexture, { enemyX[row][col], enemyY[row][col] }, 0.0f, scale, WHITE);
        }
    }
}

void EnemyShoot()
{
    enemyFireTimer += 1.0f / 60.0f;
    if (enemyFireTimer >= 1.0f)
    {
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        {
            if (!enemyBullets[i].active)
            {
                int row = rand() % currentEnemyRows;
                int col = rand() % currentEnemyCols;
                if (enemyAlive[row][col])
                {
                    enemyBullets[i].active = true;
                    enemyBullets[i].x = enemyX[row][col] + (currentEnemyTexture.width * 0.2f / 2) - (bullet.width * 0.1f / 2);
                    enemyBullets[i].y = enemyY[row][col] + (currentEnemyTexture.height * 0.2f);
                    break;
                }
            }
        }
        enemyFireTimer = 0;
    }
}
void UpdateEnemyBullets(float speed, int shipX, int shipY, float shipScale)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemyBullets[i].active)
        {

            if (!level5)
            {
                enemyBullets[i].y += speed;
            }

            else if (level5 && bossAlive)
            {
                enemyBullets[i].x += enemyBullets[i].dx * enemyBullets[i].speed;
                enemyBullets[i].y += enemyBullets[i].dy * enemyBullets[i].speed;
            }

            if (enemyBullets[i].y > screenHeight || enemyBullets[i].x > screenWidth || enemyBullets[i].x < 0)
                enemyBullets[i].active = false;

            if (shipAlive)
            {
                Rectangle shipRec = { (float)shipX, (float)shipY,
                                      spaceship.width * shipScale, spaceship.height * shipScale };

                Rectangle bulletRec = { enemyBullets[i].x, enemyBullets[i].y,
                                        bullet.width * 0.1f, bullet.height * 0.1f };

                if (CheckCollisionRecs(shipRec, bulletRec))
                {
                    enemyBullets[i].active = false;

                    if (shield <= 0)
                        shipHealth--;

                    if (shipHealth <= 0)
                        shipAlive = false;

                    for (int e = 0; e < MAX_EXPLOSIONS; e++)
                    {
                        if (!explosions[e].active)
                        {
                            explosions[e].active = true;
                            explosions[e].x = shipX + spaceship.width * shipScale / 2;
                            explosions[e].y = shipY + spaceship.height * shipScale / 2;
                            explosions[e].scale = 0.1f;
                            explosions[e].frames = 30;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void DrawEnemyBullets(float bulletScale)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemyBullets[i].active)
        {
            Color bulletColor = RED; 

            if (level2) bulletColor = ORANGE;
            else if (level3) bulletColor = YELLOW;
            else if (level4) bulletColor = GREEN;
            else if (level5) bulletColor = PURPLE;

            DrawTextureEx(bullet, { enemyBullets[i].x, enemyBullets[i].y }, 0.0f, bulletScale, bulletColor);
        }
    }
}


bool AllEnemiesDead()
{
    for (int r = 0; r < currentEnemyRows; r++)
        for (int c = 0; c < currentEnemyCols; c++)
            if (enemyAlive[r][c]) return false;
    return true;
}


void UpdateExplosions()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosions[i].active) {
            explosions[i].scale += 0.003f;
            explosions[i].frames--;
            if (explosions[i].frames <= 0) explosions[i].active = false;
        }
    }
}

void DrawExplosions()
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosions[i].active) {
            Vector2 pos = { explosions[i].x - (blast.width * explosions[i].scale) / 2,
                            explosions[i].y - (blast.height * explosions[i].scale) / 2 };
            DrawTextureEx(blast, pos, 0.0f, explosions[i].scale, WHITE);
        }
    }
}

void CheckBulletEnemyCollision(float bulletScale, float enemyScale, bool level2)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            Rectangle bulletRec = { bullets[i].x, bullets[i].y,
                                    bullet.width * bulletScale, bullet.height * bulletScale };
            for (int row = 0; row < currentEnemyRows; row++)
            {
                for (int col = 0; col < currentEnemyCols; col++)
                {
                    if (enemyAlive[row][col])
                    {
                        Rectangle enemyRec = { enemyX[row][col], enemyY[row][col],
                                               currentEnemyTexture.width * enemyScale, currentEnemyTexture.height * enemyScale };
                        if (CheckCollisionRecs(bulletRec, enemyRec))
                        {
                            bullets[i].active = false;
                            enemyAlive[row][col] = false;
                            score++;

                            for (int e = 0; e < MAX_EXPLOSIONS; e++)
                            {
                                if (!explosions[e].active)
                                {
                                    explosions[e].active = true;
                                    explosions[e].x = enemyX[row][col] + (currentEnemyTexture.width * enemyScale) / 2;
                                    explosions[e].y = enemyY[row][col] + (currentEnemyTexture.height * enemyScale) / 2;
                                    explosions[e].scale = 0.1f;
                                    explosions[e].frames = 30;
                                    break;
                                }
                            }

                            if (!powerActive && rand() % (level2 ? 2 : 4) == 0)
                            {
                                for (int t = 0; t < MAX_TOKENS; t++)
                                {
                                    if (!tokens[t].active)
                                    {
                                        tokens[t].active = true;
                                        tokens[t].collected = false;
                                        tokens[t].x = enemyX[row][col] + currentEnemyTexture.width * enemyScale / 2;
                                        tokens[t].y = enemyY[row][col] + currentEnemyTexture.height * enemyScale / 2;
                                        tokens[t].speed = 4 + rand() % 3;

                                        tokens[t].type = 1 + rand() % 4; 
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void CheckShipEnemyCollision(int shipX, int shipY, float shipScale, float enemyScale)
{
    if (!shipAlive) return;
    Rectangle shipRec = { (float)shipX, (float)shipY, spaceship.width * shipScale, spaceship.height * shipScale };
    for (int row = 0; row < currentEnemyRows; row++)
    {
        for (int col = 0; col < currentEnemyCols; col++)
        {
            if (enemyAlive[row][col]) {
                Rectangle enemyRec = { enemyX[row][col], enemyY[row][col], currentEnemyTexture.width * enemyScale, currentEnemyTexture.height * enemyScale };
                if (CheckCollisionRecs(shipRec, enemyRec))
                {
                    if (CheckCollisionRecs(shipRec, enemyRec))
                    {
                        if (shieldTimer > 0)
                        {
                            cout << " Shield active - boss collision absorbed, ship safe!\n";
                        }
                        else
                        {
                            cout << " No shield! Ship died on boss collision!\n";
                            shipAlive = false; 
                        }
                    }

                    else
                    {
                        shipAlive = false;  
                        for (int e = 0; e < MAX_EXPLOSIONS; e++)
                        {
                            if (!explosions[e].active)
                            {
                                explosions[e].active = true;
                                explosions[e].x = shipX + spaceship.width * shipScale / 2;
                                explosions[e].y = shipY + spaceship.height * shipScale / 2;
                                explosions[e].scale = 0.1f;
                                explosions[e].frames = 30;
                                break;
                            }
                        }
                    }
                }

            }
        }
    }
}

void UpdateTokens()
{
    for (int i = 0; i < MAX_TOKENS; i++)
    {
        if (tokens[i].active) {
            tokens[i].y += tokens[i].speed;
            if (tokens[i].y > screenHeight) tokens[i].active = false;
        }
    }
}
void DrawTokens()
{
    float tokenScale = 0.005f; 
    for (int i = 0; i < MAX_TOKENS; i++)
    {
        if (tokens[i].active)
        {
            DrawTextureEx(
                token,
                { tokens[i].x - (token.width * tokenScale) / 2.0f,
                  tokens[i].y - (token.height * tokenScale) / 2.0f },
                0.0f,
                tokenScale,
                WHITE
            );
        }
    }
}

void CheckTokenCatch(int shipX, int shipY, float shipScale)
{
    Rectangle shipRec = { (float)shipX, (float)shipY, spaceship.width * shipScale, spaceship.height * shipScale };
    for (int i = 0; i < MAX_TOKENS; i++)
    {
        if (tokens[i].active && !tokens[i].collected)
        {
            Rectangle tokenRec = { tokens[i].x - 15, tokens[i].y - 15, 30, 30 };
            if (CheckCollisionRecs(shipRec, tokenRec))
            {
                tokens[i].collected = true;
                tokens[i].active = false;

                switch (tokens[i].type)
                {
                case 1: multiShotTimer = 60 * 5; multiShot = 60 * 5; break; // 5 sec
                case 2: shieldTimer = 60 * 5; shield = 60 * 5; break;
                case 3: speedBoostTimer = 60 * 5; speedBoost = 60 * 5; break;
                case 4: slowDownTimer = 60 * 5; break; // disadvantage
                }
                powerActive = true;


            }
        }
    }
}

void UpdateBossAndFire()
{
    if (bossAlive)
    {
        if (bossY < screenHeight / 4)
        {
            bossY += 1.5f; 
        }
    }

    static float bossFireCooldown = 0;
    bossFireCooldown -= 1.0f / 60.0f;

    if (bossFireCooldown <= 0 && bossAlive)
    {
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        {
            if (!enemyBullets[i].active)
            {
                enemyBullets[i].active = true;

                float bx = bossX + (boss.width * bossScale) / 2;
                float by = bossY + (boss.height * bossScale);

                enemyBullets[i].x = bx;
                enemyBullets[i].y = by;

                float tx = shipX + (spaceship.width * shipScale) / 2;
                float ty = shipY + (spaceship.height * shipScale) / 2;

                float vx = tx - enemyBullets[i].x;
                float vy = ty - enemyBullets[i].y;
                float L = sqrt(vx * vx + vy * vy);

                if (L != 0)
                {
                    enemyBullets[i].dx = vx / L;
                    enemyBullets[i].dy = vy / L;
                }
                else
                {
                    enemyBullets[i].dx = 0;
                    enemyBullets[i].dy = 1;
                }

                enemyBullets[i].speed = 4.0f; 
                break;
            }
        }
        bossFireCooldown = 0.4f; 
    }
}

void UpdateEnemyBulletsUnified(float speed)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemyBullets[i].active)
        {
            if (!level5)
            {
                enemyBullets[i].y += speed; 
            }
            else if (level5 && bossAlive)
            {
                enemyBullets[i].x += enemyBullets[i].dx * enemyBullets[i].speed;
                enemyBullets[i].y += enemyBullets[i].dy * enemyBullets[i].speed;
            }

            if (enemyBullets[i].y > screenHeight || enemyBullets[i].x > screenWidth || enemyBullets[i].x < 0)
                enemyBullets[i].active = false;

            Rectangle shipRec = { (float)shipX, (float)shipY,
                                  spaceship.width * shipScale, spaceship.height * shipScale };
            Rectangle bulletRec = { enemyBullets[i].x, enemyBullets[i].y,
                                    bullet.width * 0.1f, bullet.height * 0.1f };
            if (shipAlive && CheckCollisionRecs(shipRec, bulletRec))
            {
                enemyBullets[i].active = false;
                if (shield <= 0) shipHealth--;
                if (shipHealth <= 0) shipAlive = false;
            }
        }
    }
}



void ShowLevelStartScreen(int level)
{
    float timer = 0.0f;
    while (timer < 3.0f)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("LEVEL %d", level), screenWidth / 2 - 100, screenHeight / 2 - 25, 50, WHITE);
        EndDrawing();
        timer += 1.0f / 60.0f;
    }
}

void ShowLevelCompleteScreen(int level)
{
    float timer = 0.0f;

    while (timer < 2.0f)
    {  
        BeginDrawing();

        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

        DrawText(
            TextFormat("LEVEL %d COMPLETED!", level),
            screenWidth / 2 - MeasureText("LEVEL X COMPLETED!", 50) / 2,
            screenHeight / 2 - 25,
            50,
            YELLOW
        );

        EndDrawing();

        timer += 1.0f / 60.0f;
    }
}


int ShowMainMenu()
{
    int selectedOption = 0; 
    const char* options[] = { "Start", "Settings", "Exit" };
    const int optionCount = 3;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_DOWN))
            selectedOption = (selectedOption + 1) % optionCount;
        if (IsKeyPressed(KEY_UP))
            selectedOption = (selectedOption - 1 + optionCount) % optionCount;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            return selectedOption; 
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("SPACE GAME", screenWidth / 2 - 150, 150, 80, WHITE);

        for (int i = 0; i < optionCount; i++)
        {
            Color col = (i == selectedOption) ? YELLOW : WHITE;
            DrawText(options[i], screenWidth / 2 - 60, 350 + i * 80, 50, col);
        }

        DrawText("Use UP/DOWN arrows and ENTER to select", screenWidth / 2 - 250, 600, 20, LIGHTGRAY);

        EndDrawing();
    }

    return 2; 
}

void ShowWinScreen()
{
    float timer = 0.0f;

    while (timer < 5.0f) 
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("CONGRATULATIONS!", screenWidth / 2 - 250, screenHeight / 2 - 50, 60, GREEN);
        DrawText("YOU DEFEATED THE BOSS!", screenWidth / 2 - 270, screenHeight / 2 + 30, 40, WHITE);
        DrawText("Press ESC to exit", screenWidth / 2 - 120, screenHeight / 2 + 100, 30, LIGHTGRAY);

        EndDrawing();

        timer += 1.0f / 60.0f;

        if (IsKeyPressed(KEY_ESCAPE))
            break;
    }
}


// -------------------- Main --------------------
int main()
{
    InitWindow(screenWidth, screenHeight, "Space Game");
    int menuChoice = ShowMainMenu();
    InitAudioDevice();
    Music bgMusic = LoadMusicStream("C:\\spaceship\\ConsoleApplication2\\Sounds\\backgroundmusic.ogg");
    PlayMusicStream(bgMusic);

    if (menuChoice == 2) 
    {
        CloseWindow();
        return 0;
    }
    else if (menuChoice == 1) 
    {
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Settings coming soon!", screenWidth / 2 - 150, screenHeight / 2, 50, WHITE);
            DrawText("Press ESC to go back", screenWidth / 2 - 150, screenHeight / 2 + 100, 30, LIGHTGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ESCAPE))
            {
                menuChoice = ShowMainMenu(); 
                if (menuChoice == 2) { CloseWindow(); return 0; }
                else if (menuChoice == 0) break; 
            }
        }
    }

    SetTargetFPS(60);

    srand(time(NULL));
    InitStars();
    LoadTextures();
    InitEnemies(2, 12, 60); // Level 1


    bool gameOverPrinted = false;

    cout << "Level 1" << endl;
    ShowLevelStartScreen(1);

    int level1ScoreThreshold = 24;   
    int level2ScoreThreshold = 24 + 42; 
    int level3ScoreThreshold = 24 + 42 + 64; 
    int level4ScoreThreshold = 24 + 42 + 64 + 90;

    while (!WindowShouldClose())
    {

        UpdateMusicStream(bgMusic);
        // Toggle pause
        if (IsKeyPressed(KEY_P))
        {
            gamePaused = !gamePaused;
        }

        if (!gamePaused)
        {
            UpdateStars();
            MoveSpaceship(shipX, shipY, shipSpeed, shipScale);

            if (fireCooldown > 0)
                fireCooldown -= 1.0f / 60.0f;  

            if (IsKeyDown(KEY_SPACE) && fireCooldown <= 0)
            {
                ShootBullet(shipX, shipY, shipScale, bulletScale);
                fireCooldown = 0.15f;  
            }
            UpdateBossAndFire();

            UpdateEnemyBulletsUnified(bulletSpeed);


            UpdateBullets(bulletSpeed);
            EnemyShoot();
            UpdateEnemyBullets(level2 ? 7.0f : 5.0f, shipX, shipY, shipScale);

            CheckBulletEnemyCollision(bulletScale, enemyScale, level2);
            CheckShipEnemyCollision(shipX, shipY, shipScale, enemyScale);

            if (level5 && bossAlive)
            {
                bossX += bossSpeedX;
                bossY += bossSpeedY;

                if (bossX > screenWidth + 60)
                    bossX = -60;

                if (bossX < -60)
                    bossX = screenWidth + 60;

                if (bossY > screenHeight / 2)
                    bossSpeedY *= -1;

                if (bossY < 50)
                    bossSpeedY *= -1;
            }

            UpdateEnemies();
            UpdateExplosions();
            UpdateTokens();
            CheckTokenCatch(shipX, shipY, shipScale);



            if (multiShot > 0) multiShot--;
            if (shield > 0) shield--;
            if (speedBoost > 0) speedBoost--;

            if (multiShotTimer > 0) { multiShotTimer--; }
            if (shieldTimer > 0) { shieldTimer--; }
            if (speedBoostTimer > 0) { speedBoostTimer--; }
            if (slowDownTimer > 0) { slowDownTimer--; }

            if (multiShotTimer + shieldTimer + speedBoostTimer + slowDownTimer <= 0) {
                powerActive = false;
            }

            if (speedBoost <= 0) {
                boosterOffsetX = 0;
                boosterOffsetY = 0;
            }

            if (slowDownTimer > 0) slowDownTimer--;

            if (speedBoost > 0)
            {
                boosterTimer += 1.0f / 60.0f; 

                boosterOffsetX = sin(boosterTimer * 20.0f) * 5.0f; 

                boosterOffsetY = sin(boosterTimer * 35.0f) * 3.0f; 

                float scaleFlicker = 0.25f + ((rand() % 10) / 100.0f); 
            }

        }

        if (!level2 && score >= level1ScoreThreshold) {
            level2 = true;
            level2Started = false;
        }

        if (level2 && !level2Started) {
            ShowLevelCompleteScreen(1);
            InitEnemies(3, 14, 20);  // 3 rows
            level2Started = true;

            multiShotTimer = 0;
            shieldTimer = 0;
            speedBoostTimer = 0;
            slowDownTimer = 0;

            multiShot = 0;
            shield = 0;
            speedBoost = 0;

            powerActive = false;

            shipHealth = 6;
            shipAlive = true;

            spaceship = spaceship2;
            currentEnemyTexture = enemy2;
            bullet = bullet2;
            shipScale = 0.25f;

            ShowLevelStartScreen(2);
        }

        if (!level3 && score >= level2ScoreThreshold) {
            level3 = true;
            level3Started = false;
        }

        if (level3 && AllEnemiesDead() && !level3Started) {
            ShowLevelCompleteScreen(2);
            InitEnemies(4, 16, 10);
            level3Started = true;

            multiShotTimer = 0;
            shieldTimer = 0;
            speedBoostTimer = 0;
            slowDownTimer = 0;

            multiShot = 0;
            shield = 0;
            speedBoost = 0;

            powerActive = false;

            shipHealth = 7;
            starSpeedMultiplier = 1.8f;
            shipAlive = true;


            spaceship = spaceship3;
            currentEnemyTexture = enemy3;
            bullet = bullet3;

            ShowLevelStartScreen(3);
        }

        if (!level4 && AllEnemiesDead() && score >= level3ScoreThreshold) {
            level4 = true;
            level4Started = false;
        }

        if (level4 && !level4Started) {
            ShowLevelCompleteScreen(3);
            InitEnemies(5, 18, 10);
            level4Started = true;
            multiShotTimer = 0;
            shieldTimer = 0;
            speedBoostTimer = 0;
            slowDownTimer = 0;

            multiShot = 0;
            shield = 0;
            speedBoost = 0;

            powerActive = false;

            shipHealth = 8;
            shipAlive = true;

            spaceship = spaceship4;
            currentEnemyTexture = enemy4;
            bullet = bullet4;

            shipScale = 0.22f;
            enemyScale = 0.25f;
            bulletScale = 0.12f;

            bulletSpeed = 12.0f;       
            starSpeedMultiplier = 1.4f;


            ShowLevelStartScreen(4);
        }

        if (!level5 && AllEnemiesDead() && level4) {
            level5 = true;
            level5Started = false;
        }

        if (level5 && !level5Started) {
            ShowLevelCompleteScreen(4);

           
            bossScale = 0.5f;         
            bossX = screenWidth / 2 - (boss.width * bossScale) / 2; 
            bossY = -boss.height * bossScale;                 
            bossAlive = true;


            for (int r = 0; r < MAX_ROWS; r++)
                for (int c = 0; c < MAX_COLS; c++)
                    enemyAlive[r][c] = false;

            multiShotTimer = 0;
            shieldTimer = 0;
            speedBoostTimer = 0;
            slowDownTimer = 0;

            multiShot = 0;
            shield = 0;
            speedBoost = 0;

            powerActive = false;

            shipHealth = 15;
            shipAlive = true;
            shipScale = 0.30f;
            enemyScale = 0.22f;
            bulletScale = 0.18f;
            shipSpeed = 9.5f;

            spaceship = spaceship5;
            currentEnemyTexture = enemy5; 
            bullet = bullet5;
            starSpeedMultiplier = 1.6f;

            cout << "Level 5 Boss Activated!" << endl;
            ShowLevelStartScreen(5);

            level5Started = true;
        }




        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active && bossAlive) {
                Rectangle bulletRec = { bullets[i].x, bullets[i].y,
                                        bullet.width * bulletScale, bullet.height * bulletScale };
                Rectangle bossRec = { bossX, bossY, boss.width * bossScale, boss.height * bossScale };

                if (CheckCollisionRecs(bulletRec, bossRec)) {
                    bullets[i].active = false;
                    bossHealth -= 4;
                    cout << "Boss Hit! Health: " << bossHealth << endl;

                    if (bossHealth <= 0 && level5) {
                        bossAlive = false;
                        cout << " BOSS DEFEATED! YOU WIN!" << endl;
                        ShowWinScreen();
                        CloseWindow(); 
                        return 0;      
                    }

                }
            }
        }


        if (level5 && bossAlive && shipAlive) {
            Rectangle shipRec = { (float)shipX, (float)shipY,
                                  spaceship.width * shipScale, spaceship.height * shipScale };
            Rectangle bossRec = { bossX, bossY, boss.width * bossScale, boss.height * bossScale };

            if (CheckCollisionRecs(shipRec, bossRec)) {
                if (shield > 0) {
                    shield = 0; 
                    shieldTimer = 0;
                    cout << " Shield broke saving ship!" << endl;
                }
                else {
                    shipAlive = false;
                    cout << " Boss collision! Ship destroyed ❌" << endl;
                }
            }
        }


        BeginDrawing();
        ClearBackground(BLACK);

        DrawStars();
        DrawEnemies(enemyScale);
        DrawExplosions();
        DrawTokens();
        DrawSpaceship(shipX, shipY, shipScale);

        if (level5 && bossAlive)
        {
            DrawTextureEx(boss, { bossX, bossY }, 0.0f, bossScale, WHITE);
        }


        if (gamePaused) {
            DrawText("PAUSED", screenWidth / 2 - 100, screenHeight / 2 - 50, 60, YELLOW);
        }

        if (level5 && bossAlive)
        {
            float barWidth = 400;
            float healthPercent = (float)bossHealth / (float)bossMaxHealth; 

            DrawRectangle(screenWidth / 2 - barWidth / 2, 20, barWidth, 18, GRAY);
            DrawRectangle(screenWidth / 2 - barWidth / 2, 20, barWidth * healthPercent, 18, RED);
            DrawText("BOSS", screenWidth / 2 - 40, 45, 30, YELLOW);
        }



        if (shieldTimer > 0)
        {
            DrawCircle(
                shipX + spaceship.width * shipScale / 2,
                shipY + spaceship.height * shipScale / 2,
                spaceship.width * shipScale,
                Fade(BLUE, 0.4f)  
            );
        }

        if (slowDownTimer > 0)
        {
            DrawRectangle(shipX, shipY, spaceship.width * shipScale, spaceship.height * shipScale, Fade(RED, 0.3f));
        }


        DrawBullets(bulletScale);
        DrawEnemyBullets(0.1f);

        for (int i = 0; i < shipHealth; i++)
            DrawRectangle(20 + i * 35, 20, 30, 30, GREEN);

        DrawText(TextFormat("Score: %d", score), screenWidth - 200, 20, 30, WHITE);

        if (!shipAlive) {
            DrawText("GAME OVER", screenWidth / 2 - 150, screenHeight / 2, 50, RED);
            EndDrawing();
            continue; 
        }


        EndDrawing();
    }


    // Unload textures
    UnloadTexture(enemy);
    UnloadTexture(spaceship);
    UnloadTexture(bullet);
    UnloadTexture(blast);
    UnloadTexture(spaceship2);
    UnloadTexture(enemy2);
    UnloadTexture(bullet2);
    UnloadTexture(spaceship3);
    UnloadTexture(enemy3);
    UnloadTexture(bullet3);
    UnloadTexture(spaceship4);
    UnloadTexture(enemy4);
    UnloadTexture(bullet4);
    UnloadTexture(spaceship5);
    UnloadTexture(enemy5);
    UnloadTexture(bullet5);
    UnloadTexture(token);
    UnloadTexture(booster);
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();


    CloseWindow();
    return 0;
}
