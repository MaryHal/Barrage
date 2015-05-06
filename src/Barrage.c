#include <barrage/Barrage.h>
#include <barrage/BulletLua.h>
#include <barrage/LuaUtils.h>

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <barrage/MathUtils.h>
#include <barrage/SpacialPartition.h>

struct Bullet*  g_bullet  = NULL;
struct Barrage* g_barrage = NULL;

struct Barrage* br_createBarrage_()
{
    struct Barrage* barrage = (struct Barrage*)malloc(sizeof(struct Barrage));

    // Create a new lua state
    barrage->L = luaL_newstate();
    registerLuaFunctions(barrage->L);

    // Open all lua standard libraries
    luaL_openlibs(barrage->L);

    barrage->currentIndex = 0;
    barrage->processedCount = 0;

    barrage->activeCount = 0;
    barrage->killCount = 0;

    // Setup our linked list of free bullets
    barrage->firstAvailable = &barrage->bullets[0];

    // Create our free list
    for (size_t i = 0; i < MAX_BULLETS - 1; ++i)
    {
        bl_setNext(&barrage->bullets[i], &barrage->bullets[i + 1]);
    }

    // Terminate our linked list
    bl_setNext(&barrage->bullets[MAX_BULLETS - 1], NULL);

    barrage->queue.size = 0;

    barrage->playerX = 0.0f;
    barrage->playerY = 0.0f;

    barrage->rank = 0.8f;

    return barrage;
}

void br_deleteBarrage(struct Barrage* barrage)
{
    // TODO: Determine which function references to unref.
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);
    }

    lua_close(barrage->L);
    free(barrage);
}

void br_runOnLoadFunc_(struct Barrage* barrage)
{
    const char* funcName = "onLoad";

    // Run function on load(if it exists)
    lua_getglobal(barrage->L, funcName);
    if (lua_isfunction(barrage->L, -1))
    {
        if (lua_pcall(barrage->L, 0, 0, 0))
        {
            luaL_error(barrage->L, "[%s]", lua_tostring(barrage->L, -1));
        }
    }
    else
    {
        lua_pop(barrage->L, 1);
    }
}

struct Barrage* br_createBarrageFromFile(const char* filename,
                                         float originX, float originY)
{
    struct Barrage* barrage = br_createBarrage_();

    /* luaL_loadfile(barrage->L, filename); */
    if (luaL_dofile(barrage->L, filename))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    br_runOnLoadFunc_(barrage);

    struct Bullet* b = br_getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);

    barrage->activeCount++;

    return barrage;
}

struct Barrage* br_createBarrageFromScript(const char* script,
                                           float originX, float originY)
{
    struct Barrage* barrage = br_createBarrage_();

    if (luaL_dostring(barrage->L, script))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    br_runOnLoadFunc_(barrage);

    struct Bullet* b = br_getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);

    barrage->activeCount++;

    return barrage;
}

void br_createBullet(struct Barrage* barrage,
                     float x, float y, float vx, float vy,
                     int luaFuncRef)
{
    struct Bullet* b = &barrage->queue.bullets[barrage->queue.size];

    bl_setBulletData(b, x, y, vx, vy);
    bl_setLuaFunction(b, luaFuncRef);

    barrage->queue.size++;
}

struct Bullet* br_getFreeBullet(struct Barrage* barrage)
{
    assert(barrage->activeCount < MAX_BULLETS);

    struct Bullet* b = barrage->firstAvailable;
    barrage->firstAvailable = b->next;

    bl_setBullet(b);
    bl_setLuaFunction(b, LUA_NOREF);

    return b;
}

void br_addQueuedBullets(struct Barrage* barrage)
{
    for (size_t i = 0; i <  barrage->queue.size; ++i)
    {
        // Copy data from queue
        bl_copyBullet(br_getFreeBullet(barrage), &barrage->queue.bullets[i]);
    }

    barrage->activeCount += barrage->queue.size;
    barrage->queue.size = 0;
}

void br_setRank(struct Barrage* barrage, float rank)
{
    barrage->rank = rank;
}

float br_getRank(struct Barrage* barrage)
{
    return barrage->rank;
}

void br_storeFloat(struct Barrage* barrage, const char* key, float value)
{
    (void)key;
    lua_pushlightuserdata(barrage->L, (void*)"BarrageTestValue");
    lua_pushnumber(barrage->L, value);
    lua_settable(barrage->L, LUA_REGISTRYINDEX);
}

float br_getFloat(struct Barrage* barrage, const char* key)
{
    (void)key;
    lua_pushlightuserdata(barrage->L, (void*)"BarrageTestValue");
    lua_gettable(barrage->L, LUA_REGISTRYINDEX);
    float value = lua_tonumber(barrage->L, -1);

    return value;
}

void br_setPlayerPosition(struct Barrage* barrage, float x, float y)
{
    barrage->playerX = x;
    barrage->playerY = y;
}

void br_tick(struct Barrage* barrage, struct SpacialPartition* sp)
{
    if (sp != NULL)
        br_clear(sp);

    // Make sure the lua interface knows which barrage is currently being updated.
    g_barrage = barrage;

    // Amount of killed bullets this frame. We want to update activeCount after all the bullets have
    // been updated. Since a new bullet can be launched during the update loop and we don't know
    // where in our bullets array a new bullet will spawn, we wouldn't know whether or not the new
    // bullet will be updated this frame or not. If we queue up bullet updates (and bullet counts)
    // after the loop, we have much more consistent behavior.
    barrage->killCount = 0;

    barrage->processedCount= 0;

    for (size_t i = 0;
         barrage->processedCount < barrage->activeCount && i < MAX_BULLETS;
         ++i)
    {
        // Make sure the lua interface knows which bullet is currently being updated.
        g_bullet = &barrage->bullets[i];

        // Run lua function.
        if (!bl_isDead(&barrage->bullets[i]))
        {
            // Push lua function ref to the top of the lua stack.
            lua_rawgeti(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);

#if NDEBUG
            lua_call(barrage->L, 0, 0);
#else
            if (lua_pcall(barrage->L, 0, 0, 0))
            {
                luaL_error(barrage->L, "[%s]", lua_tostring(barrage->L, -1));
            }
#endif

            barrage->bullets[i].turn++;
            barrage->processedCount++;

            // TODO: Check if out of bounds or bullet is dead
            if (bl_isDead(&barrage->bullets[i]))
            {
                // Remove function reference from bullet.
                /* luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef); */

                bl_setNext(&barrage->bullets[i], barrage->firstAvailable);
                barrage->firstAvailable = &barrage->bullets[i];

                barrage->killCount++;

                continue;
            }
        }
        else
        {
            // TODO: Profile this loop to see if saving memory hampers speed. Since bullet data is
            // unioned with our a linked list node data, we need to skip updating this bullet's
            // position if dead (or else we'll secretly be doing pointer arithmetic).
            continue;
        }

        // Update Position
        barrage->bullets[i].x += barrage->bullets[i].vx;
        barrage->bullets[i].y += barrage->bullets[i].vy;

        if (sp != NULL)
            br_addBullet(sp, &barrage->bullets[i]);
    }

    // TODO: Consider whether or not we should add new bullets after updating (here) or after
    // drawing.
    br_addQueuedBullets(barrage);
    barrage->activeCount -= barrage->killCount;

    barrage->currentIndex = 0;
    barrage->processedCount = 0;
}

void br_vanishAll(struct Barrage* barrage)
{
    br_resetHasNext(barrage);

    while (br_hasNext(barrage))
    {
        struct Bullet* b = br_yield(barrage);
        bl_vanish(b, DEFAULT_FRAMES_UNTIL_DEATH);
    }
}

int br_hasNext(struct Barrage* barrage)
{
    return barrage->processedCount < barrage->activeCount && barrage->currentIndex < MAX_BULLETS;
}

struct Bullet* br_yield(struct Barrage* barrage)
{
    // Skip over all the dead bullets and pray we don't go out of bounds.
    while (bl_isDead(&barrage->bullets[barrage->currentIndex]))
    {
        barrage->currentIndex++;

        assert(barrage->currentIndex < MAX_BULLETS);
    }

    barrage->processedCount++;
    barrage->currentIndex++;
    return &barrage->bullets[barrage->currentIndex - 1];
}

void br_resetHasNext(struct Barrage* barrage)
{
    barrage->currentIndex = 0;
    barrage->processedCount = 0;
}

void br_aimAtTarget(struct Barrage* barrage, struct Bullet* current)
{
    bl_aimAtPoint(current, barrage->playerX, barrage->playerY);
}

void br_launch(struct Barrage* barrage, struct Bullet* current,
               float dir, float speed, int luaFuncRef)
{
    if (!bl_isDying(current))
    {
        float vx =  speed * sin(dir);
        float vy = -speed * cos(dir);
        br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
    }
}

void br_launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                       float speed, int luaFuncRef)
{
    if (!bl_isDying(current))
    {
        float dir = bl_getAimDirection(current, barrage->playerX, barrage->playerY);
        float vx =  speed * sin(dir);
        float vy = -speed * cos(dir);

        br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
    }
}

void br_launchCircle(struct Barrage* barrage, struct Bullet* current,
                     int segments, float speed, int luaFuncRef)
{
    if (!bl_isDying(current))
    {
        float segRad = bl_PI * 2 / segments;

        for (int i = 0; i < segments; ++i)
        {
            float vx =  speed * sin(segRad * i);
            float vy = -speed * cos(segRad * i);

            br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
        }
    }
}
