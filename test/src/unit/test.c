#include <greatest.h>

#include <barrage/Barrage.h>
#include <barrage/SpacialPartition.h>

#include <math.h>
#include <stdbool.h>

bool inFloatRange(float value, float expected)
{
    const float TOLERANCE = 1e-4;
    return fabs(value/expected - 1.0f) < TOLERANCE;
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
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

    ASSERT(inFloatRange(barrage.bullets[0].x, 320.0f) &&
           inFloatRange(barrage.bullets[0].y, 120.0f));

    br_tick(&barrage, NULL);

    ASSERT(inFloatRange(barrage.bullets[0].x, 330.0f) &&
           inFloatRange(barrage.bullets[0].y, 130.0f));

    br_tick(&barrage, NULL);

    ASSERT(inFloatRange(barrage.bullets[0].x, 335.0f) &&
           inFloatRange(barrage.bullets[0].y, 135.0f));

    br_deleteBarrage(&barrage, false);

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
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

    // Set LuaFuncRef to point to nil. I hope we don't segfault!
    br_tick(&barrage, NULL);
    br_tick(&barrage, NULL);
    br_tick(&barrage, NULL);

    br_deleteBarrage(&barrage, false);

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
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

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

    br_deleteBarrage(&barrage, false);

    PASS();
}

TEST LaunchTest()
{
    const char* script =
        "launchTest = {\n"
        "   main = function ()\n"
        "      launch(1, 45, 1, kill)\n"
        "      launch(1, 45, 1, kill)\n"
        "      launch(1, 45, 1, kill)\n"
        "      launch(1, 45, 1, kill)\n"
        "      launch(1, 45, 1, kill)\n"
        "      kill()\n"
        "   end\n"
        "}\n"
        "return launchTest\n";

    struct Barrage barrage;
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

    ASSERT_EQ(barrage.activeCount, 1);
    br_tick(&barrage, NULL);
    ASSERT_EQ(barrage.activeCount, 5);
    br_tick(&barrage, NULL);
    ASSERT_EQ(barrage.activeCount, 0);

    br_deleteBarrage(&barrage, false);

    PASS();
}

TEST ModelTest()
{
    const char* script =
        "modelTest = {\n"
        "   main = function ()\n"
        "   end\n"
        "}\n"
        "return modelTest\n";

    struct Barrage barrage;
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 4.0f, 4.0f, 1);

    struct SpacialPartition sp;
    br_createSpacialPartition(&sp);
    br_addModel(&sp, (struct Rect){0, 0, 100, 100});

    br_tick(&barrage, &sp);
    ASSERT(br_checkCollision(&sp, 30.0f, 30.0f, 4.0f, 4.0f) == true);

    br_deleteBarrage(&barrage, false);
    br_deleteSpacialPartition(&sp, false);

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
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

    const char* key = "BarrageTestValue";
    br_storeFloat(&barrage, key, 20.0f);

    br_tick(&barrage, NULL);

    ASSERT(inFloatRange(barrage.bullets[0].x, 20.0f) &&
           inFloatRange(barrage.bullets[0].y, 10.0f));

    br_deleteBarrage(&barrage, false);

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
    br_createBarrage(&barrage);
    br_createBulletFromScript(&barrage, script, 320.0f, 120.0f, 0);

    struct SpacialPartition sp;
    br_createSpacialPartition(&sp);

    ASSERT(br_checkCollision(&sp, 0.0f, 0.0f, 4.0f, 4.0f) == false);

    br_tick(&barrage, &sp);

    ASSERT(br_checkCollision(&sp, 18.0f, 8.0f, 4.0f, 4.0f) == true);
    ASSERT(br_checkCollision(&sp, 13.0f, 8.0f, 4.0f, 4.0f) == false);

    br_deleteBarrage(&barrage, false);
    br_deleteSpacialPartition(&sp, false);

    PASS();
}

SUITE(Bullet_Functionality)
{
    RUN_TEST(BasicMovementTest);
    RUN_TEST(NilFuncTest);
    RUN_TEST(VanishTest);
    RUN_TEST(LaunchTest);

    RUN_TEST(ModelTest);

    RUN_TEST(StorageTest);

    RUN_TEST(BasicCollisionTest);
}

/* Add defcreateions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN(); /* command-line arguments, createialization. */
    {
        RUN_SUITE(Bullet_Functionality);
    }
    GREATEST_MAIN_END();   /* display results */
}
