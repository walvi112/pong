#include "game.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

static PaddleStruct Paddle_left = {};
static PaddleStruct Paddle_right = {0};
static BallStruct Ball = {0};
static ScoreStruct Score = {0};

static KeyInput input = {false};
static float Ball_speed = 2;

static enum State game_state = S_MENU;
static enum Mode game_mode = SINGLE;

static bool quit = false;
static uint32_t frame_start, frame_end, frame_duration;



static void init_game();
static void init_ball();
static void move_paddles();
static void move_ball();
static void detect_collision();
static void detect_score();
static void handle_menu(SDL_Renderer *renderer);
static void handle_game(SDL_Renderer *renderer);
static void handle_game_over(SDL_Renderer *renderer);
static void handle_pause(SDL_Renderer *renderer);
static void clear_input();


void run(SDL_Renderer *renderer)
{
    while (!quit)
    {
        frame_start = SDL_GetTicks();
        handle_event(&input, &quit);
        switch (game_state)
        {
            case S_MENU:
                handle_menu(renderer);
                break;
            case S_RUN:
                handle_game(renderer);
                break;
            case S_PAUSE:
                handle_pause(renderer);
                break;
            case S_OVER:
                handle_game_over(renderer);
                break;
        }
        frame_end = SDL_GetTicks();
        frame_duration = frame_end - frame_start;
        if (frame_duration < TARGET_FRAME_DURATION)
        {
            SDL_Delay(TARGET_FRAME_DURATION - frame_duration);
        }
    }

}

static void init_game()
{
    init_ball();
    Score.left = 0;
    Score.right = 0;
    Paddle_left = (PaddleStruct){WINDOW_HEIGHT / 2 - PADDLE_LENGTH / 2, PADDLE_LENGTH};
    Paddle_right = (PaddleStruct){WINDOW_HEIGHT / 2 - PADDLE_LENGTH / 2, PADDLE_LENGTH};
}


static void init_ball()
{
    Ball_speed = 2;
    Score.update = true;
    srand(time(0));
    Ball.x = WINDOW_WIDTH / 2;
    Ball.y = Y_MIN + (rand() % Y_MAX);
    
    do
    {
        Ball.vect_x = sin(3.14 / 4) * ((rand() % 3) - 1);
        Ball.vect_y = Ball.vect_x;
    }while (Ball.vect_x == 0);
    
}

static void handle_menu(SDL_Renderer *renderer)
{
    init_game();
    while (game_state == S_MENU && !quit)
    {
        handle_menu_event(&game_state, &game_mode, &quit);
        render_clear(renderer);
        render_menu(renderer, game_mode);
        render_present(renderer);
    }

}

static void handle_game(SDL_Renderer *renderer)
{
    static bool first_pressed;
    if (input.space_pressed)
        first_pressed = true;
    else if (!input.space_pressed && first_pressed)
    {
        first_pressed = false;
        game_state = S_PAUSE;
        clear_input();
        return;
    }
    if (input.backspace_pressed)
    {
        game_state = S_MENU;
        return;
    }
    move_paddles();
    move_ball();
    detect_collision();
    detect_score();
    clear_input();
    render_background(renderer);
    render_paddle(renderer, &Paddle_left, &Paddle_right);
    render_ball(renderer, &Ball);
    render_score(renderer, &Score);
    render_present(renderer);

}

static void handle_game_over(SDL_Renderer *renderer)
{
    static bool first_pressed;
    if (input.space_pressed)
        first_pressed = true;
    else if (!input.space_pressed && first_pressed)
    {
        first_pressed = false;
        init_game();
        game_state = S_RUN;
        clear_input();
        return;
    }
    if (input.backspace_pressed)
    {
        game_state = S_MENU;
        return;
    }

    render_background(renderer);
    render_score(renderer, &Score);
    render_game_over(renderer, Score);
    render_present(renderer);

}

static void handle_pause(SDL_Renderer *renderer)
{
    static bool first_pressed;
    if (input.space_pressed)
        first_pressed = true;
    else if (!input.space_pressed && first_pressed)
    {
        first_pressed = false;
        game_state = S_RUN;
        clear_input();
        return;
    }
    render_background(renderer);
    render_paddle(renderer, &Paddle_left, &Paddle_right);
    render_ball(renderer, &Ball);
    render_score(renderer, &Score);
    render_present(renderer);

}

static void detect_score()
{
    if (Ball.x >= 1.5 * WINDOW_WIDTH)
    {
        Score.left += 1;
        init_ball();
    }
    else if ( Ball.x <= -0.5 * WINDOW_WIDTH)
    {
        Score.right += 1;
        init_ball();
    }
    if (Score.right >=10 || Score.left >= 10)
    {
        game_state = S_OVER;
    }
}


