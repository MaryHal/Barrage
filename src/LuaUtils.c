#include <barrage/LuaUtils.h>
#include <stdio.h>

void stackDump (lua_State *L)
{
    int i;
    int top = lua_gettop(L);/* depth of the stack */

    /* repeat for each level */
    for (i = 1; i <= top; i++)
    {
        int t = lua_type(L, i);
        switch (t)
        {
        case LUA_TSTRING: {/* strings */
            printf("%d '%s'", i, lua_tostring(L, i));
            break;
        }
        case LUA_TBOOLEAN: {/* booleans */
            printf("%d %s", i, lua_toboolean(L, i) ? "true" : "false");
            break;
        }
        case LUA_TNUMBER: {/* numbers */
            printf("%d %g", i, lua_tonumber(L, i));
            break;
        }
        default: {/* other values */
            printf("%d %s", i, lua_typename(L, t));
            break;
        }
        }
        printf("\n");/* put a separator */
    }
    printf("---\n");/* end the listing */
}
