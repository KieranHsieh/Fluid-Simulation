#include "stdafx.h"
#include "Shader.h"
#include "GLT/Utilities/Utilities.h"


using namespace GLT;

GLTShaderStage::GLTShaderStage(GLenum stageName, const std::string& srcFile)
{
    m_stage = stageName;
    m_id = glCreateShader(stageName);
    AttachSource(srcFile);
}

void GLTShaderStage::AttachSource(const std::string& srcFile)
{
    //std::string src = ReadFileToString(srcFile);
    std::set<std::string> incs;
    // Grab shader source code from the given source file
    std::string src = GetFullFileIncludes(srcFile, incs);
    const char* ccsrc = src.c_str();
    glShaderSource(m_id, 1, &ccsrc, NULL);
    glCompileShader(m_id);

    // Check for compilation errors
    int status;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint loglen;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &loglen);
        std::vector<char> log(loglen);
        glGetShaderInfoLog(m_id, loglen, NULL, log.data());
        std::string strlog(log.begin(), log.end() - 1);
        std::cout << "Failed to compile shader " << m_stage << ": " << strlog << "\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
}

GLTShaderProgram::GLTShaderProgram(const std::string& name)
{
    GLT_DEBUG_LOG_CREATE("SHADER_PROGRAM", name);
    m_id = glCreateProgram();
    m_name = name;
}

GLTShaderProgram::~GLTShaderProgram()
{
    GLT_DEBUG_LOG_DELETE("SHADER_PROGRAM", m_name);
    glDeleteProgram(m_id);
}

void GLTShaderProgram::AttachShaderStage(const GLTShaderStage& stage)
{
    glAttachShader(m_id, stage);
}

void GLTShaderProgram::Link()
{
    glLinkProgram(m_id);
    // Check for linking errors
    int status;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);
    if (!status) {
        GLint loglen;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &loglen);
        std::vector<char> log(loglen);
        glGetProgramInfoLog(m_id, loglen, NULL, log.data());
        std::string strlog(log.begin(), log.end() - 1);
        std::cout << "Failed to link program: " << strlog << "\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }

    BuildUniformCache();

    // Delete OGL shader objects
    const int maxShaders = 5;
    GLint numShaders;
    GLuint attachedShaders[maxShaders];
    glGetAttachedShaders(m_id, maxShaders, &numShaders, attachedShaders);
    for (GLint i = 0; i < numShaders; i ++) {
        glDeleteShader(attachedShaders[i]);
    }
}

void GLTShaderProgram::BuildUniformCache()
{
    BuildUniformLocationCache();
    BuildUniformBlockCache();
}

void GLTShaderProgram::BuildUniformLocationCache()
{
    // Query OGL for uniform location information
    int unis;
    glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &unis);
    const int numParams = 3;
    GLenum params[numParams] = { GL_LOCATION, GL_NAME_LENGTH, GL_BLOCK_INDEX };
    for (int i = 0; i < unis; i++) {
        GLint vals[numParams];
        glGetProgramResourceiv(m_id, GL_UNIFORM, i, numParams, params, numParams, NULL, vals);
        if (vals[0] >= 0) {
            // Get uniform name and location and save to uniform cache
            std::vector<char> nameDat(vals[1]);
            glGetProgramResourceName(m_id, GL_UNIFORM, i, vals[1], NULL, nameDat.data());
            std::string uniName(nameDat.begin(), nameDat.end() - 1);
            m_uniformCache[uniName] = vals[0];
        }
    }
}

void GLTShaderProgram::BuildUniformBlockCache()
{
    // Query OGL for uniform block information
    int unis;
    glGetProgramInterfaceiv(m_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &unis);
    const int numParams = 2;
    GLenum params[numParams] = { GL_NAME_LENGTH, GL_BUFFER_BINDING};
    for (GLsizei i = 0; i < unis; i++) {
        GLint vals[numParams];
        // Get Uniform block name and Binding and save to uniform block cache
        glGetProgramResourceiv(m_id, GL_UNIFORM_BLOCK, i, numParams, params, numParams, NULL, vals);
        std::vector<char> nameDat(vals[0]);
        
        glGetProgramResourceName(m_id, GL_UNIFORM_BLOCK, i, vals[0], NULL, nameDat.data());
        std::string blockName(nameDat.begin(), nameDat.end() - 1);
        m_uniformBlockCache[blockName] = { i, vals[1] };
    }
}
