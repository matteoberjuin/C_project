#pragma once
#include "Vec2.h"
#include <string>
#include <QColor>

struct Particle
{

    std::string name;
    QColor color;
    Vec2 position;
    Vec2 predictedPosition;
    Vec2 velocity;

    bool mammamia;          // true si contact statique ce pas de temps
    Vec2 contactNormal;     // normale du dernier contact statique

    float mass;
    float radius;
};
