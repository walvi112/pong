#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <stdbool.h>
#define WINDOW_X    SDL_WINDOWPOS_UNDEFINED
#define WINDOW_Y    SDL_WINDOWPOS_UNDEFINED
#define WINDOW_WIDTH    1440
#define WINDOW_HEIGHT   860

#define SQUARE_SIZE             5

#define WALL_SIZE               5
#define PADDLE_WIDTH_SIZE       5
#define BALL_SIZE               5
#define PADDLE_SIZE             25

#define TARGET_FRAME_FPS        30
#define TARGET_FRAME_DURATION   (100 / TARGET_FRAME_FPS)

#define WALL_THICKNESS      SQUARE_SIZE * WALL_SIZE
#define PADDLE_WIDTH        SQUARE_SIZE * PADDLE_WIDTH_SIZE
#define BALL_DIM            SQUARE_SIZE * BALL_SIZE
#define PADDLE_LENGTH       PADDLE_SIZE * SQUARE_SIZE

#define X_MIN           (PADDLE_WIDTH)
#define X_MAX           (WINDOW_WIDTH - PADDLE_WIDTH - 2)
#define Y_MIN           (WALL_THICKNESS)
#define Y_MAX           (WINDOW_HEIGHT - WALL_THICKNESS - 2)

#define NUM_CHOICE          2
extern const char* const MENU_OPTIONS[];


enum Mode {SINGLE, DUO};

typedef struct MenuElementStruct
{
    int x;
    int y;
    int w;
    int h;
}MenuElementStruct;

typedef struct PaddleStruct
{
    int y;
    int length;
}PaddleStruct;

typedef struct BallStruct
{
    float x;
    float y;
    float vect_x;
    float vect_y;
}BallStruct;

typedef struct ScoreStruct
{
    int left;
    int right;
    bool update;
}ScoreStruct;

void init_render(const char* name, SDL_Window **window, SDL_Renderer **renderer , int window_x, int window_y, int window_width, int window_height);
void init_images(SDL_Renderer *renderer);

void render_present(SDL_Renderer *renderer);
void render_background(SDL_Renderer *renderer);
void render_paddle(SDL_Renderer *renderer, PaddleStruct *Paddle_left, PaddleStruct *Paddle_right);
void render_ball(SDL_Renderer *renderer, BallStruct *Ball);
void render_score(SDL_Renderer *renderer, ScoreStruct *Score);
void render_game_over(SDL_Renderer *renderer, ScoreStruct Score);
void render_clear(SDL_Renderer *renderer);
void render_menu(SDL_Renderer *renderer, enum Mode game_mode);
void free_render(SDL_Window *window, SDL_Renderer *renderer);
void get_menu_placement(MenuElementStruct *elements);




#endif