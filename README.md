# ekans
Snake clone using Win32 and C17

# Build
build with cl:
```cmd
cl main.c game.c /Fe:snake_game.exe /W4 /std:c17 gdi32.lib user32.lib
```

build with gcc:
```
gcc main.c game.c -o snake_game -lgdi32 -Wall -Wextra -std=c17
```
