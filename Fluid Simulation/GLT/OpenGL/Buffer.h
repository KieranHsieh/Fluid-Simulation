#pragma once
#ifndef H_GLT_BUFFER
#define H_GLT_BUFFER

#include <vector>
#include <memory>
#include "State.h"
#include "Shader.h"
#include <glad/glad.h>

namespace GLT {
    class GLTBufferLayout {
    public:
        struct Entry {
            GLenum type;
            GLsizei typeSize;
            GLint count;
            GLboolean normalized;
        };
        GLTBufferLayout() = default;
        /*
            GetLayout
                Returns the BufferLayout as a vector of Entries
        */
        inline std::vector<Entry> GetLayout() const { return m_layout; }
        /*
            GetStride
                Returns the stride for the buffer
        */
        inline GLsizei GetStride() const { return m_stride; }
        /*
            PushEntry
                Pushes a buffer layout entry to the current buffer
            
            GLenum type             - The data type of the entry
            GLsizei typeSize         - The size of the data type
            GLint count             - The number of type in the entry: Ex. count = 3 for an entry that is 3 floats
            GLboolean normalized    - Whether or not to tell OGL to normalize the entry
        */
        void PushEntry(GLenum type, GLsizei typeSize, GLint count, GLboolean normalized);
    private:
        std::vector<Entry> m_layout;
        GLsizei m_stride = 0;
    };

    class GLTVertexBuffer {
    public:
        /*
            GLTVertexBuffer
                A simple class that abstracts vertex buffer creation
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
            void* data                          - A pointer to the data for the buffer
            size_t size                         - The size of the data in bytes
            const GLTBufferLayout& layout       - A reference to the layout for the buffer
        */
        GLTVertexBuffer(std::unique_ptr<GLTState>& state, void* data, size_t size, const GLTBufferLayout& layout);
        ~GLTVertexBuffer();

        /*
            GetStride
                Returns the stride for the buffer
        */
        inline GLsizei GetStride() const { return m_stride; }
        /*
            Implicit cast to a GLuint that uses the ID of the buffer as the return value
        */
        operator GLuint() const { return m_id; }
        /*
            GetLayout
                Returns the layout of the vertex buffer as a vector of GLTBufferLayout entries
        */
        inline std::vector<GLTBufferLayout::Entry> GetLayout() const { return m_layout; }
    private:
        GLuint m_id;
        GLsizei m_stride;
        std::vector<GLTBufferLayout::Entry> m_layout;
    };

    class GLTElementBuffer {
    public:
        /*
            GLTElementBuffer
                An abstraction for an element buffer / index buffer object
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
            void* data                          - A pointer to the data for the buffer
            size_t size                         - The size of the data in bytes
        */
        GLTElementBuffer(std::unique_ptr<GLTState>& state, void* data, size_t size);
        ~GLTElementBuffer();
        /*
            Implicit cast to a GLuint that uses the ID of the buffer as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        GLuint m_id;
    };

    class GLTVertexArray {
    public:
        /*
            GLTVertexArray
                An abstraction for a vertex array for OGL
        */
        GLTVertexArray();
        ~GLTVertexArray();
        /*
            AddVertexBuffer
                Attaches a GLTVertexBuffer object to the vertex array
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
            const GLTVertexBuffer& buf          - A reference to the GLTVertexBuffer to attach to the vertex array
        */
        void AddVertexBuffer(std::unique_ptr<GLTState>& state, const GLTVertexBuffer& buf);
        /*
            Implicit cast to a GLuint that uses the ID of the buffer as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        GLuint m_id;
    };

    class GLTUniformBuffer {
    public:
        /*
            GLTUniformBuffer
                An abstraction for a uniform buffer object for OGL
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
            std::shared_ptr<GLTShaderProgram>   - A pointer to a GLTShaderProgram object that utilizes the uniform buffer
            GLsizei bufferSize                  - The size of the buffer in bytes
            const std::string& bufferName       - The name of the buffer in the shader
        */
        GLTUniformBuffer(std::unique_ptr<GLTState>& state, 
            std::shared_ptr<GLTShaderProgram> program, 
            GLsizei bufferSize, 
            const std::string& bufferName);
        ~GLTUniformBuffer();
        /*
            UpdateBuffer
                Updates the uniform buffer with given data
            std::unique_ptr<GLTState>& state    - A pointer to the current OGL State
            void* data                          - A pointer to the data to update the buffer with
            GLsizei offset                      - The offset into the data pointer to begin grabbing data
            GLsizei dataSize                    - The size in bytes of data to grab
        */
        void UpdateBuffer(std::unique_ptr<GLTState>& state, void* data, GLsizei offset, GLsizei dataSize);
    private:
        GLuint m_id;
        void* p_data;
    };
}

#endif