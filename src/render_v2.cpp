#include <immintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

#include "raylib.h"
#include "render.h"

//#define RENDER
#define CYCLES 10000

static const size_t UNROLLING_NUMBER = 8;

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

static const float r2Max = 10;
static const int nMax = 255;

static const float ZOOM_SPEED = 2;
static const float MOVE_SPEED = 10;

static float ZOOM = 100;
static float X0 = 0;
static float Y0 = 0;

static void CalculatePixels(Color* pixels);
static void Input();

#define CYCLE for(size_t i = 0; i < UNROLLING_NUMBER; i++)

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

    uint64_t start = __rdtsc();

    for(size_t i = 0; i < CYCLES; i++)
    {
        CalculatePixels(NULL);
    }
    uint64_t end = __rdtsc();

    uint64_t cycles = end - start;
    printf("cycles: %lu\n", cycles);

    #endif
}

static void CalculatePixels(Color* pixels)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x += UNROLLING_NUMBER)
        {
            int index = y * SCREEN_WIDTH + x;
            
            float x0[UNROLLING_NUMBER] = {};
            float y0[UNROLLING_NUMBER] = {};
            CYCLE x0[i] = ((float)(x + i) - SCREEN_WIDTH/2)    / ZOOM + X0;
            CYCLE y0[i] = ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0;

            float X[UNROLLING_NUMBER] = {}; CYCLE X[i] = x0[i];
            float Y[UNROLLING_NUMBER] = {}; CYCLE Y[i] = y0[i];

            int cmp[UNROLLING_NUMBER] = {};

            volatile unsigned char N[UNROLLING_NUMBER] = {};

            for(int n = 0; n < nMax; n++)
            {
                float x2[UNROLLING_NUMBER] = {}; CYCLE x2[i] = X[i] * X[i];
                float y2[UNROLLING_NUMBER] = {}; CYCLE y2[i] = Y[i] * Y[i];
                float xy[UNROLLING_NUMBER] = {}; CYCLE xy[i] = X[i] * Y[i];
            
                float r2[UNROLLING_NUMBER] = {}; CYCLE r2[i] = x2[i] + y2[i];
            
                int cmp[UNROLLING_NUMBER] = {};
                CYCLE if(r2[i] <= r2Max) cmp[i] = 1;

                int mask = 0;
                CYCLE mask |= (cmp[i] << i);
                if(!mask) break;;
                
                CYCLE N[i] += cmp[i];
                CYCLE X[i] = x2[i] - y2[i] + x0[i];
                CYCLE Y[i] = xy[i] + xy[i] + y0[i];

            }

            #ifdef RENDER

            CYCLE  pixels[index + i] = (Color){ N[i], N[i], N[i], nMax };

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