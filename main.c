#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "graphics.h"

int main(void)
{
	gamestate_t game;
	game.is_running = true;
	game.redraw_required = true;
	game.flags = 0;
	game.mines = 10;
	game.win = false;
	game.lose = false;
	game.debug = false;
	game.w = 10;
	game.h = 10;

	srand(time(NULL));

	generate_field(&game);
	init_graphics(&game);

	while (game.is_running)
	{
		handle_events(&game);
		if (game.redraw_required) draw_field(&game);
	}

	// Freeing all resources
	destroy_graphics(&game);
	destroy_field(&game);

	return 0;
}