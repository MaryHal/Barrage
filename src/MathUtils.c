#include <bl/MathUtils.h>

const float bl_PI = 3.14159f;

float degToRad(float deg)
{
    return bl_PI * deg / 180.0f;
}
