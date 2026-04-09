#include <stdlib.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

#include "raylib.h"
#include "render.h"

//#define RENDER
#define CYCLES 5000

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

static const float r2Max = 10;
static const int NMax = 255;

static const float ZOOM_SPEED = 2;
static const float MOVE_SPEED = 10;

static float ZOOM = 100;
static float X0 = 0;
static float Y0 = 0;

static void CalculatePixels(Color* pixels);
static void Input();

void Render()
{
    #ifdef RENDER

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelvrot");

    Image image = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Texture2D texture = LoadTextureFromImage(image);

    int frame = 0;

    while (!WindowShouldClose())
    {
        Input();
        CalculatePixels((Color*)image.data);

        UpdateTexture(texture, image.data);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();

        frame++;
    }

    UnloadTexture(texture);
    UnloadImage(image);
    CloseWindow();

    #else

    FILE* plot = fopen("plot.file", "w+");
    if(!plot) return;

    for(size_t i = 0; i < CYCLES; i++)
    {
        uint64_t start = __rdtsc();
        CalculatePixels(NULL);
        uint64_t end = __rdtsc();

        uint64_t cycles = end - start;
        fprintf(plot, "%lu;", cycles);
    }
  
    fclose(plot);

    #endif
}

static void CalculatePixels(Color* pixels)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int index = y * SCREEN_WIDTH + x;
            
            float x0 = ((float)x - SCREEN_WIDTH/2)    / ZOOM + X0;
            float y0 = ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0;

            float X = x0;
            float Y = y0;

            volatile unsigned char N = 0;
            for(; N < NMax; N++)
            {
                float x2 = X * X;
                float y2 = Y * Y;
                float xy = X * Y;
            
                float r2 = x2 + y2;
            
                if(r2 > r2Max) break;
            
                X = x2 - y2 + x0;
                Y = xy + xy + y0;
            }

            #ifdef RENDER

            pixels[index] = (Color){ N, N, N, NMax };

            #endif
        }
    }
}

static void Input()
{
    if (IsKeyDown(KEY_Z)) ZOOM *= ZOOM_SPEED;
    if (IsKeyDown(KEY_X)) ZOOM /= ZOOM_SPEED;

    if (IsKeyDown(KEY_LEFT))    X0 -= MOVE_SPEED / ZOOM;
    if (IsKeyDown(KEY_RIGHT))   X0 += MOVE_SPEED / ZOOM;
    if (IsKeyDown(KEY_DOWN))    Y0 += MOVE_SPEED / ZOOM;
    if (IsKeyDown(KEY_UP))      Y0 -= MOVE_SPEED / ZOOM;
}