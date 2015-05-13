#include <barrage/Bullet.h>
#include <barrage/Barrage.h>
#include <barrage/SpacialPartition.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Lua < 5.2 compatibility functions
#include <barrage/LuaCompat.h>

#include <stdlib.h>
#include <stdbool.h>

struct Barrage_user_data
{
        struct Barrage* barrage;
};

struct SpacialPartition_user_data
{
        struct SpacialPartition* sp;
};

static int ud_barrage_create(lua_State* L)
{
    // Have lua allocate some data for our Barrage struct.
    struct Barrage_user_data* ud = (struct Barrage_user_data*) lua_newuserdata(L, sizeof(*ud));
    ud->barrage = br_createBarrage(NULL);

    luaL_getmetatable(L, "Barrage");

    // Set metatable on userdata
    lua_setmetatable(L, -2);

    return 1;
}

static int ud_barrage_createFromFile(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    const char* filename = luaL_checkstring(L, 2);
    float originX = luaL_checknumber(L, 3);
    float originY = luaL_checknumber(L, 4);

    // Fourth argument is bullet "model".
    int model = 0;
    if (lua_gettop(L) > 5)
    {
        model = luaL_checkinteger(L, 5);
    }

    br_createBulletFromFile(ud->barrage, filename, originX, originY, model);

    return 0;
}

static int ud_barrage_createFromBuffer(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    const char* buffer = luaL_checkstring(L, 2);
    float originX = luaL_checknumber(L, 3);
    float originY = luaL_checknumber(L, 4);

    // Fourth argument is bullet "model".
    int model = 0;
    if (lua_gettop(L) > 5)
    {
        model = luaL_checkinteger(L, 5);
    }

    br_createBulletFromScript(ud->barrage, buffer, originX, originY, model);

    return 0;
}

static int ud_barrage_destroy(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");

    br_deleteBarrage(ud->barrage, true);
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
    struct SpacialPartition_user_data* udsp = (struct SpacialPartition_user_data*)luaL_checkudata(L, 2, "Barrage");

    lua_pushboolean(L, br_tick(ud->barrage, udsp->sp));

    return 1;
}

static int ud_barrage_vanishAll(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    br_vanishAll(ud->barrage);

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

    if (bl_isDying(b))
    {
        lua_pushnumber(L, 1 - ((b->frame + 1) / 30.0f));
    }
    else
    {
        lua_pushnumber(L, 1.0f);
    }

    // Since we are we are technically referring to an array from lua, let's start our
    // indices at 1.
    lua_pushinteger(L, b->model + 1);

    return 6;
}

static int ud_barrage_resetHasNext(lua_State* L)
{
    struct Barrage_user_data* ud = (struct Barrage_user_data*)luaL_checkudata(L, 1, "Barrage");
    br_resetHasNext(ud->barrage);

    return 0;
}

static int ud_spacial_partition_create(lua_State* L)
{
    // Have lua allocate some data for our Barrage struct.
    struct SpacialPartition_user_data* ud = (struct SpacialPartition_user_data*) lua_newuserdata(L, sizeof(*ud));
    ud->sp = br_createSpacialPartition(NULL);

    luaL_getmetatable(L, "Barrage");

    // Set metatable on userdata
    lua_setmetatable(L, -2);

    return 1;
}

static int ud_spacial_partition_destroy(lua_State* L)
{
    struct SpacialPartition_user_data* ud = (struct SpacialPartition_user_data*)luaL_checkudata(L, 1, "Barrage");
    br_deleteSpacialPartition(ud->sp, true);

    return 0;
}

static int ud_spacial_partition_checkCollision(lua_State* L)
{
    struct SpacialPartition_user_data* ud = (struct SpacialPartition_user_data*)luaL_checkudata(L, 1, "Barrage");
    float playerX = luaL_checknumber(L, 2);
    float playerY = luaL_checknumber(L, 3);
    float playerW = luaL_checknumber(L, 4);
    float playerH = luaL_checknumber(L, 5);

    lua_pushboolean(L, br_checkCollision(ud->sp, playerX, playerY, playerW, playerH));

    return 1;
}

static int ud_spacial_partition_addModel(lua_State* L)
{
    struct SpacialPartition_user_data* ud = (struct SpacialPartition_user_data*)luaL_checkudata(L, 1, "Barrage");
    int width  = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);

    br_addModel(ud->sp, (struct Rect){ 0, 0, width, height });

    lua_pushinteger(L, ud->sp->modelCount);

    return 1;
}

static int ud_spacial_partition_getModel(lua_State* L)
{
    struct SpacialPartition_user_data* ud = (struct SpacialPartition_user_data*)luaL_checkudata(L, 1, "Barrage");
    int modelIndex = luaL_checkinteger(L, 2);

    struct Rect collisionRect = br_getModel(ud->sp, modelIndex - 1);
    lua_pushinteger(L, collisionRect.width);
    lua_pushinteger(L, collisionRect.height);

    return 2;
}

static const struct luaL_Reg ud_barrage_methods[] =
{
    { "launchFile",           &ud_barrage_createFromFile },
    { "launchBuffer",         &ud_barrage_createFromBuffer },

    { "getActiveCount",       &ud_barrage_getActiveCount },
    { "setRank",              &ud_barrage_setRank },
    { "getRank",              &ud_barrage_getRank },
    { "storeFloat",           &ud_barrage_storeFloat },
    { "setPlayerPosition",    &ud_barrage_setPlayerPosition },
    { "tick",                 &ud_barrage_tick },
    { "vanishAll",            &ud_barrage_vanishAll },
    { "hasNext",              &ud_barrage_hasNext },
    { "resetHasNext",         &ud_barrage_resetHasNext },
    { "yield",                &ud_barrage_yield },
    { "__gc",                 &ud_barrage_destroy },
    { NULL, NULL }
};

static const struct luaL_Reg ud_spacial_partition_methods[] =
{
    { "checkCollision",       &ud_spacial_partition_checkCollision },
    { "addModel",             &ud_spacial_partition_addModel },
    { "getModel",             &ud_spacial_partition_getModel },
    { "__gc",                 &ud_spacial_partition_destroy },
    { NULL, NULL }
};

static const struct luaL_Reg ud_barrage_functions[] =
{
    { "newBarrage",           &ud_barrage_create },
    { "newSpacialPartition",  &ud_spacial_partition_create },
    { NULL, NULL }
};

int luaopen_barrageC(lua_State* L)
{
    // Create the metatable and put it on the stack.
    luaL_newmetatable(L, "Barrage");

    // Duplicate the metatable on the stack
    lua_pushvalue(L, -1);

    // Pop the first metatable off the stack and assign it to __index of the second one. We set the
    // metatable for the table to itself. This is equivalent to the following in lua:
    lua_setfield(L, -2, "__index");

    // Set the methods to the metatable that should be accessed via object:func (Barrage)
    luaL_setfuncs(L, ud_barrage_methods, 0);

    // Do the above steps again for another metatable "object" (SpacialPartition)
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, ud_spacial_partition_methods, 0);

    // Register the object.func functions into the table that is at the top of the stack.
    luaL_newlib(L, ud_barrage_functions);

    return 1;
}
