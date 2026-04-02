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
            __m128 x0_vec = _mm_setr_ps(
                                ((float)(x    ) - SCREEN_WIDTH/2)    / ZOOM + X0,
                                ((float)(x + 1) - SCREEN_WIDTH/2)    / ZOOM + X0,
                                ((float)(x + 2) - SCREEN_WIDTH/2)    / ZOOM + X0,
                                ((float)(x + 3) - SCREEN_WIDTH/2)    / ZOOM + X0
                            );

            __m128 y0_vec = _mm_set1_ps(((float)y - SCREEN_HEIGHT/2)   / ZOOM + Y0);

            __m128 X_vec = x0_vec;
            __m128 Y_vec = y0_vec;

            __m128 N = _mm_setzero_ps();

            for(int n = 0; n < nMax; n++)
            {
                __m128 x2_vec = _mm_mul_ps(X_vec, X_vec);
                __m128 y2_vec = _mm_mul_ps(Y_vec, Y_vec);
                __m128 xy_vec = _mm_mul_ps(X_vec, Y_vec);

                __m128 r2_vec = _mm_add_ps(x2_vec, y2_vec);
            
                __m128 cmp = _mm_cmple_ps(r2_vec, _mm_set1_ps(r2Max));
                int mask = _mm_movemask_ps(cmp);
                if (mask == 0) break;

                N = _mm_add_ps(N, _mm_and_ps(cmp, _mm_set1_ps(1.0f)));

                X_vec = _mm_add_ps(_mm_sub_ps(x2_vec, y2_vec), x0_vec);
                Y_vec = _mm_add_ps(_mm_add_ps(xy_vec, xy_vec), y0_vec);
            }


            float n_vals[4];
            _mm_storeu_ps(n_vals, N);
            for (int i = 0; i < 4; i++) 
            {
                unsigned char intensity = (unsigned char)(n_vals[i]);
                pixels[index + i] = (Color){ intensity, intensity, intensity, 255 };
            }
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