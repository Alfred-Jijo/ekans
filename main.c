#include "snake.h"
#include <time.h>

extern void init_game(void);
extern void update_game(void);
extern void draw_game(HDC hdc);
extern void generate_food(void);
extern void handle_input(Direction direction, WPARAM wParam);
extern void restart_game(void);

// Forward declaration of the window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int
WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
        UNREFERENCED_PARAMETER(hPrevInstance);
        UNREFERENCED_PARAMETER(lpCmdLine);

        WNDCLASSEX wc = {0};
        HWND hwnd;
        MSG msg;
}
