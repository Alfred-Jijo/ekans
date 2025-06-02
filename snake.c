/*
MIT License

Copyright (c) 2025 Alfred Jijo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum {
    GRID_WIDTH = 30,
    GRID_HEIGHT = 20,
    CELL_SIZE = 20,
    MAX_SNEK_LENGTH = (GRID_WIDTH * GRID_HEIGHT) -1, // Max number of segments snake can have
    INITIAL_SNEK_LENGTH = 5,
    GAME_TIMER_ID = 1,
    GAME_SPEED = 150, // milliseconds

    WINDOW_WIDTH = (GRID_WIDTH * CELL_SIZE),
    WINDOW_HEIGHT = (GRID_HEIGHT * CELL_SIZE)
};

static_assert(GRID_WIDTH > 0, "GRID_WIDTH must be positive.");
static_assert(GRID_HEIGHT > 0, "GRID_HEIGHT must be positive.");
static_assert(CELL_SIZE > 0, "CELL_SIZE must be positive.");
static_assert(INITIAL_SNEK_LENGTH > 0, "INITIAL_SNEK_LENGTH must be positive.");
static_assert(MAX_SNEK_LENGTH >= INITIAL_SNEK_LENGTH, "MAX_SNEK_LENGTH must be greater than or equal to INITIAL_SNEK_LENGTH.");
static_assert(MAX_SNEK_LENGTH <= (GRID_WIDTH * GRID_HEIGHT), "MAX_SNEK_LENGTH cannot exceed total grid cells."); 
static_assert(GAME_SPEED > 0, "GAME_SPEED must be positive.");

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

GameState game_state = {
    .snake_length = INITIAL_SNEK_LENGTH,
    .current_direction = DIR_RIGHT,
    .input_direction = DIR_RIGHT,
    .game_over = false,
    .score = 0,
    .timer_id = 0, // Will be set by SetTimer
    .game_speed = GAME_SPEED,
    .hwnd_main = NULL // Will be set after window creation
};

void init_game(void);
void restart_game(void);
void update_game(void);
void draw_game(HDC hdc);
void generate_food(void);
void handle_input(WPARAM wParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


/**
 * @brief Initializes the game state, snake position, food, and timer.
 */
void
init_game(void)
{
    int start_x = GRID_WIDTH / 4;
    int start_y = GRID_HEIGHT / 2;
    for (int i = 0; i < game_state.snake_length; ++i) {
        game_state.snake[i].x = start_x - i;
        game_state.snake[i].y = start_y;
    }

    generate_food();

    // Kill existing timer if any, then set a new one
    if (game_state.timer_id != 0) {
        KillTimer(game_state.hwnd_main, game_state.timer_id);
    }
    game_state.timer_id = SetTimer(game_state.hwnd_main, GAME_TIMER_ID, game_state.game_speed, NULL);

    if (game_state.timer_id == 0 && game_state.hwnd_main != NULL) {
        MessageBoxW(game_state.hwnd_main,
            L"Failed to set timer in init_game!", L"Error", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
    }
}

