#include "stdio.h"
#include "game.h"

int main(int argc, char* argv[])
{

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    init_render("Pong", &window, &renderer , WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    init_images(renderer);
    run(renderer);
    free_render(window, renderer);
    return 0;
}