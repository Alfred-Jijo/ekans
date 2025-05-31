#ifndef SNAKE_H
#define SNAKE_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>

enum {
	GRID_WIDTH = 30,
	GRID_HEIGHT = 20,
	CELL_SIZE = 20,
	MAX_SNEK_LENGTH = (GRID_WIDTH * GRID_HEIGHT) - 1,
	INITIAL_SNEK_LENGTH = 5,
	GAME_TIMER_ID = 1,
	GAME_SPEED = 150, // milliseconds

	WINDOW_WIDTH = (GRID_WIDTH * CELL_SIZE),
	WINDOW_HEIGHT = (GRID_HEIGHT * CELL_SIZE),
};

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_NONE 
} Direction;

typedef struct GameGlobalState {
	POINT snake[MAX_SNEK_LENGTH];
	int snake_length;
	POINT food;
	Direction current_direction;
	Direction input_direction;
	bool game_over;
	int score;
	UINT_PTR timer_id;
	int game_speed;
	HWND hwnd_main;
} GameState;

extern GameState game_state;

#endif // SNAKE_H_
