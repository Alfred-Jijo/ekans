#include "snake.h"
#include <time.h>

extern void init_game(void);
extern void update_game(void);
extern void draw_game(HDC hdc);
// extern void generate_food(void);
extern void handle_input(Direction direction, WPARAM wParam);
extern void restart_game(void);

// Forward declaration of the window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HWND hwnd;
	MSG msg;
	WNDCLASSEX wc = {
                .cbSize = sizeof(WNDCLASSEX),
                .style = CS_HREDRAW | CS_VREDRAW,
                .lpfnWndProc = WindowProc,
                .cbClsExtra = 0,
                .cbWndExtra = 0,
                .hInstance = hInstance,
                .hIcon = LoadIcon(NULL, IDI_APPLICATION),
                .hCursor = LoadCursor(NULL, IDC_ARROW),
                .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
                .lpszMenuName = NULL,
                .lpszClassName = L"Ekans",
                .hIconSm = LoadIcon(NULL, IDI_APPLICATION)
        };

        if (!RegisterClassEx(&wc)) {
                MessageBoxW(NULL, L"Failed to register window class!",
                         L"Error", MB_ICONEXCLAMATION | MB_OK);
                return 1;
        }

        RECT wndRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);
        int adjustedWidth = wndRect.right - wndRect.left;
        int adjustedHeight = wndRect.bottom - wndRect.top;

        hwnd = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"Ekans", 
                L"Ekans - Snake Game",
                WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
                CW_USEDEFAULT, CW_USEDEFAULT,
                adjustedWidth, adjustedHeight,
                NULL, NULL, hInstance, NULL 
        );

        if (!hwnd) {
                MessageBoxW(NULL, L"Failed to create window!",
                         L"Error", MB_ICONEXCLAMATION | MB_OK);
                return 1;
        }

        game_state.hwnd_main = hwnd;
        show_window(hwnd, nCmdShow);
        update_window(hwnd);

        srand((unsigned int)time(NULL)); 
        init_game();

        while (GetMessage(&msg, NULL, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        return (int)msg.wParam;
}

LRESULT CALLBACK
WindowProc(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
                if (game_state.timer_id == 0 && game_state.hwnd_main != NULL) {
                        game_state.timer_id = SetTimer(hwnd, GAME_TIMER_ID, game_state.game_speed, NULL);
                        if (game_state.timer_id == 0) {
                                MessageBoxW(hwnd, L"Couldnt Set Timer", L"Error", MB_OK | MB_ICONERROR);
                                PostQuitMessage(0);
                        }
                }
		break;
	case WM_TIMER:
                if (wParam == game_state.timer_id) {
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
		handle_input(game_state.input_direction, wParam);
		break;
                case WM_ERASEBKGND:
                        return 1; // Prevent flickering by not erasing background
	case WM_DESTROY:
		if (game_state.timer_id != 0)
		{
			KillTimer(hwnd, game_state.timer_id);
			game_state.timer_id = 0;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}
