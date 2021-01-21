#pragma once
#ifndef H_GLT_APP
#define H_GLT_APP

#include "GLT/OpenGL/State.h"
#include "GLT/OpenGL/Shader.h"
#include <memory>
#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GLT {
    class App {
    public:
        struct WindowInfo {
            uint32_t width;
            uint32_t height;
            uint32_t simWidth;
            uint32_t simHeight;
            const char* title;
            float timeStep;
            float vorticityScale;
            float velocityDissipation;
            float inkDissipation;
            float dx;
            float force;
            float pRadius;
        };
        App(const WindowInfo& info);
        ~App();
        void Start();
        inline void SetShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }
        inline void SetCursorPos(double x, double y) { 
            p_mousePosOld[0] = p_mousePos[0]; 
            p_mousePosOld[1] = p_mousePos[1]; 
            p_mousePos[0] = x; 
            p_mousePos[1] = y; 
        }
        inline void SetMouseDown(bool isDown) { m_m1down = isDown; }

        float* _m_color = new float[3];
    private:
        // Helper function that sets up basic GLFW callback functions
        void SetupOGLGLFWCallbacks();

        bool m_m1down = false;
        double p_mousePos[2];
        double p_mousePosOld[2];
        WindowInfo m_windowInfo;
        GLFWwindow* p_windowHandle;
        bool m_shouldClose;
        std::unique_ptr<GLTState> p_state;
    };
}

#endif