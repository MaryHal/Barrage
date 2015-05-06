#ifndef BARRAGE_H
#define BARRAGE_H

#include <stddef.h>
#include <barrage/Bullet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define QUEUE_SIZE  (1 << 8)
#define MAX_BULLETS (1 << 12)

// Forward declaration
struct SpacialPartition;

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
        size_t processedCount; // Internally keeps track of the number of bullets that still need to
                               // be processed (updated or yielded).
        size_t activeCount;    // Keeps track of the number of alive bullets.
        size_t killCount;      // Number of bullets to kill on next update.

        struct Bullet* firstAvailable; // Implicit linked list of free bullets.
        struct Bullet bullets[MAX_BULLETS];
        struct BulletQueue queue; // Queue of new bullets to add at the end up update phase.

        float playerX, playerY;
        float rank;             // Requested difficulty of this barrage (0.0, 1.0]
};

// Pointers to the bullet/barrage that is currently being processed.
extern struct Bullet*  g_bullet;
extern struct Barrage* g_barrage;

struct Barrage* br_createBarrage_();
void br_deleteBarrage(struct Barrage* barrage);

void br_runOnLoadFunc_(struct Barrage* barrage);
struct Barrage* br_createBarrageFromFile(const char* filename,
                                         float originX, float originY);
struct Barrage* br_createBarrageFromScript(const char* script,
                                           float originX, float originY);

void br_createBullet(struct Barrage* barrage,
                     float x, float y, float vx, float vy,
                     int luaFuncRef);

struct Bullet* br_getFreeBullet(struct Barrage* barrage);
void br_addQueuedBullets(struct Barrage* barrage);

void br_setRank(struct Barrage* barrage, float rank);
float br_getRank(struct Barrage* barrage);

void br_storeFloat(struct Barrage* barrage, const char* key, float value);
float br_getFloat(struct Barrage* barrage, const char* key);

void br_setPlayerPosition(struct Barrage* barrage, float x, float y);
void br_tick(struct Barrage* barrage, struct SpacialPartition* sp);

// Return the next active bullet in the barrage.
int br_hasNext(struct Barrage* barrage);
struct Bullet* br_yield(struct Barrage* barrage);

void br_resetHasNext(struct Barrage* barrage);

// "Meta"-functions used by the Lua-interface.
void br_aimAtTarget(struct Barrage* barrage, struct Bullet* current);

void br_launch(struct Barrage* barrage, struct Bullet* current,
               float dir, float speed, int luaFuncRef);
void br_launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                       float speed, int luaFuncRef);
void br_launchCircle(struct Barrage* barrage, struct Bullet* current,
                     int segments, float speed, int luaFuncRef);

#endif /* BARRAGE_H */
