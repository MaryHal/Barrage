#ifndef SPACIALPARTITION_H
#define SPACIALPARTITION_H

#include <barrage/Bullet.h>
#include <barrage/Barrage.h>

#include <barrage/Rect.h>

#include <stdbool.h>

#define MAX_MODELS   16

// How many pixels (squared) to govern per bucket.
#define TILE_SIZE    80

// How many buckets do we need?
#define HORI_BUCKETS (640 / TILE_SIZE)
#define VERT_BUCKETS (480 / TILE_SIZE)

// Maximum bullets per bucket.
#define BUCKET_LID   100

// Cut an area into a grid and organize all bullets into each grid cell so during the collision
// detection step we only need to compare bullets inside a single cell.
struct SpacialPartition
{
        struct Bullet buckets[HORI_BUCKETS][VERT_BUCKETS][BUCKET_LID];
        size_t bucketSize[HORI_BUCKETS][VERT_BUCKETS];

        struct Rect models[MAX_MODELS];
        size_t modelCount;
};

// Initialize a new spacial partition.
struct SpacialPartition* br_createSpacialPartition(struct SpacialPartition* sp);
void br_deleteSpacialPartition(struct SpacialPartition* sp, bool onHeap);

// Add collision models
void br_addModel(struct SpacialPartition* sp, struct Rect r);
struct Rect br_getModel(struct SpacialPartition* sp, int modelIndex);

// Manage a bullet (for a frame).
void br_addBullet(struct SpacialPartition* sp, struct Bullet* bullet);

// Reset buckets. Expected to be called once per frame since bullets are so unpredictable.
void br_clear(struct SpacialPartition* sp);

// Returns true if playerRect overlaps a bullet's rect. (playerRect.x, playerRect.y) is defined to
// be the center of the player's hitbox.
bool br_checkCollision(struct SpacialPartition* sp, struct Rect playerRect);
bool br_checkCollision2(struct SpacialPartition* sp,
                        float playerX, float playerY, float playerWidth, float playerHeight);

#endif /* SPACIALPARTITION_H */
