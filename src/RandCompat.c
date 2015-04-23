#include <barrage/RandCompat.h>

#include <limits.h>

unsigned int randIntRange(unsigned int a, unsigned int b)
{
    return a + rand() / (RAND_MAX / (b - a) + 1);
}

float randFloatRange(float a, float b)
{
    return a + (rand() / (RAND_MAX / (b - a)));
}
