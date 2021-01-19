#pragma once
#ifndef H_GLT_PROFILING
#define H_GLT_PROFILING

#define GLT_PROFILE_MAX_NAME_WIDTH 40
#define GLT_PROFILE_MAX_STATS_WIDTH 20
#define GLT_PROFILE_MAX_TIME_WIDTH 10

#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <stack>

namespace GLT {
    struct GLTProfileEntry {
        const std::string entryName;
        std::chrono::high_resolution_clock::time_point startTime;
    };

    static std::stack<GLTProfileEntry> __g_profilingEntryStack;
    static std::unordered_map<std::string, double> __g_profilingResults;
    static const std::string __g_gltSeparator(GLT_PROFILE_MAX_NAME_WIDTH + 12, '-');

    static void GLTPushProfileEntry(const std::string& name) {
        __g_profilingEntryStack.push({ name, std::chrono::high_resolution_clock::now() });
    }
    static double GLTPopProfileEntry() {
        GLTProfileEntry entry = __g_profilingEntryStack.top();
        __g_profilingEntryStack.pop();
        double duration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - entry.startTime).count() * 1000.0;
        if (__g_profilingResults.find(entry.entryName) != __g_profilingResults.end()) {
            __g_profilingResults[entry.entryName] = (__g_profilingResults[entry.entryName] + duration) / 2.0;
        }
        else {
            __g_profilingResults[entry.entryName] = duration;
        }
        return duration;
    }
    static void GLTDisplayAll() {
        std::cout << __g_gltSeparator << "\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_NAME_WIDTH) << "| NAME" << "|" \
            << std::setw(GLT_PROFILE_MAX_TIME_WIDTH) << "TIME (ms)" << "|" << "\n";
        double total = 0.0;
        for (auto epair : __g_profilingResults) {
            total += epair.second;
            std::cout << __g_gltSeparator << "\n";
            std::cout << "| " << std::left << std::setw(GLT_PROFILE_MAX_NAME_WIDTH - 2) << epair.first << "|" \
                << std::setw(GLT_PROFILE_MAX_TIME_WIDTH) << epair.second << "|" << "\n";
        }
        std::cout << __g_gltSeparator << "\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_STATS_WIDTH) << "MS PER FRAME: " << total << " ms\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_STATS_WIDTH) << "FPS: " << 1.0 / (total * .001) << "\n";
    }

    static void GLTDisplayEntries(const std::string* entries, size_t length) {
        std::cout << __g_gltSeparator << "\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_NAME_WIDTH) << "| NAME" << "|" \
            << std::setw(GLT_PROFILE_MAX_TIME_WIDTH) << "TIME (ms)" << "|" << "\n";
        double total = 0.0;
        for (size_t i = 0; i < length; i ++) {
            total += __g_profilingResults[entries[i]];
            std::cout << __g_gltSeparator << "\n";
            std::cout << "| " << std::left << std::setw(GLT_PROFILE_MAX_NAME_WIDTH - 2) << entries[i] << "|" \
                << std::setw(GLT_PROFILE_MAX_TIME_WIDTH) << __g_profilingResults[entries[i]] << "|" << "\n";
        }
        std::cout << __g_gltSeparator << "\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_STATS_WIDTH) << "MS PER FRAME: " << total << " ms\n";
        std::cout << std::left << std::setw(GLT_PROFILE_MAX_STATS_WIDTH) << "FPS: " << 1.0 / (total * .001) << "\n";
    }
}

#endif