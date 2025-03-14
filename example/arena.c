#include "../include/arena.h"
#include <bits/page_size.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Rectangle {
  long double xAxis, yAxis;
};

void *NewObject(size_t typeSize, const void *src) {
  void *object = ArenaAlloc(typeSize);
  memcpy(object, src, typeSize);

  return object;
}

int main(void) {
  ArenaInit();
  srand(time(NULL));

  long *dynArray = ArenaAlloc(sizeof(long) * PAGE_SIZE);
  struct Rectangle rectangle = {.xAxis = 69.69, .yAxis = 69.69}, *refRect;

  refRect = NewObject(sizeof rectangle, &rectangle);

  if (ArenaIsNull()) {
    goto EndMain;
  }

  for (long i = 0; i < PAGE_SIZE; i++) {
    long randomSeq = rand() % LONG_MAX;

    dynArray[i] = randomSeq;
    printf("%li\n", dynArray[i]);
  }

  printf("Rectangle: (%Lf, %Lf)\n", refRect->xAxis, refRect->yAxis);

EndMain:
  ArenaFree();
  return 0;
}
