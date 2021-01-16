#pragma once
#ifndef H_GLT_FRAMEBUFFER
#define H_GLT_FRAMEBUFFER

#include "Texture2D.h"
#include "State.h"
#include <glad/glad.h>

namespace GLT {
    class GLTFramebuffer {
    public:
        /*
            Framebuffer
                An abstraction for a Framebuffer object in OGL
            std::unique_ptr<GLTState>& state                            - A pointer to the current OGL State
            const std::vector<std::shared_ptr<Texture2D>>& attachments  - A vector of pointers to Texture2D objects that represent the framebuffer color attachments
        */
        GLTFramebuffer(std::unique_ptr<GLTState>& state, const std::vector<std::shared_ptr<GLTTexture2D>>& attachments);
        /*
            GLTFramebuffer
                An abstraction for a Framebuffer object in OGL
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
        */
        GLTFramebuffer(std::unique_ptr<GLTState>& state);
        ~GLTFramebuffer();
        /*
            Implicit cast to a GLuint that uses the ID of the buffer as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        GLuint m_id;
    };
}

#endif