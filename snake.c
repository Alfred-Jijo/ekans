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

/**
 * @brief Resets game state for a new game.
 */
void
restart_game(void)
{
    game_state.snake_length = INITIAL_SNEK_LENGTH;
    game_state.current_direction = DIR_RIGHT;
    game_state.input_direction = DIR_RIGHT; 
    game_state.game_over = false;
    game_state.score = 0;

    init_game();
}

/**
 * @brief Updates the game state per frame/timer tick.
 */
void
update_game(void)
{
    if (game_state.game_over) {
        return;
    }

    if ((game_state.input_direction == DIR_UP && game_state.current_direction != DIR_DOWN) ||
        (game_state.input_direction == DIR_DOWN && game_state.current_direction != DIR_UP) ||
        (game_state.input_direction == DIR_LEFT && game_state.current_direction != DIR_RIGHT) ||
        (game_state.input_direction == DIR_RIGHT && game_state.current_direction != DIR_LEFT)) {
        game_state.current_direction = game_state.input_direction;
    }

    // Calculate new head position
    POINT new_head = game_state.snake[0]; 
    switch (game_state.current_direction) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
        case DIR_NONE:  return; 
    }

    // Check for wall collision
    if (new_head.x < 0 || new_head.x >= GRID_WIDTH ||
        new_head.y < 0 || new_head.y >= GRID_HEIGHT) {
        game_state.game_over = true;
        return;
    }

    // Check for self-collision 
    for (int i = 0; i < game_state.snake_length; ++i) {
        if (new_head.x == game_state.snake[i].x && new_head.y == game_state.snake[i].y) {
            game_state.game_over = true;
            return;
        }
    }

    // Check for food consumption
    bool food_eaten = (new_head.x == game_state.food.x && new_head.y == game_state.food.y);

    if (food_eaten) {
        game_state.score += 10;
        if (game_state.snake_length < MAX_SNEK_LENGTH) { 
            game_state.snake_length++;
        }
        generate_food();
    }

    for (int i = game_state.snake_length - 1; i > 0; --i) {
        game_state.snake[i] = game_state.snake[i - 1];
    }
    game_state.snake[0] = new_head; 
}

/**
 * @brief Draws all game elements onto the screen.
 * @param hdc Device context handle.
 */
void
draw_game(
    HDC hdc)
{
    // Double buffering to reduce flicker
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HANDLE hOldBitmap = SelectObject(hdcMem, hbmMem);

    // Draw background
    HBRUSH bgBrush = CreateSolidBrush(RGB(220, 220, 220)); 
    RECT clientRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    FillRect(hdcMem, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // Draw snake
    HBRUSH snakeBrush = CreateSolidBrush(RGB(0, 128, 0));
    for (int i = 0; i < game_state.snake_length; ++i) {
        RECT segmentRect = {
            game_state.snake[i].x * CELL_SIZE,
            game_state.snake[i].y * CELL_SIZE,
            (game_state.snake[i].x + 1) * CELL_SIZE,
            (game_state.snake[i].y + 1) * CELL_SIZE
        };
        FillRect(hdcMem, &segmentRect, snakeBrush);
    }
    DeleteObject(snakeBrush);

    // Draw food
    HBRUSH foodBrush = CreateSolidBrush(RGB(255, 0, 0)); 
    RECT foodRect = {
        game_state.food.x * CELL_SIZE,
        game_state.food.y * CELL_SIZE,
        (game_state.food.x + 1) * CELL_SIZE,
        (game_state.food.y + 1) * CELL_SIZE
    };
    FillRect(hdcMem, &foodRect, foodBrush);
    DeleteObject(foodBrush);

    // Draw score
    WCHAR scoreText[50];
    swprintf_s(scoreText, 50, L"Score: %d", game_state.score);
    SetTextColor(hdcMem, RGB(0, 0, 0)); 
    SetBkMode(hdcMem, TRANSPARENT);    
    TextOutW(hdcMem, 10, 10, scoreText, lstrlenW(scoreText));

    // Draw Game Over message if applicable
    if (game_state.game_over) {
        WCHAR gameOverText[] = L"GAME OVER! Press 'R' to Restart.";
        SetTextAlign(hdcMem, TA_CENTER | TA_BASELINE);
        TextOutW(hdcMem, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, gameOverText, lstrlenW(gameOverText));
        SetTextAlign(hdcMem, TA_LEFT | TA_TOP); 
    }

    // Copy buffer to screen
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);

    // Clean up GDI objects for double buffering
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

/**
 * @brief Generates food at a random valid position on the grid.
 */
void
generate_food(void)
{
    bool free_position;
    do {
        free_position = true;
        game_state.food.x = rand() % GRID_WIDTH;
        game_state.food.y = rand() % GRID_HEIGHT;

        // Check if the new food position is on the snake
        for (int i = 0; i < game_state.snake_length; ++i) {
            if (game_state.food.x == game_state.snake[i].x && 
                game_state.food.y == game_state.snake[i].y) {
                free_position = false;
                break;
            }
        }
    } while (!free_position);
}

/**
 * @brief Handles keyboard input.
 * @param wParam Virtual-key code.
 */
void
handle_input(
    WPARAM wParam)
{
    if (game_state.game_over) {
        if (wParam == 'R' || wParam == 'r') {
            restart_game();
        }
        return;
    }

    // Buffer the input direction, actual change happens in update_game
    switch (wParam) {
        case VK_UP:
            if (game_state.current_direction != DIR_DOWN) { game_state.input_direction = DIR_UP; }
            break;
        case VK_DOWN:
            if (game_state.current_direction != DIR_UP) { game_state.input_direction = DIR_DOWN; }
            break;
        case VK_LEFT:
            if (game_state.current_direction != DIR_RIGHT) { game_state.input_direction = DIR_LEFT; }
            break;
        case VK_RIGHT:
            if (game_state.current_direction != DIR_LEFT) { game_state.input_direction = DIR_RIGHT; }
            break;
    }
}

/**
 * @brief Window procedure to handle messages for the main window.
 */
LRESULT CALLBACK
WindowProc( 
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg) { 
        case WM_CREATE:
            if (game_state.timer_id == 0 && game_state.hwnd_main != NULL) {
                game_state.timer_id = SetTimer(hwnd, GAME_TIMER_ID, game_state.game_speed, NULL);
                if (game_state.timer_id == 0) {
                    MessageBoxW(hwnd, L"Could not Set Timer in WM_CREATE", L"Error", MB_OK | MB_ICONERROR);
                    PostQuitMessage(1); 
                }
            }
            break;

        case WM_TIMER:
            if (wParam == GAME_TIMER_ID) { 
                if (!game_state.game_over) {
                    update_game();
                    InvalidateRect(hwnd, NULL, TRUE); 
                }
            }
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                draw_game(hdc);
                EndPaint(hwnd, &ps);
            }
            break;

        case WM_KEYDOWN:
            handle_input(wParam); 
            break;

        case WM_ERASEBKGND:
            return 1; 

        case WM_DESTROY:
            if (game_state.timer_id != 0) {
                KillTimer(hwnd, game_state.timer_id);
                game_state.timer_id = 0;
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
