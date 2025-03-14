EXAMPLE="arena"
CC="cc"
CFLAGS="-Wall -Werror -Wpedantic -g -fsanitize=address -o $EXAMPLE"

if [[ -f "./example/arena.c" ]] && [[ -f "./include/arena.h" ]]; then
  $CC "./example/arena.c" $CFLAGS
fi
