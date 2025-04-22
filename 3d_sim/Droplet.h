#ifndef DROPLET_H
#define DROPLET_H

#include <glm/glm.hpp>

class Droplet {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float size;

    Droplet(glm::vec3 pos, glm::vec3 vel, float sz);
    void update(float deltaTime);
};

#endif