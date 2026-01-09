#pragma once
#include "Vec2.h"
#include <string>

struct Particle
{   std::string name;
    Vec2 position;
    Vec2 predictedPosition;
    Vec2 velocity;
    bool mammamia;

    float mass;
    float radius;
};
