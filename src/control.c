#include "control.h"


static SDL_Event event;

void handle_event(KeyInput *input, bool *quit)
{
    const Uint8 *keyBoard = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                *quit = true;
                return;
        }   
    }

    *quit = keyBoard[SDL_SCANCODE_ESCAPE];
    input->w_pressed = keyBoard[SDL_SCANCODE_W];
    input->s_pressed = keyBoard[SDL_SCANCODE_S];
    input->up_pressed = keyBoard[SDL_SCANCODE_UP];
    input->down_pressed = keyBoard[SDL_SCANCODE_DOWN];
    input->space_pressed = keyBoard[SDL_SCANCODE_SPACE];
    input->backspace_pressed = keyBoard[SDL_SCANCODE_BACKSPACE];
    SDL_PumpEvents();
}

void handle_menu_event(enum State *game_state, enum Mode *mode, bool *quit)
{
    MenuElementStruct elements[NUM_CHOICE];
    get_menu_placement(elements);

    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                *quit = true;
                return;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym )
                    {
                        case SDLK_ESCAPE:
                            *quit = true;
                            return;
                            break;
                        case SDLK_RETURN:
                            *game_state = S_RUN;
                            return;
                            break;
                    }
                break;
            case SDL_MOUSEMOTION:
                for (int i = 0; i < NUM_CHOICE; i++)
                {
                    if ((event.motion.y >= elements[i].y) && (event.motion.y <= elements[i].y + elements[i].h))
                        *mode = i;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                *game_state = S_RUN;
                return;
                break;
        }   
    }
}


