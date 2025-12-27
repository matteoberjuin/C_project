#pragma once
#include "Vec2.h"
#include <vector>

class Context {
public:
    Context();
    void addParticle(const Particle& p);
    const std::vector<Particle>& particles() const;
    void updatePhysicalSystem(float dt);

private:
    std::vector<Particle> particles_;
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
