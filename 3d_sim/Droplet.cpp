#include "Droplet.h"
#include <cstdlib>
#include <iostream>
#include <random>

Droplet::Droplet(glm::vec3 pos, glm::vec3 vel, float sz)
    : position(pos), velocity(vel), size(sz) {}

void Droplet::update(float deltaTime, std::vector<Particle>& particles) {
    velocity.y -= 9.8f * deltaTime; // Gravity
    position += velocity * deltaTime;

    // Ground collision
    if (position.y - size < -2.0f) {
        position.y = -2.0f + size;

        // generate splash particles
        if (!hasCollided){
            hasCollided = true;

            std::default_random_engine generator;
            std::normal_distribution<float> angleDistribution(0.0f, 1.0f); // Mean 0, StdDev 1
            std::lognormal_distribution<float> speedDistribution(0.5f, 0.25f); // Mean 0.5, StdDev 0.25

            for (int i = 0; i < 50; i ++) {

                float angle = angleDistribution(generator) * 3.14159265359f; // scale
                float speed = speedDistribution(generator); // log-normal speed

                glm::vec3 velocity = glm::vec3(cos(angle) * speed, 1.0f, sin(angle) * speed);

                particles.emplace_back(position, velocity, 0.05f, 2.0f); // position, velocity, size, lifespan
            }
        }
        
        
        velocity = glm::vec3(0.0f); // set velocity to zero after collision
    } else {
        hasCollided = false; // Reset collision state
    }
}