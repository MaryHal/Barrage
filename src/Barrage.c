#include <bl/Barrage.h>
#include <bl/BulletLua.h>

#include <stdlib.h>

#include <assert.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

struct Bullet*  g_bullet  = NULL;
struct Barrage* g_barrage = NULL;

void createBarrage(struct Barrage* barrage)
{
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

    barrage->playerX = 0.0f;
    barrage->playerY = 0.0f;

    barrage->rank = 0.8f;
}

void deleteBarrage(struct Barrage* barrage)
{
    lua_close(barrage->L);
}

struct Bullet* getFreeBullet(struct Barrage* barrage)
{
    assert(barrage->activeCount < MAX_BULLETS);

    struct Bullet* b = barrage->firstAvailable;
    barrage->firstAvailable = b->next;

    barrage->activeCount++;

    bl_setBullet(b);
    bl_setLuaFunction(b, LUA_NOREF);

    return b;
}

void createBarrageFromFile(struct Barrage* barrage,
                           const char* filename,
                           float originX, float originY)
{
    createBarrage(barrage);

    /* luaL_loadfile(barrage->L, filename); */
    luaL_dofile(barrage->L, filename);

    struct Bullet* b = getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);
}

void createBarrageFromScript(struct Barrage* barrage,
                             const char* script,
                             float originX, float originY)
{
    createBarrage(barrage);

    luaL_dostring(barrage->L, script);

    struct Bullet* b = getFreeBullet(barrage);
    bl_setPosition(b, originX, originY);

    // Set lua function
    lua_getglobal(barrage->L, "main");
    int ref = luaL_ref(barrage->L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(b, ref);
}

void createBullet(struct Barrage* barrage,
                  float x, float y, float vx, float vy,
                  int luaFuncRef)
{
    struct Bullet* b = getFreeBullet(barrage);
    bl_setBulletData(b, x, y, vx, vy);

    // Set lua function
    bl_setLuaFunction(b, luaFuncRef);
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

    int killed = 0;

    for (size_t i = 0; i < barrage->activeCount; ++i)
    {
        // Make sure the lua interface knows which bullet is currently being updated.
        g_bullet = &barrage->bullets[i];

        // Run lua function.
        lua_rawgeti(barrage->L, LUA_REGISTRYINDEX, barrage->bullets[i].luaFuncRef);
        lua_call(barrage->L, 0, 0);

        bl_update(&barrage->bullets[i]);

        // TODO: Check if out of bounds or bullet is dead
        if (0)
        {
            bl_setNext(&barrage->bullets[i], barrage->firstAvailable);
            barrage->firstAvailable = &barrage->bullets[i];

            killed++;
        }
    }

    barrage->activeCount -= killed;

    barrage->currentIndex = 0;
}

int nextAvailable(struct Barrage* barrage)
{
    return barrage->currentIndex == barrage->activeCount;
}

struct Bullet* yield(struct Barrage* barrage)
{
    return &barrage->bullets[barrage->currentIndex++];
}
