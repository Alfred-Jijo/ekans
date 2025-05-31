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
}
