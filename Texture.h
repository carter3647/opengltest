#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

class Texture {
public:
    unsigned int textureID;

    Texture(const char path[]) {
        // GENERATE AND LOAD OPENGL TEXTURE
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Configure wrapping/filtering settings options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // OpenGL context expects images right-side up
        stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;
        std::cout << "Loading texture from: " << path << std::endl;
        unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "stb_image failure: " << stbi_failure_reason() 
              << " | Target path was: " << path << std::endl;
        }
        stbi_image_free(data); // Free system RAM image allocation since it's on VRAM now
        
        //Unbind the texture when finished initializing
        glBindTexture(GL_TEXTURE_2D, 0); 
    }

    ~Texture() {
        glDeleteTextures(1, &textureID);
    }

    void bind() {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
};
