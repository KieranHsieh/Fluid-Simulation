#pragma once
#ifndef H_GLT_TEXTURE2D
#define H_GLT_TEXTURE2D

#include <iostream>
#include <stdint.h>
#include "State.h"
#include <glad/glad.h>

namespace GLT {
    class GLTTexture2D {
    public:
        /*
            Texture2D
                An abstraction for a 2D Texture in OGL
            GLenum type                         - The type of 2D texture. E.g. GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE
            uint32_t width                      - The width of the texture
            uint32_t height                     - The height of the texture
            GLenum sizedFormat                  - A GLenum for the number of components AND precision. Ex. GL_RGBA32F
            GLenum unsizedFormat                - A GLenum for the number of components. Ex. GL_RGBA
        */
        GLTTexture2D(GLenum type, uint32_t width, uint32_t height, GLenum sizedFormat, GLenum unsizedFormat);

        ~GLTTexture2D();
        /*
            GetWidth
                Gets the width of the texture
        */
        uint32_t GetWidth() const { return m_width; }
        /*
            GetHeight
                Gets the height of the texture
        */
        uint32_t GetHeight() const { return m_height; }
        /*
            Implicit cast to a GLuint that uses the ID of the buffer as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        GLuint m_id;
        uint32_t m_width;
        uint32_t m_height;
    };
}

#endif