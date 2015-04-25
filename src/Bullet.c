#include <barrage/Bullet.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <barrage/MathUtils.h>

#include <lauxlib.h>

const int DEFAULT_FRAMES_UNTIL_DEATH = 30;

void bl_resetBullet(struct Bullet* b)
{
    bl_setBulletData(b, 0.0f, 0.0f, 0.0f, 0.0f);

    /* b->life = 0; */
    b->turn = 0;

    b->luaFuncRef = LUA_NOREF;
}

void bl_setBullet(struct Bullet* b)
{
    b->next = NULL;
    bl_resetBullet(b);
}

void bl_setBulletData(struct Bullet* b, float x, float y, float vx, float vy)
{
    b->x  = x;
    b->y  = y;
    b->vx = vx;
    b->vy = vy;

    bl_fixSpeed(b);
}

void bl_copyBullet(struct Bullet* to, struct Bullet* from)
{
    to->x  = from->x;
    to->y  = from->y;
    to->vx = from->vx;
    to->vy = from->vy;

    /* to->life = from->life; */
    to->turn = from->turn;

    to->luaFuncRef = from->luaFuncRef;
}

void bl_setNext(struct Bullet* b, struct Bullet* next)
{
    b->next = next;
}

struct Bullet* bl_getNext(struct Bullet* b)
{
    return b->next;
}

void bl_setPosition(struct Bullet* b, float x, float y)
{
    b->x = x;
    b->y = y;
}

void bl_setVelocity(struct Bullet* b, float vx, float vy)
{
    b->vx = vx;
    b->vy = vy;

    bl_fixSpeed(b);
}

void bl_setSpeedAndDirection(struct Bullet* b, float speed, float dir)
{
    b->vx =  speed * sin(dir);
    b->vy = -speed * cos(dir);

    bl_fixSpeed(b);
}

void bl_setSpeed(struct Bullet* b, float speed)
{
    float mag = bl_getSpeed(b);

    b->vx = (b->vx * speed) / mag;
    b->vy = (b->vy * speed) / mag;

    bl_fixSpeed(b);
}

void bl_setSpeedRelative(struct Bullet* b, float speed)
{
    float mag = bl_getSpeed(b);

    b->vx = (b->vx * (speed + mag)) / mag;
    b->vy = (b->vy * (speed + mag)) / mag;

    bl_fixSpeed(b);
}

float bl_getSpeed(struct Bullet* b)
{
    return sqrt(b->vx * b->vx +
                b->vy * b->vy);
}

void bl_setDirection(struct Bullet* b, float dir)
{
    float speed = bl_getSpeed(b);
    b->vx =  speed * sin(dir);
    b->vy = -speed * cos(dir);
}

void bl_setDirectionRelative(struct Bullet* b, float dir)
{
    bl_setDirection(b, dir + bl_getDirection(b));
}

void bl_aimAtPoint(struct Bullet* b, float tx, float ty)
{
    // TODO: use getDirectionAim
    bl_setDirection(b, bl_PI - atan2(tx - b->x,
                                     ty - b->y));
}

float bl_getAimDirection(struct Bullet* b, float tx, float ty)
{
    return bl_PI - atan2(tx - b->x, ty - b->y);
}

void bl_linearInterpolate(struct Bullet* b, float tx, float ty, int steps)
{
    bl_setVelocity(b,
                   (tx - b->x) / steps,
                   (ty - b->y) / steps);
}

float bl_getDirection(struct Bullet* b)
{
    return bl_PI - atan2(b->vx, b->vy);
}

void bl_vanish(struct Bullet* b, int framesTilDeath)
{
    if (!bl_isDying(b))
        b->turn = -framesTilDeath - 1 - 1;
}

void bl_kill(struct Bullet* b)
{
    // turn is updated _after_ running luaFuncRef, so we need to subtract one from the dead
    // flag so it becomes dead during the update loop.
    b->turn = DEAD - 1;
}

int bl_isDead(struct Bullet* b)
{
    return b->turn == DEAD;
}

int bl_isDying(struct Bullet* b)
{
    return b->turn < 0;
}

void bl_resetTurns(struct Bullet* b)
{
    b->turn = 0;
}

int bl_getTurn(struct Bullet* b)
{
    return b->turn;
}

// void setColor(unsigned char newR, unsigned char newG, unsigned char newB);

void bl_setLuaFunction(struct Bullet* b, int luaFuncRef)
{
    b->luaFuncRef = luaFuncRef;
}

void bl_setCFunction(struct Bullet* b, int (*function)())
{
    b->CFuncName = function;
}

void bl_update(struct Bullet* b)
{
    b->x += b->vx;
    b->y += b->vy;

    b->turn++;
}

// Adjust speed if near zero as setDirection depends on at least one component
// of our velocity vector is non-zero.
void bl_fixSpeed(struct Bullet* b)
{
    // See https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    // Direction (alone) is dependent on components, so if speed is set to 0.0f,
    // setting a direction will not do anything.
    if (fabs(b->vy) < FLT_EPSILON)
    {
        b->vy = FLT_EPSILON;
    }
}
