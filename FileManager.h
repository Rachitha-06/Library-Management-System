#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class Library;

class FileManager {
public:
    static bool save(const Library& library, const std::string& dataDirectory,
                     std::string& message);
    static bool load(Library& library, const std::string& dataDirectory,
                     std::string& message);
};

#endif
