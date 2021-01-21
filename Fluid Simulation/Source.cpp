#include <iostream>
#include "GLT/App.h"
#include "GLT/Utilities/Profiling.h"
#include <thread>
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
        info.timeStep = 0.5;
        info.vorticityScale = 0.4f;
        info.velocityDissipation = 0.9999f;
        info.inkDissipation = 0.999f;
        info.force = 1.0f;
        info.pRadius = 100.0f;

        GLT::App app(info);
        app.Start();
    }
    std::cin.get();
}