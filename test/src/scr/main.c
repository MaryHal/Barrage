#include <barrage/Barrage.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Give me a Barrage script!");
        return 1;
    }

    struct Barrage* barrage = br_createBarrageFromFile(argv[1], 320.0f, 120.0f);

    for (int i = 0; i < 200; ++i)
    {
        br_tick(barrage);

        while (br_hasNext(barrage))
        {
            br_yield(barrage);
        }
    }

    br_deleteBarrage(barrage);

    return 0;
}
