#ifndef BULLET_H
#define BULLET_H

struct Bullet
{
        union
        {
                struct
                {
                        float x, y;
                        float vx, vy;
                };

                struct Bullet* next;
        };

        /* int life; */
        int turn;

        union
        {
                int luaFuncRef;
                int (*CFuncName)();
        };
};

/* struct BulletAttribute */
/* { */
/*         float r, g, b; */
/* }; */

// TODO: Reconsider implication of Dying vs Dead
enum BulletState
{
    DYING = -31,
    JUST_ABOUT_DEAD = -2,
    DEAD  = -1
};

void bl_resetBullet(struct Bullet* b);
void bl_setBullet(struct Bullet* b);
void bl_setBulletData(struct Bullet* b, float x, float y, float vx, float vy);
void bl_copyBullet(struct Bullet* b1, struct Bullet* b2);

void bl_setNext(struct Bullet* b, struct Bullet* next);
struct Bullet* bl_getNext(struct Bullet* b);

void bl_setPosition(struct Bullet* b, float x, float y);
void bl_setVelocity(struct Bullet* b, float vx, float vy);

void bl_setSpeedAndDirection(struct Bullet* b, float speed, float dir);
void bl_setSpeed(struct Bullet* b, float speed);
void bl_setSpeedRelative(struct Bullet* b, float speed);
float bl_getSpeed(struct Bullet* b);

void bl_setDirection(struct Bullet* b, float dir);
void bl_setDirectionRelative(struct Bullet* b, float dir);

void bl_aimAtPoint(struct Bullet* b, float tx, float ty);
float bl_getAimDirection(struct Bullet* b, float tx, float ty);
void bl_linearInterpolate(struct Bullet* b, float tx, float ty, int steps);

float bl_getDirection(struct Bullet* b);

void bl_vanish(struct Bullet* b);
void bl_kill(struct Bullet* b);

int bl_isDead(struct Bullet* b);
int bl_isDying(struct Bullet* b);

void bl_resetTurns(struct Bullet* b);
int bl_getTurn(struct Bullet* b);
// void setColor(unsigned char newR, unsigned char newG, unsigned char newB);

// Set function for bullet to run.
void bl_setLuaFunction(struct Bullet* b, int luaFuncRef);
void bl_setCFunction(struct Bullet* b, int (*function)());

void bl_update(struct Bullet* b);

// Adjust speed if near zero as setDirection depends on at least one component
// of our velocity vector is non-zero.
void bl_fixSpeed(struct Bullet* b);

#endif /* BULLET_H */
