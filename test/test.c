#include <greatest.h>

#include <barrage/Barrage.h>

#include <math.h>

int isInRange(float value, float expected, float tolerance)
{
    return fabs(expected - value) < tolerance;
}

TEST BasicMovementTest()
{
    const char* script =
        "function main()\n"
        "    x, y = getPosition()\n"
        "    setPosition(x + 10, y + 10)\n"
        "    setFunction(move)\n"
        "end\n"
        "function move()\n"
        "    setVelocity(5.0, 5.0)\n"
        "end\n";

    struct Barrage* barrage = createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT(barrage->bullets[0].x == 320.0f);
    ASSERT(barrage->bullets[0].y == 120.0f);

    tick(barrage);

    ASSERT(barrage->bullets[0].x == 330.0f);
    ASSERT(barrage->bullets[0].y == 130.0f);

    tick(barrage);

    ASSERT(barrage->bullets[0].x == 335.0f);
    ASSERT(barrage->bullets[0].y == 135.0f);

    deleteBarrage(barrage);

    PASS();
}

TEST LaunchTest()
{
    const char* script =
        "function main()\n"
        "    launch(45, 1, kill)\n"
        "    launch(45, 1, kill)\n"
        "    launch(45, 1, kill)\n"
        "    launch(45, 1, kill)\n"
        "    launch(45, 1, kill)\n"
        "    kill()\n"
        "end\n";

    struct Barrage* barrage = createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT_EQ(barrage->activeCount, 1);
    tick(barrage);
    ASSERT_EQ(barrage->activeCount, 5);
    tick(barrage);
    ASSERT_EQ(barrage->activeCount, 0);

    deleteBarrage(barrage);

    PASS();
}

/* TEST FileTest() */
/* { */
/*     struct Barrage* barrage = createBarrageFromFile("example/barrage/test11.lua", 320.0f, 120.0f); */

/*     for (int i = 0; i < 200; ++i) */
/*     { */
/*         tick(barrage); */

/*         while (available(barrage)) */
/*         { */
/*             yield(barrage); */
/*         } */

/*         printf("%d %lu\n", i, barrage->activeCount); */
/*     } */

/*     deleteBarrage(barrage); */

/*     PASS(); */
/* } */

SUITE(Bullet_Functionality)
{
    RUN_TEST(BasicMovementTest);
    RUN_TEST(LaunchTest);

    /* RUN_TEST(FileTest); */
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
