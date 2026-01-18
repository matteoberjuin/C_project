
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
    checkPoches();
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

    const float groundY = bottomY_;
    const float topY    = topY_;
    const float leftX   = leftX_;
    const float rightX  = rightX_;

    const Vec2 nBottom(0.f,  1.f);
    const Vec2 nTop   (0.f, -1.f);
    const Vec2 nLeft  (1.f,  0.f);
    const Vec2 nRight (-1.f, 0.f);

    for (int i = 0; i < (int)particles_.size(); ++i)
    {
        Particle& p = particles_[i];

        const float d = p.predictedPosition.y - p.radius; // bas de la sphère
        const float t = p.predictedPosition.y + p.radius; // haut
        const float l = p.predictedPosition.x - p.radius; // gauche
        const float r = p.predictedPosition.x + p.radius; // droite

        if (d < groundY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = nBottom;
            c.penetration = groundY - d;   // profondeur
            staticConstraints_.push_back(c);
        }

        if (t > topY)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = nTop;
            c.penetration = t - topY;
            staticConstraints_.push_back(c);
        }

        if (l < leftX)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = nLeft;
            c.penetration = leftX - l;
            staticConstraints_.push_back(c);
        }

        if (r > rightX)
        {
            StaticConstraint c;
            c.p = i;
            c.normal = nRight;
            c.penetration = r - rightX;
            staticConstraints_.push_back(c);
        }
    }
}

static void enforceStaticConstraint(const StaticConstraint& c, Particle& p)
{
    p.predictedPosition += c.normal * c.penetration;

    p.mammamia = true;
    p.contactNormal = c.normal;
}


void Context::addDynamicContactConstraints(float)
{
    dynamicConstraints_.clear();

    for (size_t i = 0; i < particles_.size(); ++i)
    {
        for (size_t j = i + 1; j < particles_.size(); ++j)
        {
            Particle& p1 = particles_[i];
            Particle& p2 = particles_[j];

            Vec2 d = p2.predictedPosition - p1.predictedPosition;
            float dist = d.length();
            float minDist = p1.radius + p2.radius;

            if (dist < minDist && dist > 1e-6f)
            {
                DynamicConstraint c;
                c.p1 = static_cast<int>(i);
                c.p2 = static_cast<int>(j);
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
    const float restitution = 0.9f;
    const float friction = 0.98f;

    for (Particle& p : particles_)
    {
        Vec2 oldVelocity = p.velocity;

        p.velocity = (p.predictedPosition - p.position) * (1.f / dt);


        if (p.mammamia)
        {
            float vn = oldVelocity.dot(p.contactNormal);


            if (vn < 0.f)
            {
                float vitesse_apres_impact = p.velocity.length();
                if (vitesse_apres_impact > 0.0001f)
                {
                    float vitesse_avant_impact = oldVelocity.length();

                    p.velocity = (p.velocity / vitesse_apres_impact) * (vitesse_avant_impact * restitution);
                }
            }
            p.mammamia = false;
        }
        p.velocity *= friction;
        p.position = p.predictedPosition;
    }
}

void Context::setWorldBounds(float leftX, float rightX, float bottomY, float topY)
{
    leftX_ = leftX;
    rightX_ = rightX;
    bottomY_ = bottomY;
    topY_ = topY;
}

void Context::checkPoches()
{
    for (int i = (int)particles_.size() - 1; i >= 0; --i)
    {
        for (const auto& poche : poches_)
        {
            Vec2 diff = particles_[i].position - poche.position;
            float distSq = diff.dot(diff);
            float seuil = poche.rayon;

            if (distSq < (seuil * seuil))
            {
                if (particles_[i].name == "boule blanche")
                {
                    blanche_en_attente_  = true;
                    particles_[i].position = Vec2(-1.0f, -1.0f);
                    particles_[i].velocity = Vec2(0.f, 0.f);
                    particles_[i].predictedPosition = Vec2(-1.0f, -1.0f);
                }
                else
                {
                    particles_.erase(particles_.begin() + i);
                    score++;
                }
            }
        }
    }
}


void Context::ajtPoche(Vec2 pos, float r)
{
    Poche p;
    p.position = pos;
    p.rayon = r;
    poches_.push_back(p);
}

void Context::appliquerImpulsion(int index, Vec2 impulsion) {
    if (index >= 0 && index < (int)particles_.size()) {
        particles_[index].velocity = impulsion;
    }
}

bool Context::est_ce_que_ca_bouge_encore() const {
    for (const auto& p : particles_) {
        if (p.velocity.length() > 0.01f) {
            return true;
        }
    }
    return false;
}


void Context::replacerBlanche(Vec2 pos) {
    for (auto& p : particles_) {
        if (p.name == "boule blanche") {
            p.position = pos;
            p.predictedPosition = pos;
            p.velocity = Vec2(0.f, 0.f);
            blanche_en_attente_ = false;
            break;
        }
    }
}
