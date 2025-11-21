#pragma once
#include <SDL3/SDL.h>
#include "defines.h"

typedef struct cell_s
{
	bool flag;
	bool open;
	bool mine;
	int num;
}
cell_t;

typedef struct gamestate_s
{
	cell_t** field;
	bool is_running;
	bool win;
	bool lose;
	bool debug;
	int w, h;
	int flags;
	int mines;
	int clicks;
	bool redraw_required;

	// Graphics
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* atlas;
}
gamestate_t;

void generate_field(gamestate_t* game);
void destroy_field(gamestate_t* game);
void check_for_win(gamestate_t* game);
void check_field(gamestate_t* game, int x, int y);