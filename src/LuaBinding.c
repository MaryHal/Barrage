#include <barrage/Bullet.h>
#include <barrage/Barrage.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

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
    ud->barrage = createBarrageFromFile(filename, originX, originY);

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
        deleteBarrage(ud->barrage);
    }
    ud->barrage = NULL;

    return 0;
}

static int ud_barrage_setPlayerPosition(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    float playerX = luaL_checknumber(L, 2);
    float playerY = luaL_checknumber(L, 3);

    setPlayerPosition(ud->barrage, playerX, playerY);

    return 0;
}

static int ud_barrage_tick(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    tick(ud->barrage);

    return 0;
}

static int ud_barrage_nextAvailable(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    lua_pushboolean(L, nextAvailable(ud->barrage));

    return 1;
}

static int ud_barrage_yield(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    struct Bullet* b = yield(ud->barrage);
    lua_pushnumber(L, b->x);
    lua_pushnumber(L, b->y);

    return 2;
}

static const struct luaL_Reg ud_barrage_methods[] =
{
    { "setPlayerPosition", &ud_barrage_setPlayerPosition },
    { "tick", &ud_barrage_tick },
    { "nextAvailable", &ud_barrage_nextAvailable },
    { "yield", &ud_barrage_yield },
    { "__gc", &ud_barrage_destroy },
    { NULL, NULL }
};

static const struct luaL_Reg ud_barrage_functions[] =
{
    { "new", &ud_barrage_newBarrage },
    { NULL, NULL }
};

int luaopen_libbarrage(lua_State* L)
{
    /* Create the metatable and put it on the stack. */
    luaL_newmetatable(L, "Barrage");

    /* Duplicate the metatable on the stack (We know have 2). */
    lua_pushvalue(L, -1);

    /* Pop the first metatable off the stack and assign it to __index
     * of the second one. We set the metatable for the table to itself.
     * This is equivalent to the following in lua:
     * metatable = {}
     * metatable.__index = metatable
     */
    lua_setfield(L, -2, "__index");

    /* Set the methods to the metatable that should be accessed via object:func */
    luaL_setfuncs(L, ud_barrage_methods, 0);

    /* Register the object.func functions into the table that is at the top of the
     * stack. */
    luaL_newlib(L, ud_barrage_functions);

    return 1;
}
