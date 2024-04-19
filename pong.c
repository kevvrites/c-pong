#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 60
#define BALL_SIZE 10
#define PADDLE_SPEED 5
#define BALL_SPEED 3
#define MAX_BALL_SPEED 7
#define WIN_CON 5

typedef struct {
    Vector2 position;
    int score;
} Paddle;

typedef struct {
    Vector2 position;
    Vector2 speed;
} Ball;

typedef enum {
    START_SCREEN,
    CONTROLS_SCREEN,
    GAME_SCREEN,
    END_SCREEN
} GameState;

void startScreen(GameState *state);
void controlsScreen(GameState *state, int *p1Up, int *p1Down, int *p2Up, int *p2Down);
void gameScreen(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, bool *gameOver, int *winner, int p1Up, int p1Down, int p2Up, int p2Down);
void endScreen(GameState *state, int winner, int leftScore, int rightScore, bool *gameOver, Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball);

void startScreen(GameState *state) {
    DrawText("PONG", WINDOW_WIDTH / 2 - MeasureText("PONG", 40) / 2, WINDOW_HEIGHT / 4, 40, WHITE);
    DrawText("Press SPACE to begin", WINDOW_WIDTH / 2 - MeasureText("Press SPACE to begin", 20) / 2, WINDOW_HEIGHT / 2, 20, WHITE);
    
    DrawRectangle(WINDOW_WIDTH - 40, 13, 30, 30, WHITE);
    DrawRectangle(WINDOW_WIDTH - 38, 16, 26, 26, BLACK);
    DrawText("?", WINDOW_WIDTH - 30, 20, 20, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        *state = GAME_SCREEN;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= WINDOW_WIDTH - 30 && mousePos.x <= WINDOW_WIDTH - 10 && mousePos.y >= 20 && mousePos.y <= 40) {
            *state = CONTROLS_SCREEN;
        }
    }
}

void controlsScreen(GameState *state, int *p1Up, int *p1Down, int *p2Up, int *p2Down) {
    DrawText("Remap Controls", WINDOW_WIDTH / 2 - MeasureText("Remap Controls", 30) / 2, WINDOW_HEIGHT / 4, 30, WHITE);
    DrawText("Press the key for each control", WINDOW_WIDTH / 2 - MeasureText("Press the key for each control", 20) / 2, WINDOW_HEIGHT / 2 - 60, 20, WHITE);
    
    DrawText("P1 Up:", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, 20, WHITE);
    DrawText("P1 Down:", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 20, 20, WHITE);
    DrawText("P2 Up:", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 60, 20, WHITE);
    DrawText("P2 Down:", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 100, 20, WHITE);
    
    static int selectedControl = -1;
    
    for (int i = 0; i < 4; i++) {
        int key = (i == 0) ? *p1Up : (i == 1) ? *p1Down : (i == 2) ? *p2Up : *p2Down;
        const char* keyText = (key == KEY_UP) ? "UP" : (key == KEY_DOWN) ? "DOWN" : TextFormat("%c", key);
        
        if (i == selectedControl) {
            DrawText(keyText, WINDOW_WIDTH / 2 + 50, WINDOW_HEIGHT / 2 - 20 + i * 40, 20, WHITE);
        } else {
            DrawRectangleLines(WINDOW_WIDTH / 2 + 45, WINDOW_HEIGHT / 2 - 25 + i * 40, 80, 30, WHITE);
            DrawText(keyText, WINDOW_WIDTH / 2 + 50, WINDOW_HEIGHT / 2 - 20 + i * 40, 20, WHITE);
        }
    }
    
    DrawText("Press SPACE to return", WINDOW_WIDTH / 2 - MeasureText("Press SPACE to return", 20) / 2, WINDOW_HEIGHT - 40, 20, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (GetMouseX() >= WINDOW_WIDTH / 2 + 45 && GetMouseX() <= WINDOW_WIDTH / 2 + 75) {
            if (GetMouseY() >= WINDOW_HEIGHT / 2 - 25 && GetMouseY() <= WINDOW_HEIGHT / 2 + 5)
                selectedControl = 0;
            else if (GetMouseY() >= WINDOW_HEIGHT / 2 + 15 && GetMouseY() <= WINDOW_HEIGHT / 2 + 45)
                selectedControl = 1;
            else if (GetMouseY() >= WINDOW_HEIGHT / 2 + 55 && GetMouseY() <= WINDOW_HEIGHT / 2 + 85)
                selectedControl = 2;
            else if (GetMouseY() >= WINDOW_HEIGHT / 2 + 95 && GetMouseY() <= WINDOW_HEIGHT / 2 + 125)
                selectedControl = 3;
        }
    }
    
    if (selectedControl >= 0) {
        int key = GetKeyPressed();
        if (key > 0) {
            if (selectedControl == 0)
                *p1Up = key;
            else if (selectedControl == 1)
                *p1Down = key;
            else if (selectedControl == 2)
                *p2Up = key;
            else if (selectedControl == 3)
                *p2Down = key;
            
            selectedControl = -1;
        }
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        *state = START_SCREEN;
    }
}

