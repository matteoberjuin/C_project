#include <iostream>
#include "Context.h"
#include "Particle.h"

int main()
{
    Context ctx;

    // Particule 1
    Particle p1;
    p1.position = {0.f, 1.f};
    p1.predictedPosition = p1.position;
    p1.velocity = {0.f, 0.f};
    p1.mass = 1.f;
    p1.radius = 0.5f;

    // Particule 2
    Particle p2;
    p2.position = {0.8f, 1.f};   // chevauchement
    p2.predictedPosition = p2.position;
    p2.velocity = {0.f, 0.f};
    p2.mass = 1.f;
    p2.radius = 0.5f;

    ctx.addParticle(p1);
    ctx.addParticle(p2);

    const float dt = 0.016f; // ~60 FPS

    for (int i = 0; i < 100; ++i)
    {
        ctx.updatePhysicalSystem(dt);

        const auto& ps = ctx.particles();
        std::cout << "Frame " << i << "\n";
        std::cout << "p1: " << ps[0].position.x << ", " << ps[0].position.y << "\n";
        std::cout << "p2: " << ps[1].position.x << ", " << ps[1].position.y << "\n";
        std::cout << "------\n";
    }

    return 0;
}
