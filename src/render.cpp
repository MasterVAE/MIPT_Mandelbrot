#include <immintrin.h>

#include "raylib.h"
#include "render.h"

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

void Render()
{
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
}

static void CalculatePixels(Color* pixels)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x += 4)
        {
            int index = y * SCREEN_WIDTH + x;
            
            float x0[4] = 
            {
                ((float)(x    ) - SCREEN_WIDTH/2)    / ZOOM + X0,
                ((float)(x + 1) - SCREEN_WIDTH/2)    / ZOOM + X0,
                ((float)(x + 2) - SCREEN_WIDTH/2)    / ZOOM + X0,
                ((float)(x + 3) - SCREEN_WIDTH/2)    / ZOOM + X0
            };

            float y0[4] = 
            {
                ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0,
                ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0,
                ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0,
                ((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0
            };

            float X[4] = {}; for(int i = 0; i < 4; i++) X[i] = x0[i];
            float Y[4] = {}; for(int i = 0; i < 4; i++) Y[i] = y0[i];

            int cmp[4] = {};

            unsigned char N[4] = {};

            for(int n = 0; n < nMax; n++)
            {
                float x2[4] = {}; for(int i = 0; i < 4; i++) x2[i] = X[i] * X[i];
                float y2[4] = {}; for(int i = 0; i < 4; i++) y2[i] = Y[i] * Y[i];
                float xy[4] = {}; for(int i = 0; i < 4; i++) xy[i] = X[i] * Y[i];
            
                float r2[4] = {}; for(int i = 0; i < 4; i++) r2[i] = x2[i] + y2[i];
            
                int cmp[4] = {};
                for (int i = 0; i < 4; i++) if(r2[i] <= r2Max) cmp[i] = 1;

                int mask = 0;
                for(int i = 0; i < 4; i++) mask |= (cmp[i] << i);
                if(!mask) break;;
                
                for(int i = 0; i < 4; i++) N[i] += cmp[i];
                for(int i = 0; i < 4; i++) X[i] = x2[i] - y2[i] + x0[i];
                for(int i = 0; i < 4; i++) Y[i] = xy[i] + xy[i] + y0[i];

            }

            for(int i = 0; i < 4; i++)  pixels[index + i] = (Color){ N[i], N[i], N[i], nMax };
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