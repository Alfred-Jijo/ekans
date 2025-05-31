#include "snake.h"
#include <stdlib.h>
#include <stdio.h>

GameState game_state = {
    .snake_length = INITIAL_SNEK_LENGTH,
    .current_direction = DIR_RIGHT,
    .input_direction = DIR_RIGHT,
    .game_over = false,
    .score = 0,
    .timer_id = 0,
    .game_speed = GAME_SPEED
};

void 
init_game (void)
{
        int start_x = GRID_WIDTH / 4;
        int start_y = GRID_HEIGHT / 2;
        for (int i = 0; i < game_state.snake_length; ++i) {
            game_state.snake[i].x = start_x - i;
            game_state.snake[i].y = start_y;
        }

        generate_food();

        if (game_state.hwnd_main == NULL) {
            fprintf(stderr, "Error: hwnd_main is not set.\n");
            exit(EXIT_FAILURE);
        }
        
        if (game_state.timer_id) {
                KillTimer(game_state.hwnd_main, game_state.timer_id);
        }
        game_state.timer_id = SetTimer(game_state.hwnd_main, GAME_TIMER_ID, game_state.game_speed, NULL);

        if (game_state.timer_id == 0 && game_state.hwnd_main != NULL) {
                // Error handling if SetTimer fails
                MessageBoxW(game_state.hwnd_main,
                        L"Failed to set timer in InitializeGame!", L"Error", MB_OK | MB_ICONERROR);
        }
}

void
restart_game (void)
{
    game_state.snake_length = INITIAL_SNEK_LENGTH;
    game_state.current_direction = DIR_RIGHT;
    game_state.input_direction = DIR_RIGHT;
    game_state.game_over = false;
    game_state.score = 0;

    init_game();
}

void
update_game (void)
{

}

void
draw_game (
        HDC hdc)
{

}

void
generate_food (void)
{

}

void
handle_input (
        Direction direction)
{
    if (game_state.game_over) {
        return;
    }

    // Prevent the snake from reversing direction
    if ((direction == DIR_UP && game_state.current_direction != DIR_DOWN) ||
        (direction == DIR_DOWN && game_state.current_direction != DIR_UP) ||
        (direction == DIR_LEFT && game_state.current_direction != DIR_RIGHT) ||
        (direction == DIR_RIGHT && game_state.current_direction != DIR_LEFT)) {
        game_state.input_direction = direction;
    }
}