static void move_paddles()
{

    if (input.up_pressed)
    {
        Paddle_right.y -= PADDLE_SPEED;
    }
    if (input.down_pressed)
    {
        Paddle_right.y += PADDLE_SPEED;
    }
    switch (game_mode)
    {
        case SINGLE:
            {
                if (Ball.y <= Paddle_left.y + Paddle_left.length / 2)
                    Paddle_left.y -= PADDLE_SPEED;
                if (Ball.y >= Paddle_left.y + Paddle_left.length / 2)
                    Paddle_left.y += PADDLE_SPEED;
                break;
            }
        case DUO:
            {
                if (input.w_pressed)
                {
                    Paddle_left.y -= PADDLE_SPEED;
                }
                if (input.s_pressed)
                {
                    Paddle_left.y += PADDLE_SPEED;
                }
                break;
            }
    }
     
    if (Paddle_right.y < WALL_THICKNESS)
        Paddle_right.y = WALL_THICKNESS;
    if (Paddle_right.y > (WINDOW_HEIGHT - 1) - WALL_THICKNESS - Paddle_right.length)
        Paddle_right.y =  (WINDOW_HEIGHT - 1) - WALL_THICKNESS - Paddle_right.length;
    if (Paddle_left.y < WALL_THICKNESS)
        Paddle_left.y = WALL_THICKNESS;
    if (Paddle_left.y > (WINDOW_HEIGHT - 1) - WALL_THICKNESS - Paddle_left.length)
        Paddle_left.y =  (WINDOW_HEIGHT - 1) - WALL_THICKNESS - Paddle_left.length;
}

static void move_ball()
{
    Ball.x += Ball.vect_x * Ball_speed;
    Ball.y += Ball.vect_y * Ball_speed;
}


static void detect_collision()
{
    if (!(Ball.x <= WINDOW_WIDTH && Ball.x >= -BALL_DIM))
    {
        return;
    }

    if (Ball.y <= Y_MIN)
    {
        //Collide top wall
        Ball.x = (Ball.y - Y_MIN) * (Ball.vect_x * Ball_speed) / (-Ball.vect_y * Ball_speed) + Ball.x - Ball.vect_x * Ball_speed;
        Ball.y = Y_MIN;
        Ball.vect_y *= -1;
    }
    else if (Ball.y >= Y_MAX - BALL_DIM)
    {
        //Collide bottom wall
        Ball.x = (Ball.y - (Y_MAX - BALL_DIM)) * (Ball.vect_x * Ball_speed) / (-Ball.vect_y * Ball_speed) + Ball.x - Ball.vect_x * Ball_speed;
        Ball.y = Y_MAX - BALL_DIM;
        Ball.vect_y *= -1;
    }
    else if  ((Ball.x >= X_MAX - BALL_DIM) && (Ball.y >= Paddle_right.y - BALL_DIM) && (Ball.y <= Paddle_right.y + Paddle_right.length))
    {
        //Collide right paddle
        Ball.y = (Ball.x - (X_MAX - BALL_DIM)) * (-Ball.vect_y * Ball_speed) / (Ball.vect_x * Ball_speed) + Ball.y;
        Ball.x = X_MAX - BALL_DIM;
        float per_angle = (Ball.y - (Paddle_right.y + Paddle_right.length / 2))/ (Paddle_right.length /2 + BALL_DIM);
        Ball.vect_x = -cos(DEFLECT_ANGLE * per_angle);
        Ball.vect_y = sin(DEFLECT_ANGLE * per_angle);
        Ball_speed += 0.2;
    }
    else if ((Ball.x <= X_MIN) && (Ball.y >= Paddle_left.y- BALL_DIM) && (Ball.y <= Paddle_left.y + Paddle_left.length))
    {
        //Collide left paddle
        Ball.y = (Ball.x - X_MIN) * (-Ball.vect_y * Ball_speed) / (Ball.vect_x * Ball_speed) + Ball.y;
        Ball.x = X_MIN;
        float per_angle = (Ball.y - (Paddle_left.y + Paddle_left.length / 2))/ (Paddle_left.length /2 + BALL_DIM);
        Ball.vect_x = cos(DEFLECT_ANGLE * per_angle);
        Ball.vect_y = sin(DEFLECT_ANGLE * per_angle);
        Ball_speed += 0.2;
    }
}

static void clear_input()
{
    input.w_pressed = false;
    input.s_pressed = false;
    input.up_pressed = false;
    input.down_pressed = false;
    input.space_pressed = false;
}