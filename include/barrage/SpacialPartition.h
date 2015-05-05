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

struct SpacialPartition
{
        const struct Bullet bullets[HORI_BUCKETS][VERT_BUCKETS][BUCKET_LID];
        size_t bucketSize[HORI_BUCKETS][VERT_BUCKETS];
};

// Initialize a new spacial partition.
struct SpacialPartition* sp_createSpacialPartition();

// Manage a bullet (for a frame).
void sp_addBullet(struct SpacialPartition* sp, const struct Bullet* bullet);

// Reset buckets. Expected to be called once per frame since bullets are so unpredictable.
void sp_clear(struct SpacialPartition* sp);

// Returns true if the rect defined by the arguments intersects a bullet rect.
bool sp_checkCollision(float playerX, float playerY, float playerWidth, float playerHeight);

#endif /* SPACIALPARTITION_H */
