#include "stdafx.h"
#include "Texture2D.h"
using namespace GLT;

GLTTexture2D::GLTTexture2D(GLenum type, uint32_t width, uint32_t height, GLenum sizedFormat, GLenum unsizedFormat)
{
    m_width = width;
    m_height = height;
    glGenTextures(1, &m_id);
    GLT_DEBUG_LOG_CREATE("TEXTURE", m_id);
    glBindTexture(type, m_id);
    glTexStorage2D(type, 1, sizedFormat, width, height);
    glBindTexture(type, 0);

}

GLTTexture2D::~GLTTexture2D()
{
    GLT_DEBUG_LOG_CREATE("TEXTURE", m_id);
    glDeleteTextures(1, &m_id);
}
