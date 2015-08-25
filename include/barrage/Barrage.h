#ifndef BARRAGE_H
#define BARRAGE_H

#include <barrage/Bullet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* #include <stddef.h> */
#include <stdbool.h>

#define QUEUE_SIZE  (1 << 8)
#define MAX_BULLETS (1 << 12)

#define FPS 60
#define FRAME_TIME_MS (1.0f / FPS)

struct SpacialPartition;

struct Barrage
{
        lua_State* L;

        size_t index;
        size_t processedCount; // Internally keeps track of the number of
                               // bullets that still need to be processed
                               // (during tick or yield loop).
        size_t activeCount;    // Keeps track of the number of alive bullets.
        size_t killCount;      // Number of bullets to kill on next update.

        struct Bullet* firstAvailable; // Implicit linked list of free bullets.

        // TODO: Integrate a dynamic, resizable array instead of a static array.
        // This will probably involve restructuring our free bullet list since
        // dynamic arrays in C usually have unstable object addresses.
        struct Bullet bullets[MAX_BULLETS];

        // Since we don't really know where in our `bullets` array a bullet will
        // be added and we can add a bullet at any time during the update loop,
        // we won't know when that bullet will be updated for the first time. To
        // fix this problem, we'll queue up bullets added in the middle of the
        // update loop and add them all at the end.
        struct BulletQueue
        {
                size_t size;
                struct Bullet bullets[QUEUE_SIZE];
        } queue;

        float playerX, playerY;
        float rank;             // Requested difficulty of this barrage (0.0, 1.0]

        float timeAccumulator;
};

// Pointers to the bullet/barrage that is currently being processed.
extern struct Bullet*  g_bullet;
extern struct Barrage* g_barrage;

// One state. Just one.
extern lua_State* g_L;

lua_State* br_initGlobalLuaState_();

struct Barrage* br_createBarrage(struct Barrage* barrage);
void br_deleteBarrage(struct Barrage* barrage, bool onHeap);

// Setup our barrage. Runs onLoad and attach the `main` function to a new
// bullet. This function expects a table with an `main` function (and an option
// `onLoad` function) on top of the global lua stack.
struct Bullet* br_pushBarrageFunctions_(struct Barrage* barrage);

void br_createBulletFromFile(struct Barrage* barrage,
                             const char* filename,
                             float originX, float originY, int type);
void br_createBulletFromScript(struct Barrage* barrage,
                               const char* script,
                               float originX, float originY, int type);

void br_createBullet(struct Barrage* barrage,
                     float x, float y, float vx, float vy,
                     int luaFuncRef, int type);

struct Bullet* br_getFreeBullet_(struct Barrage* barrage);
void br_addQueuedBullets_(struct Barrage* barrage);

void br_setRank(struct Barrage* barrage, float rank);
float br_getRank(struct Barrage* barrage);

void br_storeFloat(struct Barrage* barrage, const char* key, float value);
float br_getFloat(struct Barrage* barrage, const char* key);

void br_setPlayerPosition(struct Barrage* barrage, float x, float y);
bool br_tick(struct Barrage* barrage, struct SpacialPartition* sp, float dt);

void br_vanishAll(struct Barrage* barrage);

// Return the next active bullet in the barrage.
int br_hasNext(struct Barrage* barrage);
struct Bullet* br_yield(struct Barrage* barrage);

void br_resetHasNext(struct Barrage* barrage);

size_t br_countAlive(struct Barrage* barrage);

// "Meta"-functions used by the Lua-interface.
void br_aimAtTarget(struct Barrage* barrage, struct Bullet* current);

void br_launch(struct Barrage* barrage, struct Bullet* current,
               float dir, float speed, int luaFuncRef, int type);
void br_launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                       float speed, int luaFuncRef, int type);
void br_launchCircle(struct Barrage* barrage, struct Bullet* current,
                     int segments, float speed, int luaFuncRef, int type);

#endif /* BARRAGE_H */
