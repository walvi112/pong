#include "renderer.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

const char* const MENU_OPTIONS[] = {"1 Player", "2 Players"};
static bool f_ttf_init = false;
static bool f_image_init = false;
static bool f_window_init = false;
static bool f_render_init = false;

static TTF_Font *arr_use_font[10];
static SDL_Texture *lscore_texture = NULL;
static SDL_Texture *rscore_texture = NULL;

static SDL_Texture *ball_texture = NULL;
static SDL_Texture *paddle_texture = NULL;

static  TTF_Font *score_font = NULL;
static  TTF_Font *game_font = NULL;

static int num_use_font = 0;

static void init_fonts();
static TTF_Font *add_font(const char* font_name, unsigned int font_size);
static SDL_Surface* add_image(const char* image_name);

void init_render(const char *name, SDL_Window **window, SDL_Renderer **renderer, int window_x, int window_y, int window_width, int window_height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "ERROR: SDL_INIT_VIDEO\n");
    }

    *window = SDL_CreateWindow (
        name,
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL
    );
    if (*window == NULL)
    {
        fprintf(stderr, "ERROR: Window.\n");
    }
        f_window_init = true;
    

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    if (*renderer == NULL)
    {
        fprintf(stderr, "ERROR: Renderer.\n");
    }
    else
        f_render_init = true;

    init_fonts();
}


static TTF_Font *add_font(const char* font_name, unsigned int font_size)
{
    if (!f_ttf_init)
    {
        if (TTF_Init() < 0)
        {
            fprintf(stderr, "ERROF: TTF INIT.\n");
            return NULL;
        }
        else
            f_ttf_init = true;
    }

    TTF_Font *new_font = TTF_OpenFont(font_name, font_size);
    if (new_font == NULL)
    {
        fprintf(stderr, "ERROF: Font Init.\n");
        return NULL;
    }
    arr_use_font[num_use_font++] = new_font;
    return new_font;
    
}

static void init_fonts()
{
    score_font = add_font("Roboto-Bold.ttf", 30);
    game_font = add_font("Roboto-Bold.ttf", 40);
}

static SDL_Surface* add_image(const char* image_name)
{
    if (!f_image_init)
    {
        if (IMG_Init(IMG_INIT_PNG) < 0)
        {
            fprintf(stderr, "ERROF: IMAGE INIT.\n");
            return NULL;
        }
        else
            f_image_init = true;
    }

    SDL_Surface *new_surface = IMG_Load(image_name);
    if (new_surface == NULL)
    {
        fprintf(stderr, "ERROF: Image Init.\n");
        return NULL;
    }
    return new_surface;
}

void init_images(SDL_Renderer *renderer)
{
    SDL_Surface *ball_surface = add_image("ball.png");
    SDL_Surface *paddle_surface = add_image("paddle.png");
    ball_texture = SDL_CreateTextureFromSurface(renderer, ball_surface);
    paddle_texture = SDL_CreateTextureFromSurface(renderer, paddle_surface);
    SDL_FreeSurface(ball_surface);
    SDL_FreeSurface(paddle_surface);
}

void render_present(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
}

void render_clear(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void render_background(SDL_Renderer *renderer)
{
    render_clear(renderer);
    SDL_Rect rect;
    //Render middle dotted line
    rect.w = WALL_THICKNESS / 2;
    rect.h = SQUARE_SIZE * 5;
    rect.x = WINDOW_WIDTH / 2 - 1;
    SDL_SetRenderDrawColor(renderer, 0xC0, 0xC0, 0xC0, 0xFF);
    for (rect.y = WALL_THICKNESS + SQUARE_SIZE; rect.y <=  (WINDOW_HEIGHT - WALL_THICKNESS); rect.y += 2*rect.h)
    {
        SDL_RenderFillRect(renderer, &rect);
    }

    //Render top wall
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    rect.x = 0;
    rect.y = 0;
    rect.w = WINDOW_WIDTH;
    rect.h = WALL_THICKNESS;
    SDL_RenderFillRect(renderer, &rect);
    //Render bottom wall
    rect.x = 0;
    rect.y = WINDOW_HEIGHT - WALL_THICKNESS - 1;
    SDL_RenderFillRect(renderer, &rect);
}

void render_paddle(SDL_Renderer *renderer, PaddleStruct *Paddle_left, PaddleStruct *Paddle_right)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_Rect rect = {0, Paddle_left->y, PADDLE_WIDTH, Paddle_left->length};
    SDL_RenderCopyEx(renderer, paddle_texture, NULL, &rect, 0, NULL, SDL_FLIP_HORIZONTAL);
    // SDL_RenderFillRect(renderer, &rect);
    rect.x = WINDOW_WIDTH - PADDLE_WIDTH;
    rect.y = Paddle_right->y;
    // SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopyEx(renderer, paddle_texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void render_ball(SDL_Renderer *renderer, BallStruct *Ball)
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect rect = {(int) Ball->x, (int) Ball->y, BALL_DIM, BALL_DIM};
    // SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, ball_texture, NULL, &rect);
}

