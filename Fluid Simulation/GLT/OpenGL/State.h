#pragma once
#ifndef H_GLT_STATE
#define H_GLT_STATE

#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLTState {
public:
    GLTState() = default;
    ~GLTState() = default;
    /*
        BindFramebuffer
            Binds a framebuffer object in OGL
        GLuint id - The ID of the framebuffer to bind
    */
    inline void BindFramebuffer(GLuint id) {
        if (m_boundTargets.find(GL_FRAMEBUFFER) == m_boundTargets.end()) {
            m_prevBoundTargets[GL_FRAMEBUFFER] = 0;
            m_boundTargets[GL_FRAMEBUFFER] = 0;
        }
        if (m_boundTargets[GL_FRAMEBUFFER] != id) {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            m_prevBoundTargets[GL_FRAMEBUFFER] = m_boundTargets[GL_FRAMEBUFFER];
            m_boundTargets[GL_FRAMEBUFFER] = id;
        }
    }
    /*
        BindBuffer
            Binds a buffer in OGL
        GLenum target               - A GLenum representing the buffer to bind. Ex. GL_ARRAY_BUFFER
        GLuint id                   - The ID of the buffer to bind
    */
    inline void BindBuffer(GLenum target, GLuint id) {
        if (m_boundTargets.find(target) == m_boundTargets.end()) {
            m_prevBoundTargets[target] = 0;
            m_boundTargets[target] = 0;
        }
        if (m_boundTargets[target] != id) {
            glBindBuffer(target, id);
            m_prevBoundTargets[target] = m_boundTargets[target];
            m_boundTargets[target] = id;
        }
    }
    /*
        BindVertexArray
            Binds a vertex array in OGL
        GLuint id   - The ID of the vertex array to bind
    */
    inline void BindVertexArray(GLuint id) {
        if (m_boundTargets.find(GL_VERTEX_ARRAY) == m_boundTargets.end()) {
            m_prevBoundTargets[GL_VERTEX_ARRAY] = 0;
            m_boundTargets[GL_VERTEX_ARRAY] = 0;
        }
        if (m_boundTargets[GL_VERTEX_ARRAY] != id) {
            glBindVertexArray(id);
            m_prevBoundTargets[GL_VERTEX_ARRAY] = m_boundTargets[GL_VERTEX_ARRAY];
            m_boundTargets[GL_VERTEX_ARRAY] = id;
        }
    }
    /*
        BindShader
            Binds a shader program in OGL
        GLuint id   - The ID of the shader program to bind
        NOTE: This is not used to bind shaders, it is used to bind programs
    */
    inline void BindShader(GLuint id) {
        if (m_boundTargets.find(GL_PROGRAM) == m_boundTargets.end()) {
            m_prevBoundTargets[GL_PROGRAM] = 0;
            m_boundTargets[GL_PROGRAM] = 0;
        }
        if (m_boundTargets[GL_PROGRAM] != id) {
            glUseProgram(id);
            m_prevBoundTargets[GL_PROGRAM] = m_boundTargets[GL_PROGRAM];
            m_boundTargets[GL_PROGRAM] = id;
        }
    }

    inline void ActiveTexture(GLuint tex) {
        if (tex != m_activeTexture) {
            glActiveTexture(GL_TEXTURE0 + tex);
            m_activeTexture = tex;
        }
    }

    /*
        GetPrevious
            Gets the id of the object previously bound to target
        GLenum target   - A GLenum representing the target to get the ID from. Ex. GL_ARRAY_BUFFER
    */
    inline GLuint GetPrevious(GLenum target) {
        return m_prevBoundTargets[target];
    }
    /*
        [] operator
            Gets the ID of the object bound to target
        GLenum target   - The target to find the previously bound object for
    */
    GLuint operator [] (GLenum target) { 
        if (m_boundTargets.find(target) == m_boundTargets.end()) {
            return 0;
        }
        return m_boundTargets[target]; 
    }
private:
    GLuint m_activeTexture = 0;
    std::unordered_map<GLenum, GLuint> m_boundTargets;
    std::unordered_map<GLenum, GLuint> m_prevBoundTargets;
};

#endif