#include <barrage/SpacialPartition.h>

#include <stdlib.h>
#include <string.h>

#include <barrage/Rect.h>

struct SpacialPartition* br_createSpacialPartition()
{
    struct SpacialPartition* sp = (struct SpacialPartition*)malloc(sizeof(struct SpacialPartition));
    memset(sp->bucketSize, 0, sizeof(sp->bucketSize));

    return sp;
}

void br_addBullet(struct SpacialPartition* sp, struct Bullet* bullet)
{
    if (bl_isDying(bullet))
    {
        return;
    }

    int x = bullet->x / TILE_SIZE;
    int y = bullet->y / TILE_SIZE;

    if (x < 0 || x >= HORI_BUCKETS || y < 0 || y >= VERT_BUCKETS)
    {
        return;
    }

    if (sp->bucketSize[x][y] < BUCKET_LID)
    {
        // Don't fear the copy!
        sp->buckets[x][y][sp->bucketSize[x][y]] = *bullet;
        sp->bucketSize[x][y]++;
    }
}

void br_clear(struct SpacialPartition* sp)
{
    memset(sp->bucketSize, 0, sizeof(sp->bucketSize));
}

bool br_checkCollision(struct SpacialPartition* sp,
                       float playerX, float playerY, float playerWidth, float playerHeight)
{
    int x = playerX / TILE_SIZE;
    int y = playerY / TILE_SIZE;

    if (x < 0 || x >= HORI_BUCKETS || y < 0 || y >= VERT_BUCKETS)
    {
        return false;
    }

    struct Rect playerRect =
        {
            .x      = playerX - 2,
            .y      = playerY - 2,
            .width  = playerWidth * 0 + 4,
            .height = playerHeight * 0 + 4
        };

    for (size_t i = 0; i < sp->bucketSize[x][y]; i++)
    {
        const struct Bullet* b = &sp->buckets[x][y][i];

        // Default bullet collision box metrics.
        struct Rect bulletRect =
            {
                .x      = b->x - 2,
                .y      = b->y - 2,
                .width  = 4,
                .height = 4
            };

        if (rectOverlap(playerRect, bulletRect))
        {
            return true;
        }
    }

    return false;
}
