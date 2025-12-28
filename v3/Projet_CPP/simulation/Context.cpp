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
    addStaticContactConstraints(dt);
    addDynamicContactConstraints(dt);

    // itérations PBD
    for (int i = 0; i < 5; ++i)
        projectConstraints();

    updateVelocityAndPosition(dt);
    deleteContactConstraints();
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

void Context::addStaticContactConstraints(float)
{
    staticConstraints_.clear();

    const float groundY = 0.f;
    const Vec2 normal(0.f, 1.f);

    for (int i = 0; i < particles_.size(); ++i)
    {
        Particle& p = particles_[i];

        float d = p.predictedPosition.y - p.radius;

        if (d < groundY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = normal;
            c.penetration = groundY - d;
            staticConstraints_.push_back(c);
        }
    }
}

static void enforceStaticConstraint(const StaticConstraint& c,
                                    Particle& p)
{
    p.predictedPosition += c.normal * c.penetration;
}

void Context::addDynamicContactConstraints(float)
{
    dynamicConstraints_.clear();

    for (int i = 0; i < particles_.size(); ++i)
    {
        for (int j = i + 1; j < particles_.size(); ++j)
        {
            Particle& p1 = particles_[i];
            Particle& p2 = particles_[j];

            Vec2 d = p2.predictedPosition - p1.predictedPosition;
            float dist = d.length();
            float minDist = p1.radius + p2.radius;

            if (dist < minDist && dist > 1e-6f)
            {
                DynamicConstraint c;
                c.p1 = i;
                c.p2 = j;
                c.normal = d / dist;
                c.penetration = minDist - dist;
                dynamicConstraints_.push_back(c);
            }
        }
    }
}

static void enforceDynamicConstraint(const DynamicConstraint& c,
                                     Particle& p1,
                                     Particle& p2)
{
    float w1 = 1.f / p1.mass;
    float w2 = 1.f / p2.mass;
    float wSum = w1 + w2;

    Vec2 correction = c.normal * c.penetration;

    p1.predictedPosition -= correction * (w1 / wSum);
    p2.predictedPosition += correction * (w2 / wSum);
}

void Context::projectConstraints()
{
    // Contraintes statiques
    for (const auto& c : staticConstraints_)
    {
        enforceStaticConstraint(c, particles_[c.p]);
    }

    // Contraintes dynamiques
    for (const auto& c : dynamicConstraints_)
    {
        enforceDynamicConstraint(c,
                                 particles_[c.p1],
                                 particles_[c.p2]);
    }
}

void Context::deleteContactConstraints()
{
    staticConstraints_.clear();
    dynamicConstraints_.clear();
}