void render_score(SDL_Renderer *renderer, ScoreStruct *Score)
{   
    static SDL_Rect lscore_rect;
    static SDL_Rect rscore_rect;
    if (Score->update)
    {
        int w,h;
        char score_str[15];
        SDL_Color font_color = { 255, 255, 255, 255};
        SDL_Surface *surface = NULL;
        //Render left score
        snprintf(score_str, 15, "%d", Score->left);
        surface = TTF_RenderText_Solid(score_font, score_str, font_color);
        w = surface->clip_rect.w;
        h = surface->clip_rect.h;
        if (lscore_texture != NULL)
        {
            SDL_DestroyTexture(lscore_texture);
            lscore_texture = NULL;
        }
        lscore_texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (lscore_texture == NULL)
        {
            fprintf(stderr, "ERROR: Font Texture\n");
        }
        SDL_FreeSurface(surface);
        lscore_rect = (SDL_Rect){(WINDOW_WIDTH / 4), Y_MIN + 5 * SQUARE_SIZE, w, h};
        //Render right score
        snprintf(score_str, 15, "%d", Score->right);
        surface = TTF_RenderText_Solid(score_font, score_str, font_color);
        w = surface->clip_rect.w;
        h = surface->clip_rect.h;
        if (rscore_texture != NULL)
        {
            SDL_DestroyTexture(rscore_texture);
            rscore_texture = NULL;
        }
        rscore_texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (rscore_texture == NULL)
        {
            fprintf(stderr, "ERROR: Font Texture\n");
        }
        SDL_FreeSurface(surface);
        //SDL_QueryTexture(rscore_texture, NULL, NULL, &w, &h);
        rscore_rect = (SDL_Rect){(WINDOW_WIDTH / 4) + (WINDOW_WIDTH / 2), Y_MIN + 5 * SQUARE_SIZE, w, h};
        
    }
    SDL_RenderCopy(renderer, lscore_texture, NULL, &lscore_rect);
    SDL_RenderCopy(renderer, rscore_texture, NULL, &rscore_rect);
    Score->update = false;

}

void render_game_over(SDL_Renderer *renderer, ScoreStruct Score)
{
    int w,h;
    SDL_Color font_color = { 0xFF, 0x00, 0x00, 0xFF};
    SDL_Surface *surface = TTF_RenderText_Solid_Wrapped(game_font, "           YOU WIN\n    (SPACE) Restart\n(BACKSPACE) MENU", font_color, 800);
    w = surface->clip_rect.w;
    h = surface->clip_rect.h;
    SDL_Texture *f_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (f_texture == NULL)
    {
        fprintf(stderr, "ERROR: Font Texture");
        printf("%s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);
    SDL_Rect text_rect = {0, 0, w, h};
    if (Score.left >=10)
        text_rect.x = (WINDOW_WIDTH / 8);
    else   
        text_rect.x = 5 * (WINDOW_WIDTH / 8);
    text_rect.y =  WINDOW_HEIGHT / 4;
    SDL_RenderCopy(renderer, f_texture, NULL, &text_rect);
    SDL_DestroyTexture(f_texture);
    f_texture = NULL;
}

void render_menu(SDL_Renderer *renderer, enum Mode game_mode)
{
    SDL_Rect menu_rect[NUM_CHOICE];
    SDL_Texture *menu_tex[NUM_CHOICE];
    SDL_Surface *surface = NULL;
    for (int i = 0; i < NUM_CHOICE; i++)
    {
        SDL_Color font_color = {0xFF, 0xFF, 0xFF, 0xFF};
        if (i == game_mode)
            font_color = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
            
        surface = TTF_RenderText_Solid(game_font, MENU_OPTIONS[i], font_color);
        menu_tex[i] = SDL_CreateTextureFromSurface(renderer, surface);
        menu_rect[i].x = WINDOW_WIDTH / 2 - surface->clip_rect.w / 2;
        menu_rect[i].y = WINDOW_HEIGHT / 2 - (1 - i) * surface->clip_rect.h;
        menu_rect[i].w = surface->clip_rect.w;
        menu_rect[i].h = surface->clip_rect.h;
        SDL_RenderCopy(renderer, menu_tex[i], NULL, &menu_rect[i]);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(menu_tex[i]);
    }
}

void get_menu_placement(MenuElementStruct *elements)
{
    
    SDL_Color font_color = {0xFF, 0x00, 0x00, 0xFF};
    for (int i = 0; i < NUM_CHOICE; i++)
    {
        SDL_Surface *surface = TTF_RenderText_Solid(game_font, MENU_OPTIONS[i], font_color);
        elements[i].x =  WINDOW_WIDTH / 2 - surface->clip_rect.w / 2;
        elements[i].y =  WINDOW_HEIGHT / 2 - (1 - i) * surface->clip_rect.h;
        elements[i].w = surface->clip_rect.w;
        elements[i].h = surface->clip_rect.h;
        SDL_FreeSurface(surface);
    }
}

void free_render(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (f_ttf_init)
    {
        for (int i = 0; i < num_use_font; i++)
        {
            TTF_CloseFont(arr_use_font[i]);
        }
        TTF_Quit();
    }
    SDL_DestroyTexture(rscore_texture);
    SDL_DestroyTexture(lscore_texture);
    SDL_DestroyTexture(ball_texture);
    SDL_DestroyTexture(paddle_texture);
    SDL_Quit();
}
