#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class FileManager {

public:
    void addFile();

    void listFiles() const;

    bool fileExists(
        const std::string& path
    ) const;
};

#endif
