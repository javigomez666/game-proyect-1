#include "raylib.h"
#include <cmath>

const int MAX_BULLETS = 100;
const int MAX_ENEMIES = 50;

struct Bullet {
    float x, y;
    float dx, dy;
    bool active;
};

struct Enemy {
    float x, y;
    bool active;
};

int main() {
    InitWindow(800, 600, "Time Survival -  C++");
    SetTargetFPS(60);

    // Player
    float px = 400, py = 300;

    // Game state
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];

    float survivalTime = 0;
    float bestTime = 0;

    float spawnTimer = 0;
    float spawnInterval = 1.5f;
    float enemySpeed = 1.2f;

    bool gameOver = false;

    // Init arrayss
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        if (!gameOver) {

            // TIME
            survivalTime += dt;

            // DIFFICULTY
            if (spawnInterval > 0.4f) spawnInterval -= 0.050f * dt;
            if (enemySpeed < 5.0f) enemySpeed += 0.050f * dt;

            // PLAYER MOVEMENT
            if (IsKeyDown(KEY_W)) py -= 4;
            if (IsKeyDown(KEY_S)) py += 4;
            if (IsKeyDown(KEY_A)) px -= 4;
            if (IsKeyDown(KEY_D)) px += 4;

            // SHOOT
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                Vector2 mouse = GetMousePosition();

                float dx = mouse.x - px;
                float dy = mouse.y - py;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist > 0) {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].x = px;
                            bullets[i].y = py;
                            bullets[i].dx = (dx / dist) * 7;
                            bullets[i].dy = (dy / dist) * 7;
                            break;
                        }
                    }
                }
            }

            // UPDATE BULLETS
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].x += bullets[i].dx;
                    bullets[i].y += bullets[i].dy;
                }
            }

            // SPAWN ENEMIES
            spawnTimer += dt;

            if (spawnTimer >= spawnInterval) {
                spawnTimer = 0;

                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].active = true;

                        enemies[i].x = (GetRandomValue(0, 1) == 0) ? -20 : 820;
                        enemies[i].y = GetRandomValue(0, 600);

                        break;
                    }
                }
            }

            // UPDATE ENEMIES
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {

                    float dx = px - enemies[i].x;
                    float dy = py - enemies[i].y;
                    float dist = sqrt(dx * dx + dy * dy);

                    if (dist > 0) {
                        enemies[i].x += (dx / dist) * enemySpeed;
                        enemies[i].y += (dy / dist) * enemySpeed;
                    }

                    // PLAYER COLLISION
                    if (CheckCollisionCircles({px, py}, 20, {enemies[i].x, enemies[i].y}, 15)) {
                        gameOver = true;

                        if (survivalTime > bestTime)
                            bestTime = survivalTime;
                    }
                }
            }

            // BULLET vs ENEMY
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;

                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active) {

                        if (CheckCollisionCircles(
                            {bullets[i].x, bullets[i].y}, 5,
                            {enemies[j].x, enemies[j].y}, 15
                        )) {
                            bullets[i].active = false;
                            enemies[j].active = false;
                        }
                    }
                }
            }
        }

        // RESET
        else {
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                survivalTime = 0;
                px = 400;
                py = 300;
                spawnInterval = 1.5f;
                enemySpeed = 1.2f;

                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            }
        }

        // DRAW
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {

            DrawCircle(px, py, 20, BLUE);

            for (int i = 0; i < MAX_BULLETS; i++)
                if (bullets[i].active)
                    DrawCircle(bullets[i].x, bullets[i].y, 5, BLACK);

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (enemies[i].active)
                    DrawCircle(enemies[i].x, enemies[i].y, 15, RED);

            DrawText(TextFormat("TIME: %.2f", survivalTime), 10, 10, 20, DARKGRAY);
        }
        else {
            DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.8f));

            DrawText("YOU DIED", 260, 200, 50, RED);
            DrawText(TextFormat("TIME: %.2f", survivalTime), 260, 270, 20, WHITE);
            DrawText(TextFormat("BEST: %.2f", bestTime), 260, 300, 20, YELLOW);
            DrawText("PRESS ENTER", 260, 350, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}