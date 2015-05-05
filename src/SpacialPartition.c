#include <barrage/SpacialPartition.h>

#include <stdlib.h>
#include <string.h>

struct SpacialPartition* sp_createSpacialPartition()
{
    struct SpacialPartition* sp = (struct SpacialPartition*)malloc(sizeof(struct SpacialPartition));
    memset(sp->bucketSize, 0, sizeof(sp->bucketSize));

    return sp;
}
