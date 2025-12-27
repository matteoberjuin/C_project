#include <iostream>

#include "Particle.h"

#include <iostream>
#include "Context.h"

int main()
{
    Context ctx;

    Particle p;
    p.position = {0.f, 0.f};
    p.predictedPosition = {0.f, 0.f};
    p.velocity = {0.f, 0.f};
    p.mass = 1.f;
    p.radius = 0.1f;

    ctx.addParticle(p);

    const float dt = 0.1f;

    for (int i = 0; i < 5; ++i)
    {
        ctx.updatePhysicalSystem(dt);

        const Particle& p0 = ctx.particles()[0];
        std::cout << "pos " << p0.position.x << " " << p0.position.y << "\n";
        std::cout << "vel " << p0.velocity.x << " " << p0.velocity.y << "\n";
    }

    return 0;
}