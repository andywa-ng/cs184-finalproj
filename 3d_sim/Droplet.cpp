#include "Droplet.h"

Droplet::Droplet(glm::vec3 pos, glm::vec3 vel, float sz)
    : position(pos), velocity(vel), size(sz) {}

void Droplet::update(float deltaTime) {
    velocity.y -= 9.8f * deltaTime; // Gravity
    position += velocity * deltaTime;

    // Ground collision
    if (position.y - size < -2.0f) {
        position.y = -2.0f + size;
        velocity.y = -velocity.y * 0.3f;
        if (std::abs(velocity.y) < 0.5f) velocity.y = 0.0f;
    }
}