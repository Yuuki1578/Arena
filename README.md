# Usage
```c
#include "./include/arena.h"
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

int main(void) {
  ArenaInit(); // initialize the Arena

  int *array = ArenaAlloc(sizeof(int) * 64);

  // check if null
  if (!ArrayIsNull()) {
    memset(array, 'A', sizeof(int) * 64);
  }

  assert((*array == 'A'));

  ArenaFree(); // free the entire region of memory

  ArenaInit(); // initialize it again

  /*
    * Code using the arena
    * */

  ArenaFree(); // free it again
  return 0;
}
```
