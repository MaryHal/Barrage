#include <greatest.h>

#include <barrage/Barrage.h>
/* #include <barrage/RandCompat.h> */

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

    struct Barrage* barrage = br_createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT(barrage->bullets[0].x == 320.0f);
    ASSERT(barrage->bullets[0].y == 120.0f);

    br_tick(barrage);

    ASSERT(barrage->bullets[0].x == 330.0f);
    ASSERT(barrage->bullets[0].y == 130.0f);

    br_tick(barrage);

    ASSERT(barrage->bullets[0].x == 335.0f);
    ASSERT(barrage->bullets[0].y == 135.0f);

    br_deleteBarrage(barrage);

    PASS();
}

TEST VanishTest()
{
    const char* script =
        "function main()\n"
        "    vanish()\n"
        "end\n";

    struct Barrage* barrage = br_createBarrageFromScript(script, 320.0f, 120.0f);

    printf("\n");

    ASSERT_EQ(barrage->activeCount, 1);

    for (int i = 0; i < 35; ++i)
    {
        br_tick(barrage);
    }

    ASSERT_EQ(barrage->activeCount, 0);

    for (int i = 0; i < 10; ++i)
    {
        br_tick(barrage);
    }

    ASSERT_EQ(barrage->activeCount, 0);

    br_deleteBarrage(barrage);

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

    struct Barrage* barrage = br_createBarrageFromScript(script, 320.0f, 120.0f);

    ASSERT_EQ(barrage->activeCount, 1);
    br_tick(barrage);
    ASSERT_EQ(barrage->activeCount, 5);
    br_tick(barrage);
    ASSERT_EQ(barrage->activeCount, 0);

    br_deleteBarrage(barrage);

    PASS();
}

TEST StorageTest()
{
    const char* script =
        "function main()\n"
        "    value = loadFloat('BarrageTestValue')\n"
        "    if (value == 20.0) then\n"
        "         setPosition(20.0, 10.0)\n"
        "    end\n"
        "end\n";

    struct Barrage* barrage = br_createBarrageFromScript(script, 320.0f, 120.0f);

    const char* key = "BarrageTestValue";
    br_storeFloat(barrage, key, 20.0f);

    br_tick(barrage);

    ASSERT(barrage->bullets[0].x == 20.0f);
    ASSERT(barrage->bullets[0].y == 10.0f);

    br_deleteBarrage(barrage);

    PASS();
}

SUITE(Bullet_Functionality)
{
    RUN_TEST(BasicMovementTest);
    RUN_TEST(VanishTest);
    RUN_TEST(LaunchTest);

    RUN_TEST(StorageTest);
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
