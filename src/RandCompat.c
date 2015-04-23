#include <barrage/RandCompat.h>

#include <stdlib.h>
#include <time.h>
#include <limits.h>

unsigned int timeSeed()
{
    time_t now = time(0);
    unsigned char* p = (unsigned char*)&now;
    unsigned seed = 0;

    for (size_t i = 0; i < sizeof(now); i++)
    {
        seed = seed * (UCHAR_MAX + 2U) + p[i];
    }

    return seed;
}

/* srand(time_seed()); */

unsigned int randIntRange(unsigned int a, unsigned int b)
{
    return a + rand() / (RAND_MAX / (b - a) + 1);
}

float randFloatRange(float a, float b)
{
    return a + (rand() / (RAND_MAX / (b - a)));
}
