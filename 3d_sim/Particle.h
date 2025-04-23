#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    float life; // Remaining lifetime of the particle

    Particle(glm::vec3 pos, glm::vec3 vel, float sz, float lifespan)
        : position(pos), velocity(vel), size(sz), life(lifespan) {}
};

#endif