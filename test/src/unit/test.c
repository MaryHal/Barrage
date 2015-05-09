#include <greatest.h>

#include <barrage/Barrage.h>
#include <barrage/SpacialPartition.h>

#include <math.h>

int isInRange(float value, float expected, float tolerance)
{
    return fabs(expected - value) < tolerance;
}

TEST BasicMovementTest()
{
    const char* script =
        "basicMovement = {\n"
        "   main = function ()\n"
        "      x, y = getPosition()\n"
        "      setPosition(x + 10, y + 10)\n"
        "      setFunction(basicMovement.move)\n"
        "   end,\n"
        "   move = function ()\n"
        "      setVelocity(5.0, 5.0)\n"
        "   end\n"
        "}\n"
        "return basicMovement\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);

    ASSERT(barrage.bullets[0].x == 320.0f);
    ASSERT(barrage.bullets[0].y == 120.0f);

    br_tick(&barrage, NULL);

    ASSERT(barrage.bullets[0].x == 330.0f);
    ASSERT(barrage.bullets[0].y == 130.0f);

    br_tick(&barrage, NULL);

    ASSERT(barrage.bullets[0].x == 335.0f);
    ASSERT(barrage.bullets[0].y == 135.0f);

    br_deinitBarrage(&barrage);

    PASS();
}

TEST NilFuncTest()
{
    const char* script =
        "nilFunc = {\n"
        "   main = function ()\n"
        "      setFunction(nil)\n"
        "   end\n"
        "}\n"
        "return nilFunc\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);

    // Set LuaFuncRef to point to nil. I hope we don't segfault!
    br_tick(&barrage, NULL);
    br_tick(&barrage, NULL);
    br_tick(&barrage, NULL);

    br_deinitBarrage(&barrage);

    PASS();
}

TEST VanishTest()
{
    const char* script =
        "vanishTest = {\n"
        "   main = function ()\n"
        "      vanish()\n"
        "   end\n"
        "}\n"
        "return vanishTest\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);

    ASSERT_EQ(barrage.activeCount, 1);

    for (int i = 0; i < 35; ++i)
    {
        br_tick(&barrage, NULL);
    }

    ASSERT_EQ(barrage.activeCount, 0);

    for (int i = 0; i < 10; ++i)
    {
        br_tick(&barrage, NULL);
    }

    ASSERT_EQ(barrage.activeCount, 0);

    br_deinitBarrage(&barrage);

    PASS();
}

TEST LaunchTest()
{
    const char* script =
        "launchTest = {\n"
        "   main = function ()\n"
        "      launch(45, 1, kill)\n"
        "      launch(45, 1, kill)\n"
        "      launch(45, 1, kill)\n"
        "      launch(45, 1, kill)\n"
        "      launch(45, 1, kill)\n"
        "      kill()\n"
        "   end\n"
        "}\n"
        "return launchTest\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);

    ASSERT_EQ(barrage.activeCount, 1);
    br_tick(&barrage, NULL);
    ASSERT_EQ(barrage.activeCount, 5);
    br_tick(&barrage, NULL);
    ASSERT_EQ(barrage.activeCount, 0);

    br_deinitBarrage(&barrage);

    PASS();
}

TEST StorageTest()
{
    const char* script =
        "storageTest = {\n"
        "   main = function ()\n"
        "      value = loadFloat('BarrageTestValue')\n"
        "      if (value == 20.0) then\n"
        "         setPosition(20.0, 10.0)\n"
        "      end\n"
        "   end\n"
        "}\n"
        "return storageTest\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);

    const char* key = "BarrageTestValue";
    br_storeFloat(&barrage, key, 20.0f);

    br_tick(&barrage, NULL);

    ASSERT(barrage.bullets[0].x == 20.0f);
    ASSERT(barrage.bullets[0].y == 10.0f);

    br_deinitBarrage(&barrage);

    PASS();
}

TEST BasicCollisionTest()
{
    const char* script =
        "basicCollisionTest = {\n"
        "   main = function ()\n"
        "      setPosition(20.0, 10.0)\n"
        "   end;\n"
        "}\n"
        "return basicCollisionTest\n";

    struct Barrage barrage;
    br_initBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f);
    struct SpacialPartition* sp = br_createSpacialPartition();

    ASSERT(br_checkCollision(sp, 0.0f, 0.0f, 4.0f, 4.0f) == false);

    br_tick(&barrage, sp);

    ASSERT(br_checkCollision(sp, 20.0f, 10.0f, 4.0f, 4.0f) == true);
    ASSERT(br_checkCollision(sp, 15.0f, 10.0f, 4.0f, 4.0f) == false);

    br_deinitBarrage(&barrage);
    br_deleteSpacialPartition(sp);

    PASS();
}

SUITE(Bullet_Functionality)
{
    RUN_TEST(BasicMovementTest);
    RUN_TEST(NilFuncTest);
    RUN_TEST(VanishTest);
    RUN_TEST(LaunchTest);

    RUN_TEST(StorageTest);

    RUN_TEST(BasicCollisionTest);
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
