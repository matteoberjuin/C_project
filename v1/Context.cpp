#include <iostream>
#include "Particle.h"
#include "Vec2.h"
#include "Context.h"

Context::Context() : gravity(0.f, -9.81f){}


void Context::updatePhysicalSystem(float dt)
{
    applyExternalForce(dt);
    dampVelocities(dt);
    updateExpectedPosition(dt);
    updateVelocityAndPosition(dt);
}



void Context::applyExternalForce(float dt) {
    for (Particle& p  : particles_){
    p.velocity = p.velocity + dt*gravity;}
}

void Context::dampVelocities(float dt) {}



void Context::updateExpectedPosition(float dt) {
    for (Particle& p : particles_)
    {
        p.predictedPosition = p.position + dt * p.velocity;
    }

}




void Context::updateVelocityAndPosition(float dt)
{
    for (Particle& p : particles_)
    {
        Vec2 oldPosition = p.position;

        p.position = p.predictedPosition;
        p.velocity = (p.position - oldPosition) * (1.f / dt);
    }
}



void Context::addParticle(const Particle& p){
    particles_.push_back(p);
}

const std::vector<Particle>& Context::particles() const
{
    return particles_;
}