Okay, I can show you the key changes that were made to the code in `snake_combined_resizable_c_file` to make the game window resizable.

Here are the main modifications and where you can find them in the `snake_combined_resizable_c_file` code:

1.  **Game State (`GameState` struct)**:
    * **New Members Added**: To manage the dynamic window size and cell scaling, these fields were added to the `GameState` struct:
        ```c
        // ... existing members ...
        HWND hwnd_main;
        // New fields for resizable window
        int client_width;
        int client_height;
        int current_cell_size;
        } GameState;
        ```
    * **Initialization**: The global `game_state` variable was updated to include initial default values for these new members:
        ```c
        GameState game_state = {
            // ... existing initializers ...
            .hwnd_main = NULL,
            .client_width = GRID_WIDTH * DEFAULT_CELL_SIZE,  // Initial default
            .client_height = GRID_HEIGHT * DEFAULT_CELL_SIZE, // Initial default
            .current_cell_size = DEFAULT_CELL_SIZE          // Initial default
        };
        ```

2.  **Constants**:
    * `CELL_SIZE` was renamed to `DEFAULT_CELL_SIZE` to represent the initial size before any resizing.
        ```c
        enum {
            // ...
            DEFAULT_CELL_SIZE = 20, // Initial cell size
            // ...
        };
        ```
    * The fixed `WINDOW_WIDTH` and `WINDOW_HEIGHT` macros (previously derived from `CELL_SIZE`) were effectively removed, as these dimensions are now dynamic and stored in `game_state.client_width` and `game_state.client_height`.

3.  **New Function: `update_drawing_dimensions()`**:
    * This function was added to calculate the `current_cell_size` based on the current `client_width` and `client_height` of the window, ensuring the game grid (`GRID_WIDTH` x `GRID_HEIGHT`) fits while maintaining its aspect ratio. It also centers the grid if the window's aspect ratio doesn't match.
        ```c
        void
        update_drawing_dimensions(void)
        {
            if (game_state.client_width > 0 && game_state.client_height > 0 && GRID_WIDTH > 0 && GRID_HEIGHT > 0) {
                int cell_w = game_state.client_width / GRID_WIDTH;
                int cell_h = game_state.client_height / GRID_HEIGHT;
                game_state.current_cell_size = (cell_w < cell_h) ? cell_w : cell_h; // min(cell_w, cell_h)
                if (game_state.current_cell_size < 1) { // Ensure cell size is at least 1
                    game_state.current_cell_size = 1;
                }
            } else {
                game_state.current_cell_size = DEFAULT_CELL_SIZE; // Fallback
            }
        }
        ```

4.  **Window Creation (`WinMain` function)**:
    * The `CreateWindowExW` call was modified to use `WS_OVERLAPPEDWINDOW` style, which allows resizing by default. The previous style that prevented resizing (`& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX`) was removed for the main style flags.
        ```c
        hwnd = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EkansResizable",
            L"Ekans - Resizable Snake Game",
            WS_OVERLAPPEDWINDOW, // Standard resizable window style
            // ...
        );
        ```
    * The initial window size passed to `AdjustWindowRect` and `CreateWindowExW` is now based on `DEFAULT_CELL_SIZE`.

5.  **Window Procedure (`WindowProc` function)**:
    * **`WM_CREATE`**:
        * It now gets the initial client area dimensions using `GetClientRect`.
        * It calls `update_drawing_dimensions()` to set the initial `current_cell_size`.
        * `init_game()` is called *after* these initial dimensions are set.
            ```c
            case WM_CREATE:
                {
                    RECT rcClient;
                    GetClientRect(hwnd, &rcClient);
                    game_state.client_width = rcClient.right - rcClient.left;
                    game_state.client_height = rcClient.bottom - rcClient.top;
                    update_drawing_dimensions();
                    init_game(); // Initialize game after initial dimensions are set
                    // ... timer setup ...
                }
                break;
            ```
    * **`WM_SIZE`**:
        * This new case handler is crucial for resizing.
        * It updates `game_state.client_width` and `game_state.client_height` from the `lParam` of the message.
        * It calls `update_drawing_dimensions()` to recalculate `current_cell_size`.
        * It calls `InvalidateRect(hwnd, NULL, TRUE);` to force the window to be redrawn with the new scaling.
            ```c
            case WM_SIZE:
                game_state.client_width = LOWORD(lParam);
                game_state.client_height = HIWORD(lParam);
                update_drawing_dimensions();
                InvalidateRect(hwnd, NULL, TRUE); // Request repaint
                break;
            ```

6.  **Drawing Logic (`draw_game` function)**:
    * All drawing calculations now use `game_state.current_cell_size` instead of a fixed `CELL_SIZE`.
    * The double buffer bitmap (`hbmMem`) is created using `game_state.client_width` and `game_state.client_height`.
        ```c
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, game_state.client_width, game_state.client_height);
        ```
    * The background `FillRect` also uses these dynamic dimensions.
    * Offsets (`offset_x`, `offset_y`) are calculated to center the game grid within the potentially larger or differently proportioned client area:
        ```c
        int grid_render_width = GRID_WIDTH * game_state.current_cell_size;
        int grid_render_height = GRID_HEIGHT * game_state.current_cell_size;
        int offset_x = (game_state.client_width - grid_render_width) / 2;
        int offset_y = (game_state.client_height - grid_render_height) / 2;
        ```
    * The drawing of snake segments and food uses these offsets and `game_state.current_cell_size`.
    * The "Game Over" text is centered using `game_state.client_width / 2` and `game_state.client_height / 2`.

7.  **Game Initialization (`init_game` function)**:
    * A check was added at the beginning to try and get client dimensions if `init_game` is somehow called before `WM_CREATE` has fully set them up (though the primary setup is in `WM_CREATE`).
        ```c
        if (game_state.hwnd_main && (game_state.client_width == 0 || game_state.client_height == 0) ) {
            RECT rcClient;
            GetClientRect(game_state.hwnd_main, &rcClient);
            game_state.client_width = rcClient.right - rcClient.left;
            game_state.client_height = rcClient.bottom - rcClient.top;
            update_drawing_dimensions();
        }
        ```

These are the most significant changes that enable the window resizing functionality while scaling the game content appropriately.
