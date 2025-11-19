#include <SDL3/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define CELL_SIZE 40
#define PADDING_SIZE 10
#define MINE_PADDING 5
#define FLAG_PADDING 7

typedef struct cell_s
{
	bool flag;
	bool open;
	bool mine;
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
	bool redraw_required;

	// Graphics
	SDL_Window* window;
	SDL_Renderer* renderer;
}
gamestate_t;

void init_graphics(gamestate_t* game)
{
	if (!SDL_Init(SDL_INIT_VIDEO)) {SDL_Log("%s", SDL_GetError());}
	if (!SDL_CreateWindowAndRenderer(
		u8"Сапёр",
		game->w * (CELL_SIZE + PADDING_SIZE) + PADDING_SIZE,
		game->h * (CELL_SIZE + PADDING_SIZE) + PADDING_SIZE,
		0, &game->window, &game->renderer
	))
	{SDL_Log("%s", SDL_GetError());}
}

void destroy_graphics(gamestate_t* game)
{
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	SDL_Quit();
}

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
		}
	}

	for (int i = 0; i < game->mines; i++)
	{
		int x = rand() % game->w;
		int y = rand() % game->h;
		if (game->field[x][y].mine) i--;
		else game->field[x][y].mine = true;
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
{}

void draw_field(gamestate_t* game)
{
	SDL_SetRenderDrawColor(game->renderer, 70, 70, 70, 255);
	SDL_RenderClear(game->renderer);

	for (int x = 0; x < game->w; x++)
	{
		for (int y = 0; y < game->h; y++)
		{
			SDL_FRect a;
			a.x = PADDING_SIZE + x * (CELL_SIZE + PADDING_SIZE);
			a.y = PADDING_SIZE + y * (CELL_SIZE + PADDING_SIZE);
			a.w = CELL_SIZE;
			a.h = CELL_SIZE;

			if (game->field[x][y].open)
			{
				SDL_SetRenderDrawColor(game->renderer, 240, 240, 240, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(game->renderer, 120, 120, 120, 255);
			}

			SDL_RenderFillRect(game->renderer, &a);

			if (game->field[x][y].mine && (game->debug || game->lose || game->field[x][y].open))
			{
				a.x += MINE_PADDING;
				a.y += MINE_PADDING;
				a.w -= MINE_PADDING * 2;
				a.h -= MINE_PADDING * 2;
				SDL_SetRenderDrawColor(game->renderer, 40, 40, 40, 255);
				SDL_RenderFillRect(game->renderer, &a);
			}

			if (game->field[x][y].flag)
			{
				a.x += FLAG_PADDING;
				a.y += FLAG_PADDING;
				a.w -= FLAG_PADDING * 2;
				a.h -= FLAG_PADDING * 2;
				SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
				SDL_RenderFillRect(game->renderer, &a);
			}
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
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			int x = (e.button.x - PADDING_SIZE) / (CELL_SIZE + PADDING_SIZE);
			int y = (e.button.y - PADDING_SIZE) / (CELL_SIZE + PADDING_SIZE);
			int cell_x = PADDING_SIZE + x * (CELL_SIZE + PADDING_SIZE);
			int cell_y = PADDING_SIZE + y * (CELL_SIZE + PADDING_SIZE);
			if (e.button.x >= cell_x && e.button.x <= cell_x + CELL_SIZE &&
				e.button.y >= cell_y && e.button.y <= cell_y + CELL_SIZE)
			{
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						check_field(game, x, y);
						game->redraw_required = true;
					}
					if (e.button.button == SDL_BUTTON_RIGHT)
					{
						if (game->field[x][y].flag)
						{
							game->field[x][y].flag = false;
							game->flags -= 1;
							game->redraw_required = true;
						}
						else
						{
							game->field[x][y].flag = true;
							game->flags += 1;
							game->redraw_required = true;
						}
					}
				}
			}
			if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_D)
			{
				game->debug = !game->debug;
				game->redraw_required = true;
			}
		}
	}
}

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