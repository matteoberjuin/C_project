#pragma once
#include <vector>
#include "Particle.h"
#include "Vec2.h"

struct DynamicConstraint {
    int p1;          // index particule 1
    int p2;          // index particule 2
    Vec2 normal;     // direction de séparation
    float penetration;
};

struct StaticConstraint {
    int p;
    Vec2 normal;
    float penetration;
};

class Context {
public:
    Context();
    void addParticle(const Particle& p);
    const std::vector<Particle>& particles() const;
    void updatePhysicalSystem(float dt);

private:
    std::vector<Particle> particles_;
    std::vector<DynamicConstraint> dynamicConstraints_;
    std::vector<StaticConstraint> staticConstraints_;
    Vec2 gravity;

private:
    void applyExternalForce(float dt);
    void dampVelocities(float dt);
    void updateExpectedPosition(float dt);
    void updateVelocityAndPosition(float dt);
    void addDynamicContactConstraints(float dt);
    void addStaticContactConstraints(float dt);
    void projectConstraints();
    void applyFriction(float dt);
    void deleteContactConstraints();
};
