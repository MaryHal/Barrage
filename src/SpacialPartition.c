#include <barrage/SpacialPartition.h>

#include <stdlib.h>
#include <string.h>

#include <barrage/Rect.h>

#include <assert.h>

struct SpacialPartition* br_createSpacialPartition(struct SpacialPartition* sp)
{
    if (sp == NULL)
    {
        sp = (struct SpacialPartition*)malloc(sizeof(struct SpacialPartition));
    }

    memset(sp->bucketSize, 0, sizeof(sp->bucketSize));

    // Since we delete lazily (reset the size of each bucket), we actually don't
    // need to reset the data inside the buckets.
    /* memset(sp->buckets, 0, sizeof(sp->buckets)); */

    memset(sp->models, 0, sizeof(sp->models));
    sp->modelCount = 0;

    // Add default collision model.
    struct Rect defaultCollsionRect = {
        .x      = 0,
        .y      = 0,
        .width  = 4,
        .height = 4
    };
    br_addModel(sp, defaultCollsionRect);

    return sp;
}

void br_deleteSpacialPartition(struct SpacialPartition* sp, bool doFree)
{
    if (doFree)
    {
        free(sp);
    }
}

size_t br_addModel(struct SpacialPartition* sp, struct Rect r)
{
    sp->models[sp->modelCount++] = r;
    return sp->modelCount;
}

struct Rect br_getModel(struct SpacialPartition* sp, size_t modelIndex)
{
    assert(modelIndex < sp->modelCount);

    return sp->models[modelIndex];
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

    assert(sp->bucketSize[x][y] < BUCKET_LID);

    if (sp->bucketSize[x][y] < BUCKET_LID)
    {
        // Don't fear the copy!
        sp->buckets[x][y][sp->bucketSize[x][y]] = *bullet;
        sp->bucketSize[x][y]++;
    }
}

void br_clear(struct SpacialPartition* sp)
{
    // Lazy deletion! Just reset the size count of each bucket.
    memset(sp->bucketSize, 0, sizeof(sp->bucketSize));
}

bool br_checkCollision_(struct SpacialPartition* sp, struct Rect playerRect)
{
    int x = playerRect.x / TILE_SIZE;
    int y = playerRect.y / TILE_SIZE;

    if (x < 0 || x >= HORI_BUCKETS || y < 0 || y >= VERT_BUCKETS)
    {
        return false;
    }

    for (size_t i = 0; i < sp->bucketSize[x][y]; i++)
    {
        const struct Bullet* b = &sp->buckets[x][y][i];

        struct Rect bulletRect = sp->models[b->model];
        bulletRect.x = b->x - bulletRect.width  / 2;
        bulletRect.y = b->y - bulletRect.height / 2;

        if (rectOverlap(playerRect, bulletRect))
        {
            return true;
        }
    }

    return false;
}

bool br_checkCollision(struct SpacialPartition* sp,
                        float playerX, float playerY, float playerWidth, float playerHeight)
{
    struct Rect player = {
        .x      = playerX - playerWidth / 2,
        .y      = playerY - playerHeight / 2,
        .width  = playerWidth,
        .height = playerHeight
    };

    return br_checkCollision_(sp, player);
}
