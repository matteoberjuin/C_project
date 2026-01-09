#include "Particle.h"
#include "Vec2.h"
#include "Context.h"

Context::Context() : gravity(0.f,0.f){}


void Context::updatePhysicalSystem(float dt)
{
    applyExternalForce(dt);
    dampVelocities(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints(dt);
    addDynamicContactConstraints(dt);
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
    const float topY = 2.5f;
    const float leftY = 0.f;
    const float rightY = 4.f;
    const Vec2 normalBot(0.f, 1.f);
    const Vec2 normalLeft(1.f,0.f);
    const Vec2 normalTop(0.f,-1.f);
    const Vec2 normalRight(-1.f,0.f);

    for (int i = 0; i < particles_.size(); ++i)
    {
        Particle& p = particles_[i];

        float d = p.predictedPosition.y - p.radius;
        float l = p.predictedPosition.x - p.radius;
        float t = p.predictedPosition.y + p.radius;
        float r = p.predictedPosition.x + p.radius;

        if (d < groundY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = normalBot;
            c.penetration = groundY;
            staticConstraints_.push_back(c);
        }
        else if (l < leftY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = normalLeft;
            c.penetration = leftY;
            staticConstraints_.push_back(c);

        }
        else if (r > rightY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = normalRight;
            c.penetration = rightY;
            staticConstraints_.push_back(c);

        }
        else if (t> topY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = normalTop;
            c.penetration = topY;
            staticConstraints_.push_back(c);
        }

    }
}

static void enforceStaticConstraint(const StaticConstraint& c,
                                    Particle& p)
{
    p.predictedPosition.y = p.radius + c.penetration;
    p.mammamia=1;
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

void Context::updateVelocityAndPosition(float dt)
{
    for (Particle& p : particles_)
    {
        Vec2 oldPosition = p.position;
        if (p.mammamia==0) {
        p.position = p.predictedPosition;
        p.velocity = (p.position - oldPosition) * (1.f / dt);
        } else {
            p.position = p.predictedPosition;
            p.velocity.y = (oldPosition.y - p.position.y) * (1.f / dt);
            p.mammamia = 0;
        }
    }
}

