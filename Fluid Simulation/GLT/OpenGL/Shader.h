#pragma once
#ifndef H_GLT_SHADER
#define H_GLT_SHADER

#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace GLT {
    class GLTShaderStage {
    public:
        /*
            GLTShaderStage
                An abstraction for a shader stage in OGL (Fragment, Vertex, etc.)
            GLenum stageName            - A GLenum representing the shader stage. Ex. GL_FRAGMENT_SHADER
            const std::string& srcFile  - A file path to the source code of the shader
        */
        GLTShaderStage(GLenum stageName, const std::string& srcFile);
        /*
            GetStage
                Returns a GLenum representing the shader stage. Ex. GL_VERTEX_SHADER
        */
        inline GLenum GetStage() const { return m_stage; }
        /*
            Implicit cast to a GLuint that uses the ID of the Shader Stage as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        /*
            AttachSource
                Reads shader source code from srcFile and attaches it to the shader stage
            const std::string& srcFile  - The source file to read from
        */
        void AttachSource(const std::string& srcFile);
        GLenum m_stage;
        GLuint m_id;
    };

    class GLTShaderProgram {
    public:
        struct UniformBlockData {
            GLint index;
            GLint binding;
        };
        /*
            GLTShaderProgram
                An abstraction for a program OGL object
            const std::string& name - The name of the shader, this has nothing to do with the name of the file containing the associated
                shader stages
        */
        GLTShaderProgram(const std::string& name);
        ~GLTShaderProgram();
        /*
            AttachShaderStage
                Attaches a specified GLTShaderStage to the program
            const GLTShaderStage& stage - A reference to the shader stage to be attached
        */
        void AttachShaderStage(const GLTShaderStage& stage);
        /*
            Link
                Performs shader program linking and populates the uniform and uniform block caches
        */
        void Link();
        /*
            GetUniformLocation
                Returns the location of the specified uniform in the program
            const std::string& uniformName  - The name of the uniform to get the location for
        */
        GLuint GetUniformLocation(const std::string& uniformName) { return m_uniformCache[uniformName]; };
        /*
            GetUniformBlockIndex
                Returns the uniform block index of the specified uniform in the program
            const std::string& uniformName  - The name of the uniform to get the block index for
        */
        GLuint GetUniformBlockIndex(const std::string& uniformName) { return m_uniformBlockCache[uniformName].binding; };
        /*
            GetUniformBlockBinding
                Returns the uniform block binding for the specified uniform in the program
            const std::string& uniformName  - The name of hte uniform to get the block binding for
        */
        GLuint GetUniformBlockBinding(const std::string& uniformName) { return m_uniformBlockCache[uniformName].index; };
        /*
            Implicit cast to a GLuint that uses the ID of the program as the return value
        */
        operator GLuint() const { return m_id; }
    private:
        /*
            BuildUniformCache
                Populates the uniform cache with active uniforms in the program
        */
        void BuildUniformCache();
        /*
            BuildUniformLocationCache
                Populates m_uniformCache withe the locations of associated uniforms
        */
        void BuildUniformLocationCache();
        /*
            BuildUniformBlockCache
                Popultes m_uniformBlockCache with information about the uniform block
        */
        void BuildUniformBlockCache();
        std::unordered_map<std::string, GLuint> m_uniformCache;
        std::unordered_map<std::string, UniformBlockData> m_uniformBlockCache;
        std::string m_name;
        GLuint m_id;
    };
}

#endif