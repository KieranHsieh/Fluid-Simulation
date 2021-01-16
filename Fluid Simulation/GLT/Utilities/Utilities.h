#pragma once
#ifndef H_GLT_UTILITIES
#define H_GLT_UTILITIES


#include <set>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#define GLT_PPDIRECTIVE_INCLUDE "include"

namespace GLT {
    /*
        Read File To String
            Reads a file to a string
        const std::string& filePath - The path to the file to read from
    */
    static std::string ReadFileToString(const std::string& filePath) {
        if (!std::filesystem::exists(filePath)) {
            std::cout << "Failed to find file: " << filePath << "\n";
            std::cin.get();
            exit(EXIT_FAILURE);
        }
        std::ifstream f(filePath);
        std::stringstream ss;
        ss << f.rdbuf();
        f.close();
        return ss.str();
    }

    /*
        GetRegexResults
            A helper function to run regex on a string
        const std::string& exp      - The regex expression to use
        const std::string& target   - The target string to parse
    */
    static std::vector<std::string> GetRegexResults(const std::regex& exp, const std::string& target) {
        std::vector<std::string> retMatches;
        std::smatch matches;
        std::regex_match(target, matches, exp);
        unsigned int numMatches = matches.size();
        if (!numMatches) {
            return retMatches;
        }
        retMatches.reserve(numMatches);
        for (int m = 0; m < matches.size(); m++) {
            retMatches.emplace_back(matches[m]);
        }
        return retMatches;
    }

    // Fixes a path with ".."s by removing them and iterating up the path the number of ".."s there were
    // This function assumes no filename at the end of the original filePath
    // This is a helper function for GetFullFileIncludes
    // Ex. C:/hello/hello2/hello3/../.. will become C:/hello/
    static void GetRealPath(std::filesystem::path& filePath) {
        unsigned int numDirectories = 0;
        // Remove all ".."s and iterate upwards
        while (filePath.filename().string() == "..") {
            numDirectories++;
            filePath = filePath.parent_path();
        }
        // Iterate upwards through the directory the number of removed ".."s
        for (unsigned int i = 0; i < numDirectories; i++) {
            filePath = filePath.parent_path();
        }
    }

    // RELATIVE PATHS ONLY
    /*
        GetFullFileIncludes
            Allows files to have a "#include "filename" to copy other files into that one
        const std::string& filePath             - The path to the file to read from
        std::set<std::string>& foundIncludes    - A set containing any files to exclude from the includes
    */
    static std::string GetFullFileIncludes(const std::string& filePath, std::set<std::string>& foundIncludes) {
        // Check if file exists
        if (!std::filesystem::exists(filePath)) {
            std::cout << "Failed to find file: " << filePath << "\n";
            std::cin.get();
            exit(EXIT_FAILURE);
        }
        // Grab parent file path and filename from the given filePath
        std::filesystem::path realfp(filePath);
        std::string filename = realfp.filename().string();
        realfp.make_preferred();
        // The given filepath is assumed to have an actual filename at the end, so a call to parent_path is needed to remove it
        realfp = realfp.parent_path();
        GetRealPath(realfp);
        std::filesystem::path realfpfull = realfp;
        // Readd the filename
        realfpfull.append(filename);
        realfpfull.make_preferred();
        // Attempt to add the current file to the set of discovered includes. This is needed for the case that it is the first call
        // to the function, where the set is empty and the file tries to include itself.
        foundIncludes.insert(realfpfull.string());

        // Add the current filename to the set of excluded (already found) files
        std::stringstream ss;
        std::ifstream f(filePath);
        // Regex explanation: 
        //      ^#([a-zA-Z]*) = the start of the string will be a # followed by any characters a-z case insensitive
        //       \"(.*)\" = a space followed by any length string enclosed in quotes.
        //      Capture group 1 is the preprocessor command aka include
        //      Capture group 2 is the path for the include, which MUST be relative at this time
        const std::regex exp("^#([a-zA-Z]*) \"(.*)\"");

        // Parse through the current file and add lines / parse more includes
        while (!f.eof()) {
            std::string curLine;
            std::getline(f, curLine);
            // Run regex match on the current line of the file.
            // Based on our regex string, the vector should be 3 long, with the contets:
            // [matching string, capture group 1, capture group 2]
            std::vector<std::string> matches = GetRegexResults(exp, curLine);
            if (!matches.empty()) {
                const std::string_view directive = matches[1];
                const std::string_view includeFileName = matches[2];

                // Matches is a regex string with two capture groups
                if (directive == GLT_PPDIRECTIVE_INCLUDE) {
                    std::filesystem::path incfp = realfp;
                    incfp.append(includeFileName);
                    std::string incfilename = incfp.filename().string();
                    // Since GetRealPath expects a filepath with no filenames at the end, the filename must be removed
                    incfp = incfp.parent_path();
                    // matches[1] might be in the form ../../test.frag, so a call to GetRealPath is needed
                    GetRealPath(incfp);
                    // Readd the filename
                    incfp.append(incfilename);
                    incfp.make_preferred();
                    std::string incfpstr = incfp.string();
                    // Recurse if the included file has not been included before.
                    if (foundIncludes.find(incfpstr) == foundIncludes.end()) {
                        foundIncludes.insert(incfpstr);
                        ss << GetFullFileIncludes(incfpstr, foundIncludes);
                    }
                }
            }
            // If the given line is not a preprocessor directive, simply add it to the return string
            else {
                ss << curLine << "\n";
            }
        }
        return ss.str();
    }
}

#undef GLT_PPDIRECTIVE_INCLUDE

#endif