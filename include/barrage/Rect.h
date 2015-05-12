#ifndef RECT_H
#define RECT_H

#include <stdbool.h>

struct Rect
{
        int x;
        int y;
        int width;
        int height;
};

bool valueInRange(int value, int min, int max);
bool rectOverlap(struct Rect A, struct Rect B);

#endif /* RECT_H */
