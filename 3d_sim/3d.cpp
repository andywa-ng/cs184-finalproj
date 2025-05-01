#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <OpenGL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Droplet.h"
#include "ShaderUtils.h"
#include "Particle.h"
#include <vector>
#include <iostream>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Global variables for camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f, zoom = 45.0f;
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouse = true;
std::vector<Particle> particles;

// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.1f;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Reversed since y-coordinates go bottom to top
    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

// Create a sphere mesh
void createDroplet(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int sectors, int stacks) {
    float PI = 3.14159265359f;
    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;
    float sectorAngle, stackAngle;

    std::vector<float> Cn = {0.0f, -0.1f, 0.02f, -0.005f, 0.001f}; // Cn coefficients

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = PI / 2 - i * stackStep; // polar angle thata
       
        float r = radius;
        for (size_t n = 0; n < Cn.size(); ++n) {
            r += Cn[n] * cosf(n * stackAngle);
        }

        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        // Apply scaling and tapering for teardrop shape
        float scale = 1.0f + (z / radius); // Stretch the bottom and taper the top
        z *= 0.3f; // Elongate the droplet vertically

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            // Vertex position
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal vector
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

// load shader source code
std::string vertexCode = loadShaderSource("vertex_shader.glsl");
std::string fragmentCode = loadShaderSource("fragment_shader.glsl");

const char* vertexShaderSource = vertexCode.c_str();
const char* fragmentShaderSource = fragmentCode.c_str();

int main() {
    bool isPaused = false;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Water Droplet Simulation", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // Enable depth testing and alpha blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create and compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return -1;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Create sphere mesh for water droplet
    std::vector<GLfloat> sphereVertices;
    std::vector<GLuint> sphereIndices;
    createDroplet(sphereVertices, sphereIndices, 0.1f, 32, 16);
    
    // Create VAO, VBO, EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), sphereVertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    // Create a ground plane VAO
    GLfloat groundVertices[] = {
        // Positions          // Normals
        -5.0f, -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
         5.0f, -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
         5.0f, -2.0f,  5.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, -2.0f,  5.0f,  0.0f, 1.0f, 0.0f
    };
    
    GLuint groundIndices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    GLuint groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);
    
    glBindVertexArray(groundVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    std::vector<Droplet> droplets;
    std::vector<Puddle> puddles;
    float spawnTimer = 0.0f;

    // Store initial state of droplet
    // glm::vec3 initialPosition = glm::vec3(0.0f, 2.0f, 0.0f);
    // glm::vec3 initialVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    // float initialSize = 0.25f; // change for visibility

    // Create a droplet
    // Droplet droplet(initialPosition, initialVelocity, initialSize);
    
    // Light position
    glm::vec3 lightPos = glm::vec3(2.0f, 3.0f, 2.0f);
    
    // Time tracking for animation
    float lastFrame = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        float cameraSpeed = 2.5f * 0.016f; // Adjust speed based on delta time
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        
        // Pause functionality
        static bool pKeyPressed = false;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (!pKeyPressed) {
                isPaused = !isPaused; // Toggle pause state
                pKeyPressed = true;

                if (isPaused) {
                    spawnTimer = 0.0f; // Reset spawn timer
                }
            }
        } else {
            pKeyPressed = false;
        }

        // Replay functionality
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            for (auto& droplet : droplets) {
                droplet.position = glm::vec3(static_cast<float>(rand()) / RAND_MAX * 4.0f - 2.0f, 5.0f, 0.0f); // Reset to random x position
                droplet.velocity = glm::vec3(0.0f, 0.0f, 0.0f); // Reset velocity
                droplet.size = 0.25f; // Reset size
                droplet.hasCollided = false; // Reset collision state
            }
            particles.clear(); // Clear all particles
            droplets.clear(); // Clear all droplets
            spawnTimer = 0.0f; // Reset spawn timer
        }

        // spawn new droplet every 1 second
        spawnTimer += deltaTime;
        if (spawnTimer >= 0.005f) {
            float randomX = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f; // Range: [-5.0f, 5.0f]
            float randomZ = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f; // Range: [-5.0f, 5.0f]
            glm::vec3 spawnPosition = glm::vec3(randomX, 5.0f, randomZ);
            glm::vec3 spawnVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
            float spawnSize = 0.1f;
            droplets.emplace_back(spawnPosition, spawnVelocity, spawnSize);
            spawnTimer = 0.0f;
        }

        // Update droplet physics
        if (!isPaused) {
            for (auto& droplet : droplets) {
                droplet.update(deltaTime, particles);
            }

            droplets.erase(std::remove_if(droplets.begin(), droplets.end(),
            [](const Droplet& droplet) {
                return droplet.velocity == glm::vec3(0.0f);
            }),
            droplets.end());

            // Update particles for droplet
            for (auto it = particles.begin(); it != particles.end();) {
                it->position += it->velocity * deltaTime;
                it->velocity.y -= 9.8f * deltaTime;
                it->life -= deltaTime; // Decrease lifetime
            
                // Remove dead particles
                if (it->life <= 0.0f) {
                    it = particles.erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);
        
        // set uniforms
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        // Update view and projection matrices
        // glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        
        // Activate shader
        glUseProgram(shaderProgram);
        
        //Set uniforms
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec3 lightBrown = glm::vec3(0.87f, 0.72f, 0.53f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "groundColor"), 1, glm::value_ptr(lightBrown));
        
        glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f); // Blue color for the droplets
        glUniform3fv(glGetUniformLocation(shaderProgram, "dropletColor"), 1, glm::value_ptr(blue));

        // Draw the ground
        glBindVertexArray(groundVAO);
        glm::mat4 groundModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(groundModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Render water droplet if it hasn't collided yet
        for (const auto& droplet : droplets) {
            if (!droplet.hasCollided) {
                glBindVertexArray(VAO);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, droplet.position);
                model = glm::scale(model, glm::vec3(droplet.size));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
            }
        }

         // Render droplet particles
        for (const auto& particle : particles) {
            glBindVertexArray(VAO); // Use the same VAO as the droplet
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, particle.position);
            model = glm::scale(model, glm::vec3(particle.size));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        }
       
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);
    glDeleteProgram(shaderProgram);
    
    // Terminate GLFW
    glfwTerminate();
    
    return 0;
}