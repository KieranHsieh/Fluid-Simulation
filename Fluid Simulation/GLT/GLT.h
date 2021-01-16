#pragma once
#ifndef H_GLT
#define H_GLT

#include <memory>
#include "OpenGL/Buffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/State.h"

namespace GLT {
    using ElementBuffer = std::shared_ptr<GLT::GLTElementBuffer>;
    using VertexBuffer = std::shared_ptr<GLT::GLTVertexBuffer>;
    using VertexArray = std::shared_ptr<GLT::GLTVertexArray>;
    using Shader = std::shared_ptr<GLT::GLTShaderProgram>;
}

#endif