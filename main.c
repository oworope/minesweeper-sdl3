#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "graphics.h"

void minesweeper_game(int w, int h, int m)
{
	gamestate_t game;
	game.is_running = true;
	game.redraw_required = true;
	game.flags = 0;
	game.mines = m;
	game.win = false;
	game.lose = false;
	game.debug = false;
	game.w = w;
	game.h = h;
	game.clicks = 0;

	srand(time(NULL));

	generate_field(&game);
	init_graphics(&game);

	bool game_ended = false;

	while (game.is_running)
	{
		handle_events(&game);
		if (game.redraw_required) draw_field(&game);
		if (game.win && !game_ended)
		{
			game_ended = true;
			printf("You won! Clicks: %d. You can now close the game.\n", game.clicks);
		}
		if (game.lose && !game_ended)
		{
			game_ended = true;
			printf("You lost! Clicks: %d. You can now close the game.\n", game.clicks);
		}
	}

	// Freeing all resources
	destroy_graphics(&game);
	destroy_field(&game);
}

int main()
{
	minesweeper_game(5, 5, 3);
	return 0;
}