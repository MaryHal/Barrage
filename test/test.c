#include <greatest.h>

#include <barrage/Barrage.h>

#include <math.h>

int isInRange(float value, float expected, float tolerance)
{
    return fabs(expected - value) < tolerance;
}

TEST TestPosition()
{
    const char* script =
        "function main()\n"
        "    x, y = getPosition()\n"
        "    setPosition(x + 10, y + 10)\n"
        "    x, y = getPosition()\n"
        "    setPosition(x + 20, y + 20)\n"
        "end\n";

    struct Barrage* barrage = createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT(barrage->bullets[0].x == 320.0f);
    ASSERT(barrage->bullets[0].y == 120.0f);

    tick(barrage);

    ASSERT(barrage->bullets[0].x == 350.0f);
    ASSERT(barrage->bullets[0].y == 150.0f);

    deleteBarrage(barrage);

    PASS();
}

TEST TestVelocity()
{
    const char* script =
        "function main()\n"
        "    setVelocity(10.0, 10.0)\n"
        "end\n";

    struct Barrage* barrage = createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT(barrage->bullets[0].x == 320.0f);
    ASSERT(barrage->bullets[0].y == 120.0f);

    tick(barrage);

    ASSERT(barrage->bullets[0].x == 330.0f);
    ASSERT(barrage->bullets[0].y == 130.0f);

    tick(barrage);

    ASSERT(barrage->bullets[0].x == 340.0f);
    ASSERT(barrage->bullets[0].y == 140.0f);

    deleteBarrage(barrage);

    PASS();
}

TEST TestSwitchFunction()
{
    const char* script =
        "function main()\n"
        "    setFunction(hello)\n"
        "end\n"
        "function hello()\n"
        "    setPosition(20.0, 30.0)\n"
        "end\n";

    struct Barrage* barrage = createBarrageFromScript(script, 320.0f, 120.0f);

    // Currently setFunction takes one frame, so we have to tick twice to run the new function.
    tick(barrage);
    tick(barrage);

    ASSERT(barrage->bullets[0].x == 20.0f);
    ASSERT(barrage->bullets[0].y == 30.0f);

    deleteBarrage(barrage);

    PASS();
}

SUITE(Bullet_Functionality)
{
    RUN_TEST(TestPosition);
    RUN_TEST(TestVelocity);
    RUN_TEST(TestSwitchFunction);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN(); /* command-line arguments, initialization. */
    {
        RUN_SUITE(Bullet_Functionality);
    }
    GREATEST_MAIN_END();   /* display results */
}
