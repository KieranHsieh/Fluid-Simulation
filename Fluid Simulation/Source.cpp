#include <iostream>
#include "GLT/App.h"

int main() {
    {
        GLT::App::WindowInfo info;
        info.width = 800;
        info.height = 600;
        info.simWidth = 512;
        info.simHeight = 512;
        info.title = "Fluid Simulation";
        // Fluid Sim Parameters
        info.dx = 1.0;
        info.timeStep = 1.0;
        info.curl = 0.3f;
        info.velocityDissipation = 0.99f;
        info.inkDissipation = 0.99f;
        float* color = new float[3];
        color[0] = 1.0;
        color[1] = 0.1;
        color[2] = 0.1;
        info.color = color;

        GLT::App app(info);
        app.Start();
        delete[] color;
    }
    std::cin.get();
}