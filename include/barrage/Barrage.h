#ifndef BARRAGE_H
#define BARRAGE_H

#include <stddef.h>
#include <barrage/Bullet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define QUEUE_SIZE  (1 << 8)
#define MAX_BULLETS (1 << 12)

// Since we don't really know where in our `bullets` array a bullet will be added and we can add a
// bullet at any time during the update loop, we won't know when that bullet will be updated for the
// first time. To fix this problem, we'll queue up bullets added in the middle of the update loop
// and add them all at the end.
struct BulletQueue
{
        size_t size;
        struct Bullet bullets[QUEUE_SIZE];
};

struct Barrage
{
        lua_State* L;

        size_t currentIndex;
        size_t processedCount;
        size_t activeCount;
        struct Bullet* firstAvailable;
        struct Bullet bullets[MAX_BULLETS];
        struct BulletQueue queue;

        float playerX, playerY;
        float rank;
};

// Pointers to the bullet/barrage that is currently being processed.
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
void addQueuedBullets(struct Barrage* barrage);

void setPlayerPosition(struct Barrage* barrage, float x, float y);
void tick(struct Barrage* barrage);

// Return the next active bullet in the barrage.
int nextAvailable(struct Barrage* barrage);
struct Bullet* yield(struct Barrage* barrage);

#endif /* BARRAGE_H */
