#include "stdafx.h"
#include "Framebuffer.h"

using namespace GLT;

GLTFramebuffer::GLTFramebuffer(std::unique_ptr<GLTState>& state, const std::vector<std::shared_ptr<GLTTexture2D>>& attachments) {
    // Generate and attach color attachments to the framebuffer
    glGenFramebuffers(1, &m_id);
    GLT_DEBUG_LOG_CREATE("FRAMEBUFFER", m_id);

    GLuint prevFramebuffer = state->GetPrevious(GL_FRAMEBUFFER);
    state->BindFramebuffer(m_id);
    for (size_t i = 0; i < attachments.size(); i ++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i), GL_TEXTURE_2D, *attachments[i], 0);
    }
    state->BindFramebuffer(prevFramebuffer);
}

GLTFramebuffer::GLTFramebuffer(std::unique_ptr<GLTState>& state)
{
    glGenFramebuffers(1, &m_id);
    GLT_DEBUG_LOG_CREATE("FRAMEBUFFER", m_id);
}

GLTFramebuffer::~GLTFramebuffer() {
    GLT_DEBUG_LOG_DELETE("FRAMEBUFFER", m_id);
    glDeleteFramebuffers(1, &m_id);
}