void gameScreen(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, bool *gameOver, int *winner, int p1Up, int p1Down, int p2Up, int p2Down) {
    if (IsKeyDown(p1Up) && leftPaddle->position.y > 0)
        leftPaddle->position.y -= PADDLE_SPEED;
    if (IsKeyDown(p1Down) && leftPaddle->position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
        leftPaddle->position.y += PADDLE_SPEED;
    if (IsKeyDown(p2Up) && rightPaddle->position.y > 0)
        rightPaddle->position.y -= PADDLE_SPEED;
    if (IsKeyDown(p2Down) && rightPaddle->position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
        rightPaddle->position.y += PADDLE_SPEED;

    ball->position.x += ball->speed.x;
    ball->position.y += ball->speed.y;

    if (ball->position.y < 0 || ball->position.y > WINDOW_HEIGHT - BALL_SIZE)
        ball->speed.y = -ball->speed.y;

    if (CheckCollisionRecs((Rectangle){ leftPaddle->position.x, leftPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                            (Rectangle){ ball->position.x, ball->position.y, BALL_SIZE, BALL_SIZE })) {
        ball->speed.x = -ball->speed.x;
        ball->position.x = leftPaddle->position.x + PADDLE_WIDTH;
        if (fabsf(ball->speed.x) < MAX_BALL_SPEED)
            ball->speed.x *= 1.1f;
    }
    if (CheckCollisionRecs((Rectangle){ rightPaddle->position.x, rightPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                            (Rectangle){ ball->position.x, ball->position.y, BALL_SIZE, BALL_SIZE })) {
        ball->speed.x = -ball->speed.x;
        ball->position.x = rightPaddle->position.x - BALL_SIZE;
        if (fabsf(ball->speed.x) < MAX_BALL_SPEED)
            ball->speed.x *= 1.1f;
    }

    if (ball->position.x < 0) {
        rightPaddle->score++;
        ball->position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
        ball->speed = (Vector2){ BALL_SPEED, BALL_SPEED };
    }
    if (ball->position.x > WINDOW_WIDTH) {
        leftPaddle->score++;
        ball->position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
        ball->speed = (Vector2){ -BALL_SPEED, BALL_SPEED };
    }

    if (leftPaddle->score == WIN_CON) {
        *gameOver = true;
        *winner = 1;
    }
    else if (rightPaddle->score == WIN_CON) {
        *gameOver = true;
        *winner = 2;
    }

    DrawRectangle(leftPaddle->position.x, leftPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    DrawRectangle(rightPaddle->position.x, rightPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    DrawRectangle(ball->position.x, ball->position.y, BALL_SIZE, BALL_SIZE, WHITE);
    DrawText(TextFormat("%d", leftPaddle->score), WINDOW_WIDTH / 4, 20, 20, WHITE);
    DrawText(TextFormat("%d", rightPaddle->score), 3 * WINDOW_WIDTH / 4, 20, 20, WHITE);
}

void endScreen(GameState *state, int winner, int leftScore, int rightScore, bool *gameOver, Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball) {
    *gameOver = false;

    const char* winnerText = winner == 1 ? "P1 WINS!" : "P2 WINS!";
    const char* scoreText = TextFormat("%d-%d", leftScore, rightScore);

    DrawText(winnerText, WINDOW_WIDTH / 2 - MeasureText(winnerText, 40) / 2, WINDOW_HEIGHT / 4, 40, WHITE);
    DrawText(scoreText, WINDOW_WIDTH / 2 - MeasureText(scoreText, 30) / 2, WINDOW_HEIGHT / 2 - 15, 30, WHITE);
    DrawText("Press SPACE to play again", WINDOW_WIDTH / 2 - MeasureText("Press SPACE to play again", 20) / 2, WINDOW_HEIGHT / 2 + 100, 20, WHITE);
    DrawText("Press S to go to the start menu", WINDOW_WIDTH / 2 - MeasureText("Press S to go to the start menu", 20) / 2, WINDOW_HEIGHT / 2 + 130, 20, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        *state = GAME_SCREEN;
        leftPaddle->score = 0;
        rightPaddle->score = 0;
        ball->position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
        ball->speed = (Vector2){ BALL_SPEED, BALL_SPEED };
    }

    if (IsKeyPressed(KEY_S)) {
        *state = START_SCREEN;
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG");
    SetTargetFPS(60);

    GameState state = START_SCREEN;

    Paddle leftPaddle = { 0 };
    leftPaddle.position = (Vector2){ 20, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2 };

    Paddle rightPaddle = { 0 };
    rightPaddle.position = (Vector2){ WINDOW_WIDTH - 20 - PADDLE_WIDTH, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2 };

    Ball ball = { 0 };
    ball.position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
    ball.speed = (Vector2){ BALL_SPEED, BALL_SPEED };

    bool gameOver = false;
    int winner = 0;
    
    int p1Up = KEY_W;
    int p1Down = KEY_S;
    int p2Up = KEY_UP;
    int p2Down = KEY_DOWN;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (state) {
            case START_SCREEN:
                startScreen(&state);
                break;
            case CONTROLS_SCREEN:
                controlsScreen(&state, &p1Up, &p1Down, &p2Up, &p2Down);
                break;
            case GAME_SCREEN:
                gameScreen(&leftPaddle, &rightPaddle, &ball, &gameOver, &winner, p1Up, p1Down, p2Up, p2Down);
                if (gameOver) {
                    state = END_SCREEN;
                }
                break;
            case END_SCREEN:
                endScreen(&state, winner, leftPaddle.score, rightPaddle.score, &gameOver, &leftPaddle, &rightPaddle, &ball);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}