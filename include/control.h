#ifndef CONTROL_H
#define CONTROL_H
#include "renderer.h"

enum State {S_MENU, S_RUN, S_OVER, S_PAUSE};


typedef struct KeyInput
{
    bool w_pressed;
    bool s_pressed;
    bool up_pressed;
    bool down_pressed;
    bool space_pressed;
    bool backspace_pressed;
}KeyInput;


void handle_event(KeyInput *input, bool *quit);
void handle_menu_event(enum State *game_state, enum Mode *mode, bool *quit);
#endif