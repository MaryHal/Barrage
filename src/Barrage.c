#include <barrage/Barrage.h>
#include <barrage/BulletLua.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <barrage/MathUtils.h>
#include <barrage/SpacialPartition.h>

struct Bullet*  g_bullet  = NULL;
struct Barrage* g_barrage = NULL;
lua_State* g_L = NULL;

lua_State* br_initGlobalLuaState_()
{
    if (g_L == NULL)
    {
        g_L = luaL_newstate();

        // Register Bullet functions.
        br_registerLuaFunctions_(g_L);

        //Open all lua standard libraries.
        luaL_openlibs(g_L);
    }

    return g_L;
}

struct Barrage* br_createBarrage(struct Barrage* barrage)
{
    if (barrage == NULL)
    {
        barrage = (struct Barrage*)malloc(sizeof(struct Barrage));
    }

#if NDEBUG
    // Initialize all our data -- hush valgrind
    memset(barrage, 0, sizeof(struct Barrage));
#endif

    // Create a new lua state (if it's the first one created), otherwise get the
    // global lua state.
    barrage->L = br_initGlobalLuaState_();

    barrage->index = 0;
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

void br_deleteBarrage(struct Barrage* barrage, bool doFree)
{
    // TODO: If possible, More carefully determine which function references to unref.
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);
    }

    // Since we are using a global lua state, don't destroy it if a single
    // barrage is destroyed.

    // From the Lua documentation: "On several platforms, you may not need to
    // call [lua_close], because all resources are naturally released when the
    // host program ends. On the other hand..."

    // So we're all good, right?

    /* lua_close(barrage->L); */

     if (doFree)
     {
         free(barrage);
     }
}

struct Bullet* br_pushBarrageFunctions_(struct Barrage* barrage)
{
    // We expect the script to return a table to us that contains an `onLoad`
    // function and a `main` function.

    // Push a copy of the script's table to the top of the lua stack since we
    // need to grab two things from it.
    lua_pushvalue(barrage->L, -1);

    // Pop the table from the stack and replace it with the function at
    // table["onLoad"].
    lua_pushstring(barrage->L, "onLoad");
    lua_gettable(barrage->L, -2);

    // Run function on load(if it exists)
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

    // Create a new bullet and attach the `main` function to it.
    struct Bullet* b = br_getFreeBullet_(barrage);

    // Pop the table from the stack and replace it with the function at
    // table["main"].
    lua_pushstring(barrage->L, "main");
    lua_gettable(barrage->L, -2);

    // Pop the function off the stack and create a reference to it.
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);

    barrage->activeCount++;

    return b;
}

void br_createBulletFromFile(struct Barrage* barrage,
                             const char* filename,
                             float originX, float originY, int model)
{
    // Eval file
    if (luaL_dofile(barrage->L, filename))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    struct Bullet* initialBullet = br_pushBarrageFunctions_(barrage);

    bl_setPosition(initialBullet, originX, originY);
    bl_setModel(initialBullet, model);
}

void br_createBulletFromScript(struct Barrage* barrage,
                               const char* script,
                               float originX, float originY, int model)
{
    // Run the inline script.
    if (luaL_dostring(barrage->L, script))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    struct Bullet* initialBullet = br_pushBarrageFunctions_(barrage);

    bl_setPosition(initialBullet, originX, originY);
    bl_setModel(initialBullet, model);
}

void br_createBullet(struct Barrage* barrage,
                     float x, float y, float vx, float vy,
                     int luaFuncRef, int type)
{
    struct Bullet* b = &barrage->queue.bullets[barrage->queue.size];

    bl_setBulletData(b, x, y, vx, vy);
    bl_setLuaFunction(b, luaFuncRef);

    bl_setModel(b, type);

    barrage->queue.size++;
}

struct Bullet* br_getFreeBullet_(struct Barrage* barrage)
{
    assert(barrage->activeCount < MAX_BULLETS);

    struct Bullet* b = barrage->firstAvailable;
    barrage->firstAvailable = b->next;

    bl_setBullet(b);
    bl_setLuaFunction(b, LUA_NOREF);

    return b;
}

void br_addQueuedBullets_(struct Barrage* barrage)
{
    for (size_t i = 0; i <  barrage->queue.size; ++i)
    {
        // Copy data from queue
        bl_copyBullet(br_getFreeBullet_(barrage), &barrage->queue.bullets[i]);
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

bool br_tick(struct Barrage* barrage, struct SpacialPartition* sp)
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

    for (barrage->index = 0;
         barrage->processedCount < barrage->activeCount && barrage->index < MAX_BULLETS;
         barrage->index++)
    {
        // Make sure the lua interface knows which bullet is currently being updated.
        g_bullet = &barrage->bullets[barrage->index];

        // Run lua function.
        if (!bl_isDead(&barrage->bullets[barrage->index]))
        {
            // Push lua function ref to the top of the lua stack.
            lua_rawgeti(barrage->L, LUA_REGISTRYINDEX,
                        barrage->bullets[barrage->index].luaFuncRef);

            // Let's branch more! Retire nullFunc by allowing nil to be passed as a function
            // reference.
            if (!lua_isnil(barrage->L, -1))
            {
#if NDEBUG
                lua_call(barrage->L, 0, 0);
#else
                if (lua_pcall(barrage->L, 0, 0, 0))
                {
                    luaL_error(barrage->L, "[%s]", lua_tostring(barrage->L, -1));
                }
#endif
            }
            else
            {
                lua_pop(barrage->L, 1);
            }

            barrage->bullets[barrage->index].frame++;
            barrage->processedCount++;

            // TODO: Check if out of bounds or bullet is dead
            if (bl_isDead(&barrage->bullets[barrage->index]))
            {
                // Remove function reference from bullet and Lua State.
                /* luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[barrage->index].luaFuncRef); */

                // Re-add this bullet to the free list.
                bl_setNext(&barrage->bullets[barrage->index], barrage->firstAvailable);
                barrage->firstAvailable = &barrage->bullets[barrage->index];

                barrage->killCount++;

                continue;
            }

            // Update Position
            barrage->bullets[barrage->index].x += barrage->bullets[barrage->index].vx;
            barrage->bullets[barrage->index].y += barrage->bullets[barrage->index].vy;

            // Finally, add this bullet to our collision detection system.
            if (sp != NULL)
            {
                br_addBullet(sp, &barrage->bullets[barrage->index]);
            }
        }
        else // We're dead! Don't do anything!
        {
            // TODO: Profile this loop to see if saving memory hampers speed. Since bullet data is
            // unioned with our a linked list node data, we need to skip updating this bullet's
            // position if dead (or else we'll secretly be doing pointer arithmetic).
            continue;
        }
    }

    bool bulletLaunched = barrage->queue.size > 0;

    br_addQueuedBullets_(barrage);
    barrage->activeCount -= barrage->killCount;

    barrage->index = 0;
    barrage->processedCount = 0;

    return bulletLaunched;
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
    return barrage->processedCount < barrage->activeCount && barrage->index < MAX_BULLETS;
}

struct Bullet* br_yield(struct Barrage* barrage)
{
    // Skip over all the dead bullets and pray we don't go out of bounds.
    while (bl_isDead(&barrage->bullets[barrage->index]))
    {
        barrage->index++;

        assert(barrage->index < MAX_BULLETS);
    }

    barrage->processedCount++;
    barrage->index++;
    return &barrage->bullets[barrage->index - 1];
}

void br_resetHasNext(struct Barrage* barrage)
{
    barrage->index = 0;
    barrage->processedCount = 0;
}

size_t br_countAlive(struct Barrage* barrage)
{
    size_t count = 0;
    for (size_t i = 0; i < MAX_BULLETS; ++i)
    {
        if (bl_isDead(&barrage->bullets[i]))
        {
            count++;
        }
    }

    return count;
}

void br_aimAtTarget(struct Barrage* barrage, struct Bullet* current)
{
    bl_aimAtPoint(current, barrage->playerX, barrage->playerY);
}

void br_launch(struct Barrage* barrage, struct Bullet* current,
               float dir, float speed, int luaFuncRef, int type)
{
    if (!bl_isDying(current))
    {
        float vx =  speed * sin(dir);
        float vy = -speed * cos(dir);
        br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef, type);
    }
}

void br_launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                       float speed, int luaFuncRef, int type)
{
    if (!bl_isDying(current))
    {
        float dir = bl_getAimDirection(current, barrage->playerX, barrage->playerY);
        float vx =  speed * sin(dir);
        float vy = -speed * cos(dir);

        br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef, type);
    }
}

void br_launchCircle(struct Barrage* barrage, struct Bullet* current,
                     int segments, float speed, int luaFuncRef, int type)
{
    if (!bl_isDying(current))
    {
        float segRad = bl_PI * 2 / segments;

        for (int i = 0; i < segments; ++i)
        {
            float vx =  speed * sin(segRad * i);
            float vy = -speed * cos(segRad * i);

            br_createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef, type);
        }
    }
}
