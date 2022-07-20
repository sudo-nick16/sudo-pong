#pragma once

#include "raylib.h"
#include "iostream"
#include "math.h"
#include "fstream"
#include "random"

class Ball
{
public:
    float x, y;
    float radius;
    float speedX, speedY;

    Ball(const float &xx, const float &yy, const float &r, const float &sx, const float &sy)
        : x(xx), y(yy), radius(r), speedX(sx), speedY(sy)
    {
    }

    void Draw()
    {
        DrawCircle(x, y, radius, BLACK);
    }
};

class Paddle
{
public:
    float x, y;
    float width, height;
    float speed = 700.0f;
    int score = 0;

    Paddle(const float &xx, const float &yy, const float &w, const float &h, const float &s)
        : x(xx), y(yy), width(w), height(h), speed(s)
    {
    }

    Rectangle GetRect()
    {
        return Rectangle({x - width / 2.0f, y - height / 2.0f, width, height});
    }

    void Move(const float &ExpPoint)
    {
        // move up if paddle is below expected point
        if (y >= ExpPoint)
        {
            y -= speed * GetFrameTime();
        }
        // move down if paddle is above expected point
        if (y <= ExpPoint)
        {
            y += speed * GetFrameTime();
        }
    }

    float GetExpPoint(const float &xx, const float &yy, const Ball *ball, const int &offset = 0)
    {
        float Slope = (yy - ball->y) / (xx - ball->x);
        float ExpPoint = -Slope * (xx - ball->x) + yy + offset;
        if (ExpPoint <= 0)
        {
            ExpPoint = 0;
        }
        if (ExpPoint >= GetScreenHeight())
        {
            ExpPoint = GetScreenHeight();
        }
        return ExpPoint;
    }

    void Draw()
    {
        DrawRectangleRec(GetRect(), BLACK);
    }
};

static int HighScore = 0;
static int Screen = 0;
static int FrameCount = 0;
static int Countdown = 3;
static const char *Winner = nullptr;

