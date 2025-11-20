#include "game.h"
#include "defines.h"

#include <stdlib.h>

void generate_field(gamestate_t* game)
{
	game->field = (cell_t**)malloc(sizeof(cell_t*) * game->w);
	for (int i = 0; i < game->w; i++)
	{
		game->field[i] = (cell_t*)malloc(sizeof(cell_t) * game->h);
	}

	for (int x = 0; x < game->w; x++)
	{
		for (int y = 0; y < game->h; y++)
		{
			game->field[x][y].flag = false;
			game->field[x][y].open = false;
			game->field[x][y].mine = false;
			game->field[x][y].num = 0;
		}
	}

	for (int i = 0; i < game->mines; i++)
	{
		int x = rand() % game->w;
		int y = rand() % game->h;
		if (game->field[x][y].mine) i--;
		else game->field[x][y].mine = true;
	}

	for (int x = 1; x < game->w - 1; x++)
	{
		for (int y = 1; y < game->h - 1; y++)
		{
			//
			if (game->field[x][y].mine) continue;
			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					if (game->field[x + i][y + j].mine)
						game->field[x][y].num += 1;
				}
			}
		}
	}
}

void destroy_field(gamestate_t* game)
{
	for (int i = 0; i < game->w; i++)
	{
		free(game->field[i]);
	}
	free(game->field);
}

void check_field(gamestate_t* game, int x, int y)
{
	if (game->field[x][y].mine) { game->lose = true; return; }
	game->field[x][y].open = true;
	game->field[x][y].flag = false;
	game->redraw_required = true;
	if (x == game->w - 1 || y == game->w - 1 || x == 0 || y == 0)
	return;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (x + i < 0 || y + i < 0 || x + i > game->w || y + i > game->h)
				continue;
			if (
				!game->field[x + i][y + j].mine
				&& !game->field[x + i][y + j].open
				&& game->field[x][y].num == 0
			)
			check_field(game, x + i, y + j);
		}
	}
}