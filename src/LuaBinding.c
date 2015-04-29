#include <barrage/Bullet.h>
#include <barrage/Barrage.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Lua < 5.2 compatibility functions
#include <barrage/LuaCompat.h>
#include <barrage/RandCompat.h>

#include <stdlib.h>

struct Barrage_user_data
{
        struct Barrage* barrage;
};

static int ud_barrage_newBarrage(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    float originX = luaL_checknumber(L, 2);
    float originY = luaL_checknumber(L, 3);

    // TODO: Argument validation.

    // Have lua allocate some data for our Barrage struct.
    struct Barrage_user_data* ud = (struct Barrage_user_data*) lua_newuserdata(L, sizeof(*ud));
    ud->barrage = br_createBarrageFromFile(filename, originX, originY);

    luaL_getmetatable(L, "Barrage");

    // Set metatable on userdata
    lua_setmetatable(L, -2);

    return 1;
}

static int ud_barrage_destroy(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    if (ud->barrage != NULL)
    {
        br_deleteBarrage(ud->barrage);
    }
    ud->barrage = NULL;

    return 0;
}

static int ud_barrage_getActiveCount(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    lua_pushinteger(L, ud->barrage->activeCount);

    return 1;
}

static int ud_barrage_setRank(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    float rank = luaL_checknumber(L, 2);

    /* br_setRank(ud->barrage, rank); */
    ud->barrage->rank = rank;

    return 0;
}

static int ud_barrage_getRank(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    lua_pushnumber(L, ud->barrage->rank);

    return 1;
}

static int ud_barrage_storeFloat(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    const char* key = luaL_checkstring(L, 1);
    float value = luaL_checknumber(L, 2);

    br_storeFloat(ud->barrage, key, value);

    return 0;
}

static int ud_barrage_setPlayerPosition(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    float playerX = luaL_checknumber(L, 2);
    float playerY = luaL_checknumber(L, 3);

    br_setPlayerPosition(ud->barrage, playerX, playerY);

    return 0;
}

static int ud_barrage_tick(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    br_tick(ud->barrage);

    return 0;
}

static int ud_barrage_hasNext(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    lua_pushboolean(L, br_hasNext(ud->barrage));

    return 1;
}

static int ud_barrage_yield(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    struct Bullet* b = br_yield(ud->barrage);
    lua_pushnumber(L, b->x);
    lua_pushnumber(L, b->y);

    lua_pushnumber(L, b->vx);
    lua_pushnumber(L, b->vy);

    lua_pushinteger(L, b->turn);

    return 5;
}

static const struct luaL_Reg ud_barrage_methods[] =
{
    { "getActiveCount",     &ud_barrage_getActiveCount },
    { "setRank",            &ud_barrage_setRank },
    { "getRank",            &ud_barrage_getRank },
    { "storeFloat",         &ud_barrage_storeFloat },
    { "setPlayerPosition",  &ud_barrage_setPlayerPosition },
    { "tick",               &ud_barrage_tick },
    { "hasNext",            &ud_barrage_hasNext },
    { "yield",              &ud_barrage_yield },
    { "__gc",               &ud_barrage_destroy },
    { NULL, NULL }
};

static const struct luaL_Reg ud_barrage_functions[] =
{
    { "new",                &ud_barrage_newBarrage },
    { NULL, NULL }
};

int luaopen_barrageC(lua_State* L)
{
    // Create the metatable and put it on the stack.
    luaL_newmetatable(L, "Barrage");

    // Duplicate the metatable on the stack (We know have 2).
    lua_pushvalue(L, -1);

    // Pop the first metatable off the stack and assign it to __index of the second one. We set the
    // metatable for the table to itself. This is equivalent to the following in lua:
    //
    // metatable = {}
    // metatable.__index = metatable

    lua_setfield(L, -2, "__index");

    // Set the methods to the metatable that should be accessed via object:func
    luaL_setfuncs(L, ud_barrage_methods, 0);

    // Register the object.func functions into the table that is at the top of the stack.
    luaL_newlib(L, ud_barrage_functions);

    return 1;
}
