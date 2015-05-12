#include <barrage/Rect.h>

bool valueInRange(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}

bool rectOverlap(struct Rect A, struct Rect B)
{
    bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) ||
        valueInRange(B.x, A.x, A.x + A.width);

    bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) ||
        valueInRange(B.y, A.y, A.y + A.height);

    return xOverlap && yOverlap;
}
