#include "Droplet.h"
#include <cstdlib>
#include <iostream>

Droplet::Droplet(glm::vec3 pos, glm::vec3 vel, float sz)
    : position(pos), velocity(vel), size(sz) {}

void Droplet::update(float deltaTime, std::vector<Particle>& particles) {
    velocity.y -= 9.8f * deltaTime; // Gravity
    position += velocity * deltaTime;

    // Ground collision
    if (position.y - size < -2.0f) {
        position.y = -2.0f + size;

        // stop bouncing when energy is low
        if (std::abs(velocity.y) < 0.5f){
            velocity.y = 0.0f;

            // generate splash particles
            if (!hasCollided){
                hasCollided = true;
                for (int i = 0; i < 50; i ++) {
                    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265359f;
                    float speed = static_cast<float>(rand()) / RAND_MAX * 2.0f + 1.0f;
                    glm::vec3 velocity = glm::vec3(cos(angle) * speed, 1.0f, sin(angle) * speed);
                    particles.emplace_back(position, velocity, 0.1f, 2.0f); // position, velocity, size, lifespan
                }
            }
            
        }
        velocity = glm::vec3(0.0f);
    } else {
        hasCollided = false; // Reset collision state
    }
}