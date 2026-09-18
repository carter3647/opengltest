#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Camera Vectors
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    // Mouse Angles (Yaw initialized to -90 so it points down the -Z axis initially)
    float Yaw   = -90.0f;
    float Pitch =  0.0f;

    // Movement Speeds
    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;

    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f)) {
        Position = startPosition;
        Up = glm::vec3(0.0f, 1.0f, 0.0f);
        updateCameraVectors();
    }

    // Returns the final View Matrix for your shader
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 1. HANDLES KEYBOARD (WASD)
    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == 0) Position += Front * velocity; // W (Forward)
        if (direction == 1) Position -= Front * velocity; // S (Backward)
        if (direction == 2) Position -= glm::normalize(glm::cross(Front, Up)) * velocity; // A (Left)
        if (direction == 3) Position += glm::normalize(glm::cross(Front, Up)) * velocity; // D (Right)
    }

    // 2. HANDLES MOUSE MOVEMENT
    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Constraint: Lock the camera from flipping completely upside down (just like Roblox)
        if (Pitch > 89.0f)  Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateCameraVectors();
    }

private:
    // Recalculates the Front vector whenever mouse angles shift
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
    }
};
