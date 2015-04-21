#include <barrage/Barrage.h>
#include <barrage/BulletLua.h>
#include <barrage/LuaUtils.h>

#include <stdlib.h>

#include <math.h>
#include <barrage/MathUtils.h>

#include <assert.h>

struct Bullet*  g_bullet  = NULL;
struct Barrage* g_barrage = NULL;

struct Barrage* createBarrage()
{
    struct Barrage* barrage = (struct Barrage*)malloc(sizeof(struct Barrage));

    // Create a new lua state
    barrage->L = luaL_newstate();
    registerLuaFunctions(barrage->L);

    // Open all lua standard libraries
    luaL_openlibs(barrage->L);

    barrage->currentIndex = 0;
    barrage->activeCount = 0;

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

void deleteBarrage(struct Barrage* barrage)
{
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);
    }

    lua_close(barrage->L);
    free(barrage);
}

struct Barrage* createBarrageFromFile(const char* filename,
                                      float originX, float originY)
{
    struct Barrage* barrage = createBarrage();

    /* luaL_loadfile(barrage->L, filename); */
    if (luaL_dofile(barrage->L, filename))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    struct Bullet* b = getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);

    barrage->activeCount++;

    return barrage;
}

struct Barrage* createBarrageFromScript(const char* script,
                                        float originX, float originY)
{
    struct Barrage* barrage = createBarrage();

    if (luaL_dostring(barrage->L, script))
    {
        luaL_error(barrage->L, "%s", lua_tostring(barrage->L, -1));
    }

    struct Bullet* b = getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);

    barrage->activeCount++;

    return barrage;
}

void createBullet(struct Barrage* barrage,
                  float x, float y, float vx, float vy,
                  int luaFuncRef)
{
    struct Bullet* b = &barrage->queue.bullets[barrage->queue.size];

    bl_setBulletData(b, x, y, vx, vy);
    bl_setLuaFunction(b, luaFuncRef);

    barrage->queue.size++;
}

struct Bullet* getFreeBullet(struct Barrage* barrage)
{
    assert(barrage->activeCount < MAX_BULLETS);

    struct Bullet* b = barrage->firstAvailable;
    barrage->firstAvailable = b->next;

    bl_setBullet(b);
    bl_setLuaFunction(b, LUA_NOREF);

    return b;
}

void addQueuedBullets(struct Barrage* barrage)
{
    for (size_t i = 0; i <  barrage->queue.size; ++i)
    {
        // Copy data from queue
        bl_copyBullet(getFreeBullet(barrage), &barrage->queue.bullets[i]);
    }

    barrage->activeCount += barrage->queue.size;
    barrage->queue.size = 0;
}

void setPlayerPosition(struct Barrage* barrage, float x, float y)
{
    barrage->playerX = x;
    barrage->playerY = y;
}

void tick(struct Barrage* barrage)
{
    // Make sure the lua interface knows which barrage is currently being updated.
    g_barrage = barrage;

    // Amount of killed bullets this frame. We want to update activeCount after all the bullets have
    // been updated. Since a new bullet can be launched during the update loop and we don't know
    // where in our bullets array a new bullet will spawn, we wouldn't know whether or not the new
    // bullet will be updated this frame or not. If we queue up bullet updates (and bullet counts)
    // after the loop, we have much more consistent behavior.
    int killed = 0;

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
            lua_rawgeti(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);
            if (lua_pcall(barrage->L, 0, 0, 0))
            {
                luaL_error(barrage->L, "[%s]", lua_tostring(barrage->L, -1));
            }

            barrage->processedCount++;

            // TODO: Check if out of bounds or bullet is dead
            if (bl_isDead(&barrage->bullets[i]))
            {
                // Remove function reference from bullet.
                /* luaL_unref(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef); */

                bl_setNext(&barrage->bullets[i], barrage->firstAvailable);
                barrage->firstAvailable = &barrage->bullets[i];

                barrage->bullets[i].turn = DEAD;

                killed++;

                // Don't update this bullet.
                continue;
            }

            bl_update(&barrage->bullets[i]);
        }
    }

    // TODO: Consider whether or not we should add new bullets after updating (here) or after
    // drawing.
    addQueuedBullets(barrage);

    barrage->activeCount -= killed;
    barrage->currentIndex = 0;
    barrage->processedCount = 0;
}

int nextAvailable(struct Barrage* barrage)
{
    return barrage->processedCount < barrage->activeCount && barrage->currentIndex < MAX_BULLETS;
}

struct Bullet* yield(struct Barrage* barrage)
{
    if (!bl_isDead(&barrage->bullets[barrage->currentIndex]))
    {
        barrage->processedCount++;
        return &barrage->bullets[barrage->currentIndex++];
    }

    return NULL;
}

void launch(struct Barrage* barrage, struct Bullet* current,
            float dir, float speed, int luaFuncRef)
{
    float vx =  speed * sin(dir);
    float vy = -speed * cos(dir);
    createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
}

void launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                    float speed, int luaFuncRef)
{
    float dir = radToDeg(bl_getAimDirection(current, barrage->playerX, barrage->playerY));
    float vx =  speed * sin(dir);
    float vy = -speed * cos(dir);

    createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
}

void launchCircle(struct Barrage* barrage, struct Bullet* current,
                  int segments, float speed, int luaFuncRef)
{
    float segRad = bl_PI * 2 / segments;

    for (int i = 0; i < segments; ++i)
    {
        float vx =  speed * sin(segRad * i);
        float vy = -speed * cos(segRad * i);

        createBullet(barrage, current->x, current->y, vx, vy, luaFuncRef);
    }
}
