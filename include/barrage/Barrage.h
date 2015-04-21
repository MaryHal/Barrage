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
        size_t processedCount; // Internally keeps track of the number of bullets that still need to
                               // be processed (updated or yielded).
        size_t activeCount;    // Keeps track of the number of alive bullets.

        struct Bullet* firstAvailable; // Implicit linked list of free bullets.
        struct Bullet bullets[MAX_BULLETS];
        struct BulletQueue queue; // Queue of new bullets to add at the end up update phase.

        float playerX, playerY;
        float rank;             // Requested difficulty of this barrage (0.0, 1.0]
};

// Pointers to the bullet/barrage that is currently being processed.
extern struct Bullet*  g_bullet;
extern struct Barrage* g_barrage;

// TODO: Prefix barrage function names.

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


// "Meta"-functions used by the Lua-interface.
void aimAtTarget(struct Barrage* barrage, struct Bullet* current);

void launch(struct Barrage* barrage, struct Bullet* current,
            float dir, float speed, int luaFuncRef);
void launchAtTarget(struct Barrage* barrage, struct Bullet* current,
                    float speed, int luaFuncRef);
void launchCircle(struct Barrage* barrage, struct Bullet* current,
                  int segments, float speed, int luaFuncRef);

#endif /* BARRAGE_H */
