#pragma once

#include <iostream>

#define GLT_DEBUG

#include <iostream>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>


#ifdef GLT_DEBUG
//#define GLT_DEBUG_MEMORY
#define GLT_DEBUG_PROFILE
#endif

#ifdef GLT_DEBUG_MEMORY
#define GLT_DEBUG_LOG_CREATE(target, name) std::cout << "CREATED\t" << target << "\t" << name << "\n"
#define GLT_DEBUG_LOG_DELETE(target, name) std::cout << "DELETED\t" << target << "\t" << name << "\n"
#else
#define GLT_DEBUG_LOG_CREATE(target, name)
#define GLT_DEBUG_LOG_DELETE(target, name)
#endif

#ifdef GLT_DEBUG_PROFILE
#include "GLT/Utilities/Profiling.h"
#define GLT_DEBUG_PUSH(name) GLT::GLTPushProfileEntry(name)
#define GLT_DEBUG_POP() GLT::GLTPopProfileEntry()
#define GLT_DEBUG_DISPALL() GLT::GLTDisplayAll()
#define GLT_DEBUG_DISP(size, ...) { \
    std::string glttempentries[size] = {__VA_ARGS__};\
    GLT::GLTDisplayEntries(glttempentries, size);\
}
#else
#define GLT_DEBUG_PUSH(name)
#define GLT_DEBUG_POP()
#define GLT_DEBUG_DISPALL()
#define GLT_DEBUG_DISP(name)
#endif