#ifndef SPACIALPARTITION_H
#define SPACIALPARTITION_H

#include <barrage/Bullet.h>
#include <barrage/Barrage.h>

#include <stdbool.h>

// How many pixels (squared) to govern per bucket.
#define TILE_SIZE    50

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
};

// Initialize a new spacial partition.
struct SpacialPartition* br_createSpacialPartition(struct SpacialPartition* sp);
void br_deleteSpacialPartition(struct SpacialPartition* sp, bool onHeap);

// Manage a bullet (for a frame).
void br_addBullet(struct SpacialPartition* sp, struct Bullet* bullet);

// Reset buckets. Expected to be called once per frame since bullets are so unpredictable.
void br_clear(struct SpacialPartition* sp);

// Returns true if the rect defined by the arguments intersects a bullet rect.
// playerX, playerY is defined to be the center of the player's hitbox.
bool br_checkCollision(struct SpacialPartition* sp,
                       float playerX, float playerY, float playerWidth, float playerHeight);

#endif /* SPACIALPARTITION_H */
