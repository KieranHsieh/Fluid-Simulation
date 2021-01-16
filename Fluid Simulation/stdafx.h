#pragma once

#include <iostream>

#define GLT_DEBUG

#ifdef GLT_DEBUG
//#define GLT_DEBUG_MEMORY
#endif

#ifdef GLT_DEBUG_MEMORY
#define GLT_DEBUG_LOG_CREATE(target, name) std::cout << "CREATED\t" << target << "\t" << name << "\n"
#define GLT_DEBUG_LOG_DELETE(target, name) std::cout << "DELETED\t" << target << "\t" << name << "\n"
#else
#define GLT_DEBUG_LOG_CREATE(target, name)
#define GLT_DEBUG_LOG_DELETE(target, name)
#endif