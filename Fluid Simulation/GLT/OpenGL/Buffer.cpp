#include "stdafx.h"
#include "Buffer.h"
#include <GLFW/glfw3.h>

using namespace GLT;

void GLTBufferLayout::PushEntry(GLenum type, GLsizei typeSize, GLint count, GLboolean normalized)
{
    Entry e;
    e.type = type;
    e.typeSize = typeSize;
    e.count = count;
    e.normalized = normalized;
    m_stride += typeSize * count;
    m_layout.push_back(e);
}

GLTVertexBuffer::GLTVertexBuffer(std::unique_ptr<GLTState>& state, void* data, size_t size, const GLTBufferLayout& layout)
{
    // Create and populate buffer
    glGenBuffers(1, &m_id);
    GLT_DEBUG_LOG_CREATE("VERTEX_BUFFER", m_id);
    GLuint prevBuffer = state->GetPrevious(GL_ARRAY_BUFFER);
    state->BindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    m_layout = layout.GetLayout();
    state->BindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    m_stride = layout.GetStride();
}

GLTVertexBuffer::~GLTVertexBuffer()
{
    GLT_DEBUG_LOG_DELETE("VERTEX_BUFFER", m_id);
    glDeleteBuffers(1, &m_id);
}

GLTVertexArray::GLTVertexArray()
{
    glGenVertexArrays(1, &m_id);
    GLT_DEBUG_LOG_CREATE("VERETX_ARRAY", m_id);
}

GLTVertexArray::~GLTVertexArray()
{
    GLT_DEBUG_LOG_DELETE("VERTEX_ARRAY", m_id);
    glDeleteVertexArrays(1, &m_id);
}

void GLTVertexArray::AddVertexBuffer(std::unique_ptr<GLTState>& state, const GLTVertexBuffer& buf)
{
    // Save ID of previously bound vertex array to rebind after vertex buffer creation
    GLuint prevVao = state->GetPrevious(GL_VERTEX_ARRAY);
    state->BindVertexArray(m_id);
    GLuint prevBuffer = state->GetPrevious(GL_ARRAY_BUFFER);
    state->BindBuffer(GL_ARRAY_BUFFER, buf);
    std::vector<GLTBufferLayout::Entry> layout = buf.GetLayout();
    size_t offset = 0;
    // Set up vertex array
    for (GLuint i = 0; i < layout.size(); i++) {
        glEnableVertexAttribArray(i);
        size_t offsetIncrement = layout[i].typeSize * static_cast<uint64_t>(layout[i].count);
        glVertexAttribPointer(i, layout[i].count, layout[i].type, layout[i].normalized, buf.GetStride(), (void*)offset);
        offset += offsetIncrement;
    }
    state->BindVertexArray(prevVao);
    // Restore the previously bound array buffer
    state->BindBuffer(GL_ARRAY_BUFFER, prevBuffer);
}

GLTElementBuffer::GLTElementBuffer(std::unique_ptr<GLTState>& state, void* data, size_t size)
{
    glGenBuffers(1, &m_id);
    GLT_DEBUG_LOG_CREATE("ELEMENT_BUFFER", m_id);
    // Store the ID of the previously bound element array buffer
    GLuint prevBuffer = state->GetPrevious(GL_ELEMENT_ARRAY_BUFFER);
    state->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    // Restore the previously bound element array buffer
    state->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevBuffer);
}

GLTElementBuffer::~GLTElementBuffer()
{
    GLT_DEBUG_LOG_DELETE("ELEMENT_BUFFER", m_id);
    glDeleteBuffers(1, &m_id);
}

GLTUniformBuffer::GLTUniformBuffer(std::unique_ptr<GLTState>& state, std::shared_ptr<GLTShaderProgram> program, GLsizei bufferSize, const std::string& bufferName)
{
    // Generate cpu storage for uniform buffer.
    // NOTE: Maybe change this so you aren't using 2x the space you need. Instead possibly store the layout of the buffer and update that way
    p_data = malloc(bufferSize);
    glGenBuffers(1, &m_id);
    GLT_DEBUG_LOG_CREATE("UNIFORM_BUFFER", m_id);
    state->BindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, program->GetUniformBlockIndex(bufferName), m_id);
    state->BindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLTUniformBuffer::~GLTUniformBuffer()
{
    GLT_DEBUG_LOG_DELETE("UNIFORM_BUFFER", m_id);
    glDeleteBuffers(1, &m_id);
    delete p_data;
}

void GLTUniformBuffer::UpdateBuffer(std::unique_ptr<GLTState>& state, void* data, GLsizei offset, GLsizei dataSize)
{
    // Update the uniform buffer
    state->BindBuffer(GL_UNIFORM_BUFFER, m_id);
    memcpy(p_data, reinterpret_cast<char*>(data) + offset, dataSize);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, p_data);
}
