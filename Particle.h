#pragma once
#include "Vec2.h"


struct Particle
{
    Vec2 position;
    Vec2 predictedPosition;
    Vec2 velocity;

    float mass;
    float radius;
};


