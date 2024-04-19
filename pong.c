#include "raylib.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 60
#define BALL_SIZE 10
#define PADDLE_SPEED 5
#define BALL_SPEED 3
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
    HELP_SCREEN,
    GAME_SCREEN,
    END_SCREEN
} GameState;

void startScreen(GameState *state);
void helpScreen(GameState *state);
void gameScreen(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, bool *gameOver, int *winner);
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
            *state = HELP_SCREEN;
        }
    }
}

void helpScreen(GameState *state) {
    DrawText("Controls", WINDOW_WIDTH / 2 - MeasureText("Controls", 30) / 2, WINDOW_HEIGHT / 4, 30, WHITE);
    DrawText("Left Paddle: W/S", WINDOW_WIDTH / 2 - MeasureText("Left Paddle: W/S", 20) / 2, WINDOW_HEIGHT / 2 - 20, 20, WHITE);
    DrawText("Right Paddle: Up/Down", WINDOW_WIDTH / 2 - MeasureText("Right Paddle: Up/Down", 20) / 2, WINDOW_HEIGHT / 2 + 20, 20, WHITE);
    DrawText("Press SPACE to return", WINDOW_WIDTH / 2 - MeasureText("Press SPACE to return", 20) / 2, WINDOW_HEIGHT - 40, 20, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        *state = START_SCREEN;
    }
}

void gameScreen(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, bool *gameOver, int *winner) {
    if (IsKeyDown(KEY_W) && leftPaddle->position.y > 0)
        leftPaddle->position.y -= PADDLE_SPEED;
    if (IsKeyDown(KEY_S) && leftPaddle->position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
        leftPaddle->position.y += PADDLE_SPEED;
    if (IsKeyDown(KEY_UP) && rightPaddle->position.y > 0)
        rightPaddle->position.y -= PADDLE_SPEED;
    if (IsKeyDown(KEY_DOWN) && rightPaddle->position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
        rightPaddle->position.y += PADDLE_SPEED;

    ball->position.x += ball->speed.x;
    ball->position.y += ball->speed.y;

    if (ball->position.y < 0 || ball->position.y > WINDOW_HEIGHT - BALL_SIZE)
        ball->speed.y = -ball->speed.y;

    if (CheckCollisionRecs((Rectangle){ leftPaddle->position.x, leftPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                            (Rectangle){ ball->position.x, ball->position.y, BALL_SIZE, BALL_SIZE }))
        ball->speed.x = -ball->speed.x;
    if (CheckCollisionRecs((Rectangle){ rightPaddle->position.x, rightPaddle->position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                            (Rectangle){ ball->position.x, ball->position.y, BALL_SIZE, BALL_SIZE }))
        ball->speed.x = -ball->speed.x;

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

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (state) {
            case START_SCREEN:
                startScreen(&state);
                break;
            case HELP_SCREEN:
                helpScreen(&state);
                break;
            case GAME_SCREEN:
                gameScreen(&leftPaddle, &rightPaddle, &ball, &gameOver, &winner);
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