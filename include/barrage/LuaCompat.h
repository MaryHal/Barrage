#ifndef LUACOMPAT_H
#define LUACOMPAT_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// < Lua5.2 compatibility
#if LUA_VERSION_NUM < 520
#define luaL_newlib(L,l) (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

#define luaL_newlibtable(L,l) (lua_createtable(L,0,sizeof(l)))
#define luaL_setfuncs luaX52_luaL_setfuncs

static inline void luaX52_luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup)
{
    luaL_checkstack(L, nup, "too many upvalues");

    /* fill the table with given functions */
    for (; l->name != NULL; l++)
    {
        /* copy upvalues to the top */
        for (int i = 0; i < nup; i++)
            lua_pushvalue(L, -nup);

        /* closure with those upvalues */
        lua_pushcclosure(L, l->func, nup);
        lua_setfield(L, -(nup + 2), l->name);
    }

    /* remove upvalues */
    lua_pop(L, nup);
}
#endif


#endif /* LUACOMPAT_H */
