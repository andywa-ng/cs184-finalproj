#ifndef DROPLET_H
#define DROPLET_H

#include <glm/glm.hpp>
#include "Particle.h"
#include <vector>

class Droplet {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    bool hasCollided; // if the droplet has collided with an object or the ground

    Droplet(glm::vec3 pos, glm::vec3 vel, float sz);
    void update(float deltaTime, std::vector<Particle>& particles);
};

#endif