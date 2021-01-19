#include "stdafx.h"
#include "App.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

#include "OpenGL/Shader.h"
#include "OpenGL/State.h"
#include "OpenGL/Buffer.h"
#include "OpenGL/Texture2D.h"
#include "OpenGL/Framebuffer.h"

/*
    Helper function for texture creation
*/
static void SetupTexture(GLuint texID, GLenum filter) {
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

#define GLT_MAKE_TEXTURE(varName, type, width, height, sizedFormat, unsizedFormat) \
    std::make_shared<GLTTexture2D>(type, width, height, sizedFormat, unsizedFormat); \
    glBindTexture(type, *varName); \
    SetupTexture(*varName, GL_LINEAR);

using namespace GLT;

App::App(const WindowInfo& info)
{
    _m_color[0] = float(rand()) / RAND_MAX;
    _m_color[1] = float(rand()) / RAND_MAX;
    _m_color[2] = float(rand()) / RAND_MAX;
    // GLFW Initialization
    m_windowInfo = info;
    if (glfwInit() == GLFW_FALSE) {
        std::cout << "Failed to initialize GLFW\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef GLT_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    // Using OGL 4.4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    p_windowHandle = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);
    if (p_windowHandle == NULL) {
        std::cout << "Failed to create GLFW window\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(p_windowHandle);
#ifdef GLT_DEBUG
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load glad\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#else
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
#endif
    glfwSetWindowUserPointer(p_windowHandle, this);
    SetupOGLGLFWCallbacks();
    p_state = std::make_unique<GLTState>();
}

App::~App()
{
    delete _m_color;
    glfwDestroyWindow(p_windowHandle);
    glfwTerminate();
}

void App::SetupOGLGLFWCallbacks()
{
    glfwSetCursorPosCallback(p_windowHandle, [](GLFWwindow* win, double x, double y) {
        App* shouldClose = reinterpret_cast<App*>(glfwGetWindowUserPointer(win));
        shouldClose->SetCursorPos(x, y);
    });
    glfwSetMouseButtonCallback(p_windowHandle, [](GLFWwindow* win, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_1) {
            App* application = reinterpret_cast<App*>(glfwGetWindowUserPointer(win));
            if (action == GLFW_RELEASE) {
                application->SetMouseDown(false);
            }
            else if (action == GLFW_PRESS) {
                application->SetMouseDown(true);
                application->_m_color[0] = float(rand()) / RAND_MAX;
                application->_m_color[1] = float(rand()) / RAND_MAX;
                application->_m_color[2] = float(rand()) / RAND_MAX;
            }
        }
    });
    glfwSetKeyCallback(p_windowHandle, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            App* shouldClose = reinterpret_cast<App*>(glfwGetWindowUserPointer(win));
            shouldClose->SetShouldClose(true);
        }
    });
    glfwSetWindowCloseCallback(p_windowHandle, [](GLFWwindow* win) {
        App* shouldClose = reinterpret_cast<App*>(glfwGetWindowUserPointer(win));
        shouldClose->SetShouldClose(true);
    });
#ifdef GLT_DEBUG
    glDebugMessageCallback([](GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam) {

        const char* ctype = nullptr;
        switch (type) {
        case GL_DEBUG_TYPE_ERROR: ctype = "Error"; break;
        case GL_DEBUG_TYPE_OTHER: ctype = "Other"; break;
        case GL_DEBUG_TYPE_MARKER: ctype = "Marker"; break;
        case GL_DEBUG_TYPE_POP_GROUP: ctype = "Pop Group"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: ctype = "Push Group"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: ctype = "Performance"; break;
        case GL_DEBUG_TYPE_PORTABILITY: ctype = "Portability"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: ctype = "Undefined Behavior"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ctype = "Deprecated Behavior"; break;
        }

        const char* cseverity = nullptr;
        switch (severity) {
        case GL_DEBUG_SEVERITY_LOW: cseverity = "LOW Severity"; break;
        case GL_DEBUG_SEVERITY_HIGH: cseverity = "HIGH Severity"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: cseverity = "MEDIUM Severity"; break;
        }

        std::cout << "OpenGL " << ctype << ":: " << cseverity << ":: " << message << "\n";
        std::cout << "----------------------------------------------------------------\n";
        

    }, NULL);
#endif
}


void App::Start()
{    

    /*
        Shader Creation
    
    */

    GLTShaderStage baseVertexStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage advectionShaderStage(GL_FRAGMENT_SHADER, "resources/advectionShader.frag");
    std::shared_ptr<GLTShaderProgram> advectionShader = std::make_shared<GLTShaderProgram>("Advection Shader");
    advectionShader->AttachShaderStage(baseVertexStage);
    advectionShader->AttachShaderStage(advectionShaderStage);
    advectionShader->Link();
    
    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage splatShaderStage(GL_FRAGMENT_SHADER, "resources/splat.frag");
    std::shared_ptr<GLTShaderProgram> splatShader = std::make_shared<GLTShaderProgram>("Splat Shader");
    splatShader->AttachShaderStage(baseVertexStage);
    splatShader->AttachShaderStage(splatShaderStage);
    splatShader->Link();

    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage jacobiShaderStage(GL_FRAGMENT_SHADER, "resources/jacobiIterationShader.frag");
    std::shared_ptr<GLTShaderProgram> jacobiShader = std::make_shared<GLTShaderProgram>("Jacobi Shader");
    jacobiShader->AttachShaderStage(baseVertexStage);
    jacobiShader->AttachShaderStage(jacobiShaderStage);
    jacobiShader->Link();

    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage divergenceShaderStage(GL_FRAGMENT_SHADER, "resources/divergenceShader.frag");
    std::shared_ptr<GLTShaderProgram> divergenceShader = std::make_shared<GLTShaderProgram>("Divergence Shader");
    divergenceShader->AttachShaderStage(baseVertexStage);
    divergenceShader->AttachShaderStage(divergenceShaderStage);
    divergenceShader->Link();

    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage gradientShaderStage(GL_FRAGMENT_SHADER, "resources/gradientSubtractionShader.frag");
    std::shared_ptr<GLTShaderProgram> gradientShader = std::make_shared<GLTShaderProgram>("Gradient Shader");
    gradientShader->AttachShaderStage(baseVertexStage);
    gradientShader->AttachShaderStage(gradientShaderStage);
    gradientShader->Link();

    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage displayShaderStage(GL_FRAGMENT_SHADER, "resources/displayShader.frag");
    std::shared_ptr<GLTShaderProgram> displayShader = std::make_shared<GLTShaderProgram>("Display Shader");
    displayShader->AttachShaderStage(baseVertexStage);
    displayShader->AttachShaderStage(displayShaderStage);
    displayShader->Link();
    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage vorticityGradShaderStage(GL_FRAGMENT_SHADER, "resources/vorticityGradShader.frag");
    std::shared_ptr<GLTShaderProgram> vorticityGradShader = std::make_shared<GLTShaderProgram>("Vorticity Gradient Shader");
    vorticityGradShader->AttachShaderStage(baseVertexStage);
    vorticityGradShader->AttachShaderStage(vorticityGradShaderStage);
    vorticityGradShader->Link();
    
    baseVertexStage = GLTShaderStage(GL_VERTEX_SHADER, "resources/vertexShaderBase.vert");
    GLTShaderStage vorticityShaderStage(GL_FRAGMENT_SHADER, "resources/vorticityShader.frag");
    std::shared_ptr<GLTShaderProgram> vorticityShader = std::make_shared<GLTShaderProgram>("");
    vorticityShader->AttachShaderStage(baseVertexStage);
    vorticityShader->AttachShaderStage(vorticityShaderStage);
    vorticityShader->Link();
    


    /*
        Create Full Screen Quad VBO,VAO, and EBO
    */

    GLfloat quadVertices[] = {
        -1.0f, -1.0f,       +0.0f,                      +0.0f,                                                                  // Bottom Left
        -1.0f, +1.0f,       +0.0f,                      static_cast<GLfloat>(m_windowInfo.simHeight),                           // Top Left
        +1.0f, -1.0f,       static_cast<GLfloat>(m_windowInfo.simWidth),      +0.0f,                                            // Bottom Right
        +1.0f, +1.0f,       static_cast<GLfloat>(m_windowInfo.simWidth),      static_cast<GLfloat>(m_windowInfo.simHeight),     // Top Right
    };
    GLuint quadIndices[] = {
        0, 2, 1,
        1, 2, 3
    };
    GLTBufferLayout quadLayout;
    quadLayout.PushEntry(GL_FLOAT, sizeof(GLfloat), 2, GL_FALSE);
    quadLayout.PushEntry(GL_FLOAT, sizeof(GLfloat), 2, GL_FALSE);
    std::shared_ptr<GLTVertexBuffer> quadVBO = std::make_shared<GLTVertexBuffer>(p_state, quadVertices, sizeof(quadVertices), quadLayout);
    std::shared_ptr<GLTElementBuffer> quadEBO = std::make_shared<GLTElementBuffer>(p_state, quadIndices, sizeof(quadIndices));
    std::shared_ptr<GLTVertexArray> quadVAO = std::make_shared<GLTVertexArray>();
    quadVAO->AddVertexBuffer(p_state, *quadVBO);
    p_state->BindVertexArray(*quadVAO);
    p_state->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, *quadEBO);

    /*
        Create Textures
    */
    std::shared_ptr<GLTTexture2D> u1Tex = GLT_MAKE_TEXTURE(u1Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RG32F, GL_RG);
    std::shared_ptr<GLTTexture2D> u2Tex = GLT_MAKE_TEXTURE(u2Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RG32F, GL_RG);
    std::shared_ptr<GLTTexture2D> p1Tex = GLT_MAKE_TEXTURE(p1Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RG32F, GL_RG);
    std::shared_ptr<GLTTexture2D> p2Tex = GLT_MAKE_TEXTURE(p2Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RG32F, GL_RG);
    std::shared_ptr<GLTTexture2D> c1Tex = GLT_MAKE_TEXTURE(c1Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RGBA32F, GL_RGBA);
    std::shared_ptr<GLTTexture2D> c2Tex = GLT_MAKE_TEXTURE(c2Tex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RGBA32F, GL_RGBA);
    std::shared_ptr<GLTTexture2D> openTex = GLT_MAKE_TEXTURE(openTex, GL_TEXTURE_RECTANGLE_NV, m_windowInfo.simWidth, m_windowInfo.simHeight, GL_RG32F, GL_RG);

    /*
        Create Framebuffers
    */

    std::shared_ptr<GLTFramebuffer> targetFBO = std::make_shared<GLTFramebuffer>(p_state);


    /*
        Create Uniform Bufferse
    */
    
    std::shared_ptr<GLTUniformBuffer> shaderDataBuffer = std::make_shared<GLTUniformBuffer>(p_state, advectionShader, 
        sizeof(GLfloat) * 4, "shaderData");
    {
        GLfloat shaderData[] = {
            float(m_windowInfo.simWidth), float(m_windowInfo.simHeight),
            1.0f / m_windowInfo.dx, 
            0.5f / m_windowInfo.dx
        };
        shaderDataBuffer->UpdateBuffer(p_state, shaderData, 0, sizeof(GLfloat) * 4);
    }
    
    /*
        Texture Swapping
    */

    // Velocity Textures & Temporary swap var
    GLuint uREAD = *u1Tex;
    GLuint uWRITE = *u2Tex;
    GLuint uTEMP = 0;

    // Texture for vorticity and divergence
    GLuint freeTex = *openTex;

    // Pressure Textures & Swap var
    GLuint pREAD = *p1Tex;
    GLuint pWRITE = *p2Tex;
    GLuint pTEMP = 0;


    // Color Textures & Swap var
    GLuint cREAD = *c1Tex;
    GLuint cWRITE = *c2Tex;
    GLuint cTEMP = 0;

    /*
        Time tracking
    */

    float currTime = static_cast<float>(glfwGetTime());
    float lastTime = currTime;
    float dTime = currTime - lastTime;

    /*
        Main Render Loop
    */
    while (!m_shouldClose) {
        
        auto now = std::chrono::high_resolution_clock::now();
        lastTime = currTime;
        currTime = static_cast<float>(glfwGetTime());
        dTime = currTime - lastTime;


        p_state->BindFramebuffer(0);
        glClear(GL_COLOR_BUFFER_BIT);

        p_state->BindVertexArray(*quadVAO);
        
        p_state->BindFramebuffer(*targetFBO);
        glViewport(0, 0, m_windowInfo.simWidth, m_windowInfo.simHeight);

        /*-----------------------------------
         |          ADVECTION STAGE         |
         -----------------------------------*/
        
        GLT_DEBUG_PUSH("Advection");
        p_state->BindShader(*advectionShader);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, uWRITE, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(advectionShader->GetUniformLocation("timestep"), m_windowInfo.timeStep);
        glUniform1f(advectionShader->GetUniformLocation("dissipation"), m_windowInfo.velocityDissipation);
        glUniform1i(advectionShader->GetUniformLocation("velocityTex"), 0);
        glUniform1i(advectionShader->GetUniformLocation("targetTex"), 1);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);
        p_state->ActiveTexture(1);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, cWRITE, 0);

        glUniform1f(advectionShader->GetUniformLocation("dissipation"), m_windowInfo.inkDissipation);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uWRITE);
        p_state->ActiveTexture(1);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, cREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        uTEMP = uREAD;
        uREAD = uWRITE;
        uWRITE = uTEMP;

        cTEMP = cREAD;
        cREAD = cWRITE;
        cWRITE = cTEMP;
        GLT_DEBUG_POP();
        
        /*---------------------------------------
         |          ADD IMPULSE STAGE           |
         ---------------------------------------*/
        
        GLT_DEBUG_PUSH("Add Impulse");

        if (m_m1down) {
            p_state->BindShader(*splatShader);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, uWRITE, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            float xAdjust = float(m_windowInfo.simWidth) / float(m_windowInfo.width);
            float yAdjust = float(m_windowInfo.simHeight) / float(m_windowInfo.height);
            float xDirRaw = static_cast<float>(p_mousePos[0] - p_mousePosOld[0]) * xAdjust;
            float yDirRaw = -static_cast<float>(p_mousePos[1] - p_mousePosOld[1]) * yAdjust;
            float len = sqrt((xDirRaw * xDirRaw) + (yDirRaw * yDirRaw));
            float xDir = (xDirRaw / len) * m_windowInfo.force;
            float yDir = (yDirRaw / len) * m_windowInfo.force;
            glUniform3f(splatShader->GetUniformLocation("color"), xDir, yDir, 0.0);
            glUniform2f(splatShader->GetUniformLocation("position"), 
                static_cast<float>(p_mousePos[0] / m_windowInfo.width), 
                static_cast<float>(1 - (p_mousePos[1] / m_windowInfo.height)));
            glUniform1f(splatShader->GetUniformLocation("radius"), 0.5f);
            glUniform1i(splatShader->GetUniformLocation("base"), 0);

            p_state->ActiveTexture(0);
            glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, cWRITE, 0);
            glUniform3f(splatShader->GetUniformLocation("color"), _m_color[0], _m_color[1], _m_color[2]);

            p_state->ActiveTexture(0);
            glBindTexture(GL_TEXTURE_RECTANGLE_NV, cREAD);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

            uTEMP = uREAD;
            uREAD = uWRITE;
            uWRITE = uTEMP;

            cTEMP = cREAD;
            cREAD = cWRITE;
            cWRITE = cTEMP;
        }
    
        GLT_DEBUG_POP();
        /*-------------------------------------
         |          VORTICITY STAGE           |
         -------------------------------------*/
        
        GLT_DEBUG_PUSH("Vorticity");

        p_state->BindShader(*vorticityGradShader);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, freeTex, 0);

        glUniform1i(vorticityGradShader->GetUniformLocation("u"), 0);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        
        p_state->BindShader(*vorticityShader);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, uWRITE, 0);

        glUniform1i(vorticityShader->GetUniformLocation("vort"), 0);
        glUniform1i(vorticityShader->GetUniformLocation("u"), 1);
        glUniform1f(vorticityShader->GetUniformLocation("timestep"), m_windowInfo.timeStep);
        glUniform1f(vorticityShader->GetUniformLocation("epsilon"), 2.4414e-4f);
        glUniform2f(vorticityShader->GetUniformLocation("dxscale"), m_windowInfo.curl * m_windowInfo.dx, m_windowInfo.curl * m_windowInfo.dx);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, freeTex);
        p_state->ActiveTexture(1);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        
        uTEMP = uREAD;
        uREAD = uWRITE;
        uWRITE = uTEMP;

        GLT_DEBUG_POP();
        /*-------------------------------------
         |          DIVERGENCE STAGE          |
         -------------------------------------*/
        
        GLT_DEBUG_PUSH("Divergence");

        p_state->BindShader(*divergenceShader);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, freeTex, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(divergenceShader->GetUniformLocation("w"), 0);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        GLT_DEBUG_POP();
        /*--------------------------------------- 
         |          CALCULATE PRESSURE          |
         ---------------------------------------*/
        
        GLT_DEBUG_PUSH("Pressure");

        p_state->BindShader(*jacobiShader);
        
        glUniform1i(jacobiShader->GetUniformLocation("b"), 0);
        glUniform1i(jacobiShader->GetUniformLocation("x"), 1);
        glUniform1f(jacobiShader->GetUniformLocation("alpha"), -m_windowInfo.dx * m_windowInfo.dx);
        glUniform1f(jacobiShader->GetUniformLocation("rBeta"), 0.25f);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, freeTex);

        p_state->ActiveTexture(1);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, pWRITE);
        glClearTexImage(pWRITE, 0, GL_RG, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, pREAD);
        glClearTexImage(pREAD, 0, GL_RG, GL_FLOAT, NULL);
        

        const unsigned int numItersPressure = 40;
        for (unsigned int i = 0; i < numItersPressure; i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, pREAD, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            p_state->ActiveTexture(1);
            glBindTexture(GL_TEXTURE_RECTANGLE_NV, pWRITE);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
            
            pTEMP = pREAD;
            pREAD = pWRITE;
            pWRITE = pTEMP;
        }

        GLT_DEBUG_POP();
        /*-------------------------------------------------------
         |          SUBTRACT PRESSURE GRADIENT STAGE            |
         -------------------------------------------------------*/
        
        GLT_DEBUG_PUSH("Subtract Gradient");

        p_state->BindShader(*gradientShader);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, uWRITE, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(gradientShader->GetUniformLocation("p"), 0);
        glUniform1i(gradientShader->GetUniformLocation("w"), 1);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, pREAD);
        p_state->ActiveTexture(1);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, uREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        
        uTEMP = uREAD;
        uREAD = uWRITE;
        uWRITE = uTEMP;

        GLT_DEBUG_POP();
        /*---------------------------------------------
         |          COPY TO DRAW FRAMEBUFFER          |
         ---------------------------------------------*/
        
        GLT_DEBUG_PUSH("Draw");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, cWRITE, 0);
        
        p_state->BindShader(*displayShader);

        glUniform1i(displayShader->GetUniformLocation("tex"), 0);

        p_state->ActiveTexture(0);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, cREAD);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        GLT_DEBUG_POP();
        
        GLT_DEBUG_PUSH("Copy To Default Framebuffer");

        p_state->BindFramebuffer(0);
        glViewport(0, 0, m_windowInfo.width, m_windowInfo.height);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlitNamedFramebuffer(*targetFBO, 0,
            0, 0, m_windowInfo.simWidth, m_windowInfo.simHeight,
            0, 0, m_windowInfo.width, m_windowInfo.height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        GLT_DEBUG_POP();
        
        GLT_DEBUG_PUSH("GLFW Calls");
        glfwSwapBuffers(p_windowHandle);
        glfwPollEvents();
#ifdef GLT_DEBUG_PROFILE
        if (!m_shouldClose) {
            GLT_DEBUG_POP();
        }
#endif
    }
    GLT_DEBUG_DISPALL();
}
