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

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG");
    SetTargetFPS(60);

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
        if (!gameOver) {
            // Move the paddles
            if (IsKeyDown(KEY_W) && leftPaddle.position.y > 0)
                leftPaddle.position.y -= PADDLE_SPEED;
            if (IsKeyDown(KEY_S) && leftPaddle.position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
                leftPaddle.position.y += PADDLE_SPEED;
            if (IsKeyDown(KEY_UP) && rightPaddle.position.y > 0)
                rightPaddle.position.y -= PADDLE_SPEED;
            if (IsKeyDown(KEY_DOWN) && rightPaddle.position.y < WINDOW_HEIGHT - PADDLE_HEIGHT)
                rightPaddle.position.y += PADDLE_SPEED;

            // Move the ball
            ball.position.x += ball.speed.x;
            ball.position.y += ball.speed.y;

            // Check for collision with walls
            if (ball.position.y < 0 || ball.position.y > WINDOW_HEIGHT - BALL_SIZE)
                ball.speed.y = -ball.speed.y;

            // Check for collision with paddles
            if (CheckCollisionRecs((Rectangle){ leftPaddle.position.x, leftPaddle.position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                                (Rectangle){ ball.position.x, ball.position.y, BALL_SIZE, BALL_SIZE }))
                ball.speed.x = -ball.speed.x;
            if (CheckCollisionRecs((Rectangle){ rightPaddle.position.x, rightPaddle.position.y, PADDLE_WIDTH, PADDLE_HEIGHT },
                                (Rectangle){ ball.position.x, ball.position.y, BALL_SIZE, BALL_SIZE }))
                ball.speed.x = -ball.speed.x;

            // Check for scoring
            if (ball.position.x < 0) {
                rightPaddle.score++;
                ball.position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
            }
            if (ball.position.x > WINDOW_WIDTH) {
                leftPaddle.score++;
                ball.position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
            }

            if (leftPaddle.score == WIN_CON) {
                gameOver = true;
                winner = 1;
            }

            if (rightPaddle.score == WIN_CON) {
                gameOver = true;
                winner = 2;
            }
        }
        else {
            if (IsKeyPressed(KEY_SPACE)) {
                gameOver = false;
                winner = 0;
                leftPaddle.score = 0;
                rightPaddle.score = 0;
                ball.position = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
            }
        }
        BeginDrawing();
        ClearBackground(BLACK);
        
        if (!gameOver) {
            // Draw the paddles
            DrawRectangle(leftPaddle.position.x, leftPaddle.position.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
            DrawRectangle(rightPaddle.position.x, rightPaddle.position.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);

            // Draw the ball
            DrawRectangle(ball.position.x, ball.position.y, BALL_SIZE, BALL_SIZE, WHITE);

            // Draw the scores
            DrawText(TextFormat("%d", leftPaddle.score), WINDOW_WIDTH / 4, 20, 20, WHITE);
            DrawText(TextFormat("%d", rightPaddle.score), 3 * WINDOW_WIDTH / 4, 20, 20, WHITE);
        }
        else {
            const char* winnerText = winner == 1 ? "P1 WINS!" : "P2 WINS!";
            DrawText(winnerText, WINDOW_WIDTH / 2 - MeasureText(winnerText, 40) / 2, WINDOW_HEIGHT / 2 - 40, 40, WHITE);
            DrawText("Press Space to play again",WINDOW_WIDTH / 2 - MeasureText("Press Space to play again", 20) / 2, WINDOW_HEIGHT / 2 + 20, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
