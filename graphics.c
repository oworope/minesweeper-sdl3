#include <SDL3/SDL.h>
#include "game.h"
#include "graphics.h"

// texture atlas
#define TEXTURE_FILE "Minesweeper_simple_texture_atlas.bmp"
#define TEXTURE_UNIT_SIZE 16

void init_graphics(gamestate_t* game)
{
	if (!SDL_Init(SDL_INIT_VIDEO)) { SDL_Log("%s", SDL_GetError()); }
	if (!SDL_CreateWindowAndRenderer(
		"Minesweeper",
		game->w * CELL_SIZE,
		game->h * CELL_SIZE,
		0, &game->window, &game->renderer
	)) SDL_Log("%s", SDL_GetError());

	SDL_Surface* texture = SDL_LoadBMP(TEXTURE_FILE);
	if (!texture) SDL_Log("%s", SDL_GetError());
	game->atlas = SDL_CreateTextureFromSurface(game->renderer, texture);
	if (!game->atlas) SDL_Log("%s", SDL_GetError());
	SDL_DestroySurface(texture);
	SDL_SetTextureScaleMode(game->atlas, SDL_SCALEMODE_PIXELART);
}

void destroy_graphics(gamestate_t* game)
{
	SDL_DestroyTexture(game->atlas);
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	SDL_Quit();
}

void draw_field(gamestate_t* game)
{
	SDL_SetRenderDrawColor(game->renderer, 70, 70, 70, 255);
	SDL_RenderClear(game->renderer);

	for (int x = 0; x < game->w; x++)
	{
		for (int y = 0; y < game->h; y++)
		{
			SDL_FRect dest;
			dest.x = x * CELL_SIZE;
			dest.y = y * CELL_SIZE;
			dest.w = CELL_SIZE;
			dest.h = CELL_SIZE;

			int tx = 0; // What texture from atlas to use
			int ty = 0;

			if (game->field[x][y].open || game->field[x][y].flag || game->debug)
			{
				if (!game->lose && game->field[x][y].flag && !game->field[x][y].open)
				{
					tx = 2; ty = 2;
				}
				else if ((game->field[x][y].open || game->debug) && game->field[x][y].num != 0)
				{
					// Draw a number
					switch (game->field[x][y].num)
					{
					case 1:
						tx = 1; ty = 0;
						break;
					case 2:
						tx = 2; ty = 0;
						break;
					case 3:
						tx = 3; ty = 0;
						break;
					case 4:
						tx = 0; ty = 1;
						break;
					case 5:
						tx = 1; ty = 1;
						break;
					case 6:
						tx = 2; ty = 1;
						break;
					case 7:
						tx = 3; ty = 1;
						break;
					case 8:
						tx = 0; ty = 2;
						break;
					}
				}
				else
				{
					// Draw an empty cell
					tx = 0;
					ty = 0;
				}
			}
			else
			{
				// Draw a closed cell
				tx = 1;
				ty = 2;
			}

			if (game->debug)
			{
				if (game->field[x][y].mine) { tx = 0; ty = 3; }
			}

			if (game->lose)
			{
				if (game->field[x][y].mine) { tx = 1; ty = 3; }
			}

			SDL_FRect src;
			src.x = tx * TEXTURE_UNIT_SIZE;
			src.y = ty * TEXTURE_UNIT_SIZE;
			src.w = TEXTURE_UNIT_SIZE;
			src.h = TEXTURE_UNIT_SIZE;

			SDL_RenderTexture(game->renderer, game->atlas, &src, &dest);
		}
	}

	SDL_RenderPresent(game->renderer);
}

void handle_events(gamestate_t* game)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			game->is_running = false;
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !(game->lose || game->win))
		{
			int x = e.button.x / CELL_SIZE;
			int y = e.button.y / CELL_SIZE;
			int cell_x = x * CELL_SIZE;
			int cell_y = y * CELL_SIZE;
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				check_field(game, x, y);
				game->redraw_required = true;
			}
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				if (!game->field[x][y].flag && !game->field[x][y].open)
				{
					game->field[x][y].flag = true;
					game->flags += 1;
					game->redraw_required = true;
				}
				else
				{
					game->field[x][y].flag = false;
					game->flags -= 1;
					game->redraw_required = true;
				}
			}
		}
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_D)
		{
			game->debug = !game->debug;
			game->redraw_required = true;
		}
	}
	SDL_Delay(16);
}