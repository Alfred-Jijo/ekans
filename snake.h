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
};

#endif // SNAKE_H_
