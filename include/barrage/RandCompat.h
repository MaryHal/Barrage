#ifndef RANDCOMPAT_H
#define RANDCOMPAT_H

// Uses rand()...

unsigned int timeSeed();

unsigned int randIntRange(unsigned int a, unsigned int b);
float randFloatRange(float a, float b);

#endif /* RANDCOMPAT_H */
