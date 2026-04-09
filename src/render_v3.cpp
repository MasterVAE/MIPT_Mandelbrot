#include <immintrin.h>
#include <math.h>
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
    #ifdef RENDER

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelvrot");

    Image image = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Texture2D texture = LoadTextureFromImage(image);

    int frame = 0;

    while (!WindowShouldClose())
    {
        
        CalculatePixels((Color*)image.data);

        Input();
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
        for (int x = 0; x < SCREEN_WIDTH; x += 8)
        {
            int index = y * SCREEN_WIDTH + x;


            float zoomer = 1/ZOOM;
            float iterator[8] = {0, 1, 2, 3, 4, 5, 6, 7};
            __m256 x0_vec = _mm256_setzero_ps();
            _mm256_storeu_ps(iterator, x0_vec);
            x0_vec = _mm256_add_ps(x0_vec, _mm256_set1_ps(x));
            x0_vec = _mm256_sub_ps(x0_vec, _mm256_set1_ps(SCREEN_WIDTH/2));
            x0_vec = _mm256_mul_ps(x0_vec, _mm256_set1_ps(zoomer));
            x0_vec = _mm256_add_ps(x0_vec, _mm256_set1_ps(X0));

            __m256 y0_vec = _mm256_set1_ps(((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0);

            __m256 X_vec = x0_vec;
            __m256 Y_vec = y0_vec;

            volatile __m256 N = _mm256_setzero_ps();

            for(int n = 0; n < nMax; n++)
            {
                __m256 x2_vec = _mm256_mul_ps(X_vec, X_vec);
                __m256 y2_vec = _mm256_mul_ps(Y_vec, Y_vec);
                __m256 xy_vec = _mm256_mul_ps(X_vec, Y_vec);

                __m256 r2_vec = _mm256_add_ps(x2_vec, y2_vec);
            
                __m256 cmp = _mm256_cmp_ps(r2_vec, _mm256_set1_ps(r2Max), _CMP_LE_OQ);
                int mask = _mm256_movemask_ps(cmp);
                if (mask == 0) break;

                N = _mm256_add_ps(N, _mm256_and_ps(cmp, _mm256_set1_ps(1.0f)));

                X_vec = _mm256_add_ps(_mm256_sub_ps(x2_vec, y2_vec), x0_vec);
                Y_vec = _mm256_add_ps(_mm256_add_ps(xy_vec, xy_vec), y0_vec);
            }


            float n_vals[8];
            _mm256_storeu_ps(n_vals, N);


            #ifdef RENDER

            for (int i = 0; i < 8; i++) 
            {
                unsigned char intensity = (unsigned char)(n_vals[i]);
                pixels[index + i] = (Color){ intensity, intensity, intensity, 255 };
            }

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