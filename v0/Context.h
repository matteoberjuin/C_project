#pragma once
#include "Vec2.h"
#include <vector>

class Context {
public:
    Context();
    void addParticle(const Particle& p) { particles.push_back(p); }
    const std::vector<Particle>& getParticles() const { return particles; }
public:
    void updatePhysicalSystem(float dt);

private:
    std::vector<Particle> particles;
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
