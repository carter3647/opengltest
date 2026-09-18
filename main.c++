#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

const std::vector<float> SCREEN_SIZE = {800,600};

Camera camera(glm::vec3(0.0f,0.0f,5.0f));
float lastX = SCREEN_SIZE[0]/2, lastY = SCREEN_SIZE[1]/2; // Screen midpoints
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool spectatorMode = false; // If true, camera will not be able to move the Y-position

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos; lastY = ypos; firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos; lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

int main() {
    // std::cout << "Current working directory test: " << __FILE__ << std::endl;
    
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_SIZE[0], SCREEN_SIZE[1], "big pepperoni what the fuck goin on", NULL, NULL);

    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // Tell GLFW to capture and hide the mouse cursor
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);

    // Format: Position (X,Y,Z), Normal (X,Y,Z), Texture UV Coordinates (U,V)
    std::vector<Vertex> cubeVertices = {
        // Each row represents: { glm::vec3(Position), glm::vec3(Normal), glm::vec2(TexCoords) }

        // -- BACK FACE (Normals point towards -Z)
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },

        // -- FRONT FACE (Normals point towards +Z)
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f} },

        // -- LEFT FACE (Normals point towards -X)
        { {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },

        // -- RIGHT FACE (Normals point towards +X)
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f,  0.0f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f,  0.0f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f,  0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f,  0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f,  0.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f,  0.0f}, {1.0f, 0.0f} },

        // -- BOTTOM FACE (Normals point towards -Y)
        { {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },

        // -- TOP FACE (Normals point towards +Y)
        { {-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f, 0.0f}, {1.0f, 1.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }
    };

    // Format: Position (X,Y,Z), Normal (X,Y,Z), Texture UV Coordinates (U,V)
    // Floor plane centered at Y = 0.0 with a size of 10x10 units.
    std::vector<Vertex> floorVertices = {
        // Upward-pointing normal (+Y) so lighting works properly on top of the floor

        // First Triangle
        { {-5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} }, // Bottom-Left
        { { 5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {10.0f, 0.0f} }, // Bottom-Right
        { { 5.0f, 0.0f,  5.0f}, {0.0f, 1.0f, 0.0f}, {10.0f, 10.0f} }, // Top-Right

        // Second Triangle
        { { 5.0f, 0.0f,  5.0f}, {0.0f, 1.0f, 0.0f}, {10.0f, 10.0f} }, // Top-Right
        { {-5.0f, 0.0f,  5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 10.0f} }, // Top-Left
        { {-5.0f, 0.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} }  // Bottom-Left
    };

    Shader cubeShader = Shader("shaders/vtx.glsl", "shaders/frag.glsl");
    Shader floorShader = Shader("shaders/vtx.glsl", "shaders/frag.glsl");
    Mesh mesh = Mesh(cubeVertices);
    Mesh floor = Mesh(floorVertices);
    Texture cubeTexture("textures/metal.jpg");
    Texture floorTexture("textures/woodbox.jpg");

    glm::vec3 lightPos(1.2f, 2.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Bright clean white light

    cubeShader.use();
    cubeShader.setInt("ourTexture", 0);
    cubeShader.setVec3("lightPos", lightPos);
    cubeShader.setVec3("lightColor", lightColor);

    floorShader.use();
    floorShader.setInt("ourTexture", 0);
    floorShader.setVec3("lightPos", lightPos);
    floorShader.setVec3("lightColor", lightColor);
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) spectatorMode = !spectatorMode; // Toggle spectator mode on/off with Left Shift key
        
        // Process WASD Keys (Calls the global camera object)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(1, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(2, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(3, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCREEN_SIZE[0] / SCREEN_SIZE[1], 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 model2 = glm::mat4(1.0f);

        model2 = glm::translate(model2,glm::vec3(0.0f,-3.0f,0.0f));

        

        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 1.0f));

        cubeShader.use();
        cubeShader.setMat4("model",model);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        cubeTexture.bind();
        mesh.draw();

        floorShader.use();
        floorShader.setMat4("model",model2);
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", projection);

        floorTexture.bind();
        floor.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
