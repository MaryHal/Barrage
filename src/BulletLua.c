#include <barrage/BulletLua.h>

#include <barrage/Bullet.h>
#include <barrage/Barrage.h>

#include <stdio.h>

#include <barrage/MathUtils.h>

void registerLuaFunctions(lua_State* L)
{
    lua_register(L, "nullfunc", &l_nullFunc);

    lua_register(L, "setPosition", &l_setPosition);
    lua_register(L, "getPosition", &l_getPosition);

    lua_register(L, "setVelocity", &l_setVelocity);
    lua_register(L, "getVelocity", &l_getVelocity);

    lua_register(L, "setSpeedAndDirection", &l_setSpeedAndDirection);
    lua_register(L, "setSpeed", &l_setSpeed);
    lua_register(L, "setSpeedRelative", &l_setSpeedRelative);
    lua_register(L, "getSpeed", &l_getSpeed);

    lua_register(L, "setDirection", &l_setDirection);
    lua_register(L, "setDirectionRelative", &l_setDirectionRelative);

    lua_register(L, "aimAtTarget", &l_aimAtTarget);
    lua_register(L, "aimAtPoint", &l_aimAtPoint);
    lua_register(L, "getAimDirection", &l_getAimDirection);

    lua_register(L, "linearInterpolate", &l_linearInterpolate);

    lua_register(L, "getDirection", &l_getDirection);

    lua_register(L, "vanish", &l_vanish);
    lua_register(L, "kill", &l_kill);

    lua_register(L, "isDead", &l_isDead);
    lua_register(L, "isDying", &l_isDying);

    lua_register(L, "resetTurns", &l_resetTurns);
    lua_register(L, "getTurn", &l_getTurn);

    lua_register(L, "setFunction", &l_setLuaFunction);

    lua_register(L, "getRank", &l_getRank);

    lua_register(L, "getTargetPosition", &l_getTargetPosition);

    lua_register(L, "launch", &l_launch);
    lua_register(L, "launchAtTarget", &l_launchAtTarget);
    lua_register(L, "launchCircle", &l_launchCircle);
}

int l_nullFunc(lua_State* L)
{
    (void)L;
    return 0;
}

int l_setPosition(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    bl_setPosition(g_bullet, x, y);

    return 0;
}

int l_getPosition(lua_State* L)
{
    lua_pushnumber(L, g_bullet->x);
    lua_pushnumber(L, g_bullet->y);

    return 2;
}

int l_setVelocity(lua_State* L)
{
    float vx = luaL_checknumber(L, 1);
    float vy = luaL_checknumber(L, 2);

    bl_setVelocity(g_bullet, vx, vy);

    return 0;
}

int l_getVelocity(lua_State* L)
{
    lua_pushnumber(L, g_bullet->vx);
    lua_pushnumber(L, g_bullet->vy);

    return 2;
}

int l_setSpeedAndDirection(lua_State* L)
{
    float speed = luaL_checknumber(L, 1);
    float dir   = degToRad(luaL_checknumber(L, 2));
    bl_setSpeedAndDirection(g_bullet, speed, dir);

    return 0;
}

int l_setSpeed(lua_State* L)
{
    float speed = luaL_checknumber(L, 1);
    bl_setSpeed(g_bullet, speed);

    return 0;
}

int l_setSpeedRelative(lua_State* L)
{
    float speed = luaL_checknumber(L, 1);
    bl_setSpeedRelative(g_bullet, speed);

    return 0;
}

int l_getSpeed(lua_State* L)
{
    lua_pushnumber(L, bl_getSpeed(g_bullet));

    return 1;
}

int l_setDirection(lua_State* L)
{
    float dir = degToRad(luaL_checknumber(L, 1));
    bl_setDirection(g_bullet, dir);

    return 0;
}

int l_setDirectionRelative(lua_State* L)
{
    float dir = degToRad(luaL_checknumber(L, 1));
    bl_setDirectionRelative(g_bullet, dir);

    return 0;
}

int l_aimAtTarget(lua_State* L)
{
    (void)L;

    aimAtTarget(g_barrage, g_bullet);

    return 0;
}

int l_aimAtPoint(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    bl_aimAtPoint(g_bullet, x, y);

    return 0;
}

int l_getAimDirection(lua_State* L)
{
    float targetX = luaL_checknumber(L, 1);
    float targetY = luaL_checknumber(L, 2);

    lua_pushnumber(L, radToDeg(bl_getAimDirection(g_bullet, targetX, targetY)));
    return 1;
}

int l_linearInterpolate(lua_State* L)
{
    float targetX = luaL_checknumber(L, 1);
    float targetY = luaL_checknumber(L, 2);
    int steps = luaL_checkint(L, 3);

    bl_linearInterpolate(g_bullet, targetX, targetY, steps);

    return 0;
}

int l_getDirection(lua_State* L)
{
    lua_pushnumber(L, radToDeg(bl_getDirection(g_bullet)));
    return 1;
}

int l_vanish(lua_State* L)
{
    (void)L;

    bl_vanish(g_bullet);
    return 0;
}

int l_kill(lua_State* L)
{
    (void)L;

    bl_kill(g_bullet);
    return 0;
}

int l_isDead(lua_State* L)
{
    lua_pushboolean(L, bl_isDead(g_bullet));
    return 1;
}

int l_isDying(lua_State* L)
{
    lua_pushboolean(L, bl_isDying(g_bullet));
    return 1;
}

int l_resetTurns(lua_State* L)
{
    (void)L;

    bl_resetTurns(g_bullet);
    return 0;
}

int l_getTurn(lua_State* L)
{
    lua_pushinteger(L, bl_getTurn(g_bullet));
    return 1;
}

int l_setLuaFunction(lua_State* L)
{
    /* int ref = luaL_checkint(L, 1); */
    /* lua_pushinteger(L, ref); */

    /* // Release g_bullet function reference */
    /* luaL_unref(L, LUA_REGISTRYINDEX, g_bullet->luaFuncRef); */

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    bl_setLuaFunction(g_bullet, ref);

    return 0;
}

int l_getRank(lua_State* L)
{
    lua_pushnumber(L, g_barrage->rank);
    return 1;
}

int l_getTargetPosition(lua_State* L)
{
    lua_pushnumber(L, g_barrage->playerX);
    lua_pushnumber(L, g_barrage->playerY);

    return 2;
}

int l_launch(lua_State* L)
{
    float dir   = degToRad(luaL_checknumber(L, 1));
    float speed = luaL_checknumber(L, 2);

    // Third argument is a function handle.
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    launch(g_barrage, g_bullet, dir, speed, ref);

    return 0;
}

int l_launchAtTarget(lua_State* L)
{
    float speed = luaL_checknumber(L, 1);

    // Second argument is a function handle.
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    launchAtTarget(g_barrage, g_bullet, speed, ref);

    return 0;
}

int l_launchCircle(lua_State* L)
{
    int segments = luaL_checkint(L, 1);
    float speed  = luaL_checknumber(L, 2);

    // Third argument is a function handle.
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    launchCircle(g_barrage, g_bullet, segments, speed, ref);

    return 0;
}
