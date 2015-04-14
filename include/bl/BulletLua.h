#ifndef BULLETLUA_H
#define BULLETLUA_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Debug function
void stackDump (lua_State *L);

// Load our functions.
void registerLuaFunctions(lua_State* L);

// Lua wrapper functions.

// Bullet functions
int l_nullFunc(lua_State* L);

int l_setPosition(lua_State* L);
int l_getPosition(lua_State* L);

int l_setVelocity(lua_State* L);
int l_getVelocity(lua_State* L);

int l_setSpeedAndDirection(lua_State* L);
int l_setSpeed(lua_State* L);
int l_setSpeedRelative(lua_State* L);
int l_getSpeed(lua_State* L);

int l_setDirection(lua_State* L);
int l_setDirectionRelative(lua_State* L);

int l_aimAtPoint(lua_State* L);
int l_getAimDirection(lua_State* L);

int l_linearInterpolate(lua_State* L);

int l_getDirection(lua_State* L);

int l_vanish(lua_State* L);
int l_kill(lua_State* L);

int l_isDead(lua_State* L);
int l_isDying(lua_State* L);

int l_resetTurns(lua_State* L);
int l_getTurn(lua_State* L);

int l_setLuaFunction(lua_State* L);

// Barrage functions

int l_getRank(lua_State* L);

int l_getTargetPosition(lua_State* L);

int l_launch(lua_State* L);
int l_launchAtTarget(lua_State* L);
int l_launchCircle(lua_State* L);

#endif /* BULLETLUA_H */