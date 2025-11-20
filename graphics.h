#pragma once
#include "game.h"
void init_graphics(gamestate_t* game);
void destroy_graphics(gamestate_t* game);
void draw_field(gamestate_t* game);
void handle_events(gamestate_t* game);