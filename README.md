# ekans
Snake clone using Win32 and C17

# Build
build with cl:
```cmd
cl snake.c /Fe:snake_game.exe user32.lib gdi32.lib /Wall /std:c17
```

build with gcc:
```shell
gcc snake.c -o snake_game -lgdi32 -Wall -Wextra -std=c17
```