int main()
{
    std::random_device rseed;
    std::mt19937 rng(rseed());
    std::uniform_int_distribution<int> dist(-1, 1);

    std::ifstream file("pong.csv");
    file.read((char *)&HighScore, sizeof(HighScore));
    file.close();

    InitWindow(800, 600, "SudoPong");
    InitAudioDevice();
    SetTargetFPS(60);

    // init sound
    Sound BgSong = LoadSound("resources/song.mp3");
    Sound duup = LoadSound("resources/duup.wav");
    PlaySound(BgSong);

    // Load assets
    Ball ball(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 10.0f, 400, (dist(rng) < 0) ? -350 : 350);
    Paddle LeftPaddle(100.0f, GetScreenHeight() / 2.0f, 8.0f, 100.0f, 700.0f);
    Paddle RightPaddle(GetScreenWidth() - 100.0f, GetScreenHeight() / 2.0f, 8.0f, 100.0f, 700.0f);

    float x, y, ExpPoint = GetScreenHeight() / 2.0f;

    float XLeft, YLeft, ExpPointRight = GetScreenHeight() / 2.0f;

    while (!WindowShouldClose())
    {
        // play bg song in loop
        if (IsSoundPlaying(BgSong) == false)
        {
            PlaySound(BgSong);
        }

        // recond second x1, y1 cordinates to calculate slope and expected point
        if (ball.x >= LeftPaddle.x && ball.speedX <= 0)
        {
            ExpPoint = LeftPaddle.GetExpPoint(x, y, &ball);
            // std::cout << ey << std::endl;
        }
        if (ball.x <= RightPaddle.x && ball.speedX >= 0)
        {
            ExpPointRight = RightPaddle.GetExpPoint(x, y, &ball);
        }

        if (Screen == 2)
        {
            // mode right paddle (if you wanna automate right paddle too)
            // RightPaddle.Move(ExpPointRight);

            // move left paddle to expected point
            LeftPaddle.Move(ExpPoint);

            // moving the ball

            ball.x += ball.speedX * GetFrameTime();
            ball.y += ball.speedY * GetFrameTime();
        }

        // top and bottom boundaries for ball
        if (ball.y <= 0 || ball.y >= GetScreenHeight())
        {
            if (ball.y <= 0)
            {
                ball.y = 0 + ball.radius;
            }
            else
            {
                ball.y = GetScreenHeight() - ball.radius;
            }
            ball.speedY *= -1;
        }

        // when ball hits left boundary right wins
        if (ball.x <= 0)
        {
            Winner = "You Won!";
        }

        // when ball hits right boundary left wins
        if (ball.x >= GetScreenWidth())
        {
            Winner = "Sudo Won!";
        }

        // left paddle controls
        if (IsKeyDown(KEY_W) && (LeftPaddle.y - LeftPaddle.height / 2.0f) > 0)
        {
            LeftPaddle.y -= LeftPaddle.speed * GetFrameTime();
        }

        if (IsKeyDown(KEY_S) && (LeftPaddle.y + LeftPaddle.height / 2.0f) < GetScreenHeight())
        {
            LeftPaddle.y += LeftPaddle.speed * GetFrameTime();
        }

        // right paddle controls
        if (IsKeyDown(KEY_UP) && (RightPaddle.y - RightPaddle.height / 2.0f) > 0)
        {
            RightPaddle.y -= RightPaddle.speed * GetFrameTime();
        }

        if (IsKeyDown(KEY_DOWN) && (RightPaddle.y + RightPaddle.height / 2.0f) < GetScreenHeight())
        {
            RightPaddle.y += RightPaddle.speed * GetFrameTime();
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, LeftPaddle.GetRect()))
        {
            PlaySound(duup);
            ball.x += 4;
            // ball.speedX *= -1.05f;
            ball.speedX = -ball.speedX + 20;
            LeftPaddle.score += 1;
            ball.speedY = (abs(ball.y - LeftPaddle.y) / (LeftPaddle.height / 2)) * ball.speedX * 2 * (dist(rng) <= 0 ? -1 : 1);
            XLeft = ball.x;
            YLeft = ball.y;
            ExpPoint = GetScreenHeight() / 2.0F;
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, RightPaddle.GetRect()))
        {
            PlaySound(duup);
            ball.x -= 4;
            // ball.speedX *= -1.05f;
            ball.speedX = -ball.speedX - 20;
            RightPaddle.score += 1;
            ball.speedY = (abs(ball.y - RightPaddle.y) / (RightPaddle.height / 2)) * ball.speedX * 2 * (dist(rng) <= 0 ? -1 : 1);

            // after hitting the ball move to middle
            ExpPointRight = GetScreenHeight() / 2.0F;

            // initial x, y cordinates
            x = ball.x;
            y = ball.y;

            // setting high score
            if (RightPaddle.score >= HighScore)
            {
                HighScore = RightPaddle.score;
                std::ofstream file("pong.csv");
                file.write((char *)&HighScore, sizeof(HighScore));
                file.close();
            }
        }

        if (IsKeyDown(KEY_SPACE) && Screen == 0)
        {
            Screen = 1;
            Countdown = 3;
            Winner = nullptr;
            LeftPaddle.score = 0;
            RightPaddle.score = 0;
        }

        if (Winner && IsKeyDown(KEY_SPACE))
        {
            Winner = nullptr;
            LeftPaddle.score = 0;
            RightPaddle.score = 0;
            ball.x = GetScreenWidth() / 2.0f;
            ball.y = GetScreenHeight() / 2.0f;
            ball.speedX = 400;
            ball.speedY = 400;
            ExpPoint = GetScreenHeight() / 2.0f;
            LeftPaddle.y = GetScreenHeight() / 2.0f;
            RightPaddle.y = GetScreenHeight() / 2.0f;
            Screen = 1;
        }

        if (IsKeyDown(KEY_Q))
        {
            Screen = 0;
            Countdown = 3;
            FrameCount = 0;
            Winner = nullptr;
            LeftPaddle.score = 0;
            RightPaddle.score = 0;
            ball.speedX = 400;
            ball.speedY = 400;
            ball.x = GetScreenWidth() / 2.0f;
            ball.y = GetScreenHeight() / 2.0f;
        }

        BeginDrawing();
        // DrawFPS(10, 10);
        ClearBackground(WHITE);

        switch (Screen)
        {
        case 0:
        {
            const char *Title = "Pong - sudonick";
            const char *Instruction = "Press Space to start";
            DrawText(Title, GetScreenWidth() / 2 - MeasureText(Title, 50) / 2, GetScreenHeight() / 2 - 50, 50, BLACK);
            DrawText(Instruction, GetScreenWidth() / 2 - MeasureText(Instruction, 30) / 2, GetScreenHeight() / 2 + 50, 30, BLACK);
            break;
        };
        case 1:
        {
            const char *CountdownStr = TextFormat("%d", Countdown);
            DrawText(CountdownStr, GetScreenWidth() / 2 - MeasureText(CountdownStr, 30) / 2, GetScreenHeight() / 2 - 15, 30, BLACK);
            FrameCount++;
            Countdown = 3 - FrameCount / 60;
            // std::cout << "Countdown: " << Countdown << std::endl;
            if (Countdown == 0)
            {
                Screen = 2;
                Countdown = 3;
                Winner = nullptr;
                FrameCount = 0;
                ball.x = GetScreenWidth() / 2.0f;
            }
            break;
        };
        case 2:
        {
            LeftPaddle.Draw();
            RightPaddle.Draw();
            ball.Draw();

            // line connecting ball and expected point derived from out calculation
            // DrawLine(ball.x, ball.y, 100, ey, BLUE);

            if (Winner != nullptr)
            {
                DrawText(Winner, GetScreenWidth() / 2.0f - MeasureText(Winner, 30) / 2.0f, GetScreenHeight() / 2.0f - 15, 30, BLACK);
            }
            const char *HighScoreStr = TextFormat("High Score: %d", HighScore);
            DrawText(HighScoreStr, GetScreenWidth() / 2.0f - MeasureText(HighScoreStr, 30) / 2.0f, 30, 30, BLACK);
            DrawText(TextFormat("%d", LeftPaddle.score), 50, GetScreenHeight() / 2 - 15, 30, BLACK);
            DrawText(TextFormat("%d", RightPaddle.score), GetScreenWidth() - 50, GetScreenHeight() / 2 - 15, 30, BLACK);
            break;
        }
        default:
        {
            break;
        }
        }
        EndDrawing();
    }

    // unloading resources
    UnloadSound(duup);
    UnloadSound(BgSong);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
