#ifndef BARRAGE_H
#define BARRAGE_H

#include <stddef.h>
#include <barrage/Bullet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* #define MAX_BULLETS (1 << 12) */
#define MAX_BULLETS (1 << 4)

struct Barrage
{
        lua_State* L;

        size_t currentIndex;
        size_t activeCount;
        struct Bullet* firstAvailable;
        struct Bullet bullets[MAX_BULLETS];

        float playerX, playerY;
        float rank;
};

extern struct Bullet*  g_bullet;
extern struct Barrage* g_barrage;

struct Barrage* createBarrage();
void deleteBarrage(struct Barrage* barrage);

struct Barrage* createBarrageFromFile(const char* filename,
                                      float originX, float originY);
struct Barrage* createBarrageFromScript(const char* script,
                                        float originX, float originY);

void createBullet(struct Barrage* barrage,
                  float x, float y, float vx, float vy,
                  int luaFuncRef);

struct Bullet* getFreeBullet(struct Barrage* barrage);

void setPlayerPosition(struct Barrage* barrage, float x, float y);
void tick(struct Barrage* barrage);

// Return the next active bullet in the barrage.
int nextAvailable(struct Barrage* barrage);
struct Bullet* yield(struct Barrage* barrage);

#endif /* BARRAGE_H */
