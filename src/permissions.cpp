#include "../include/permissions.h"
#include "../include/logger.h"

#include <filesystem>
#include <iostream>
#include <sys/stat.h>

namespace fs = std::filesystem;

void Permissions::viewPermissions() {

    std::string filename;

    std::cout
        << "\nEnter vault filename: ";

    std::cin >> filename;

    fs::path path =
        fs::path("vault") / filename;

    struct stat fileInfo{};

    if (
        stat(
            path.c_str(),
            &fileInfo
        ) != 0
    ) {

        std::cout
            << "File not found.\n";

        return;
    }

    std::cout
        << "\nPermissions for "
        << filename
        << ":\n";

    std::cout << "Owner: ";

    std::cout
        << (
            fileInfo.st_mode & S_IRUSR
            ? "R"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IWUSR
            ? "W"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IXUSR
            ? "X"
            : "-"
        );

    std::cout << "\nGroup: ";

    std::cout
        << (
            fileInfo.st_mode & S_IRGRP
            ? "R"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IWGRP
            ? "W"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IXGRP
            ? "X"
            : "-"
        );

    std::cout << "\nOthers: ";

    std::cout
        << (
            fileInfo.st_mode & S_IROTH
            ? "R"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IWOTH
            ? "W"
            : "-"
        );

    std::cout
        << (
            fileInfo.st_mode & S_IXOTH
            ? "X"
            : "-"
        );

    std::cout << '\n';
}

void Permissions::changePermissions() {

    std::string filename;

    int mode;

    std::cout
        << "\nEnter vault filename: ";

    std::cin >> filename;

    fs::path path =
        fs::path("vault") / filename;

    if (!fs::exists(path)) {

        std::cout
            << "File not found.\n";

        return;
    }

    std::cout
        << "\nLinux permission examples:\n";

    std::cout
        << "600 = Owner read/write only\n";

    std::cout
        << "644 = Owner read/write, others read\n";

    std::cout
        << "700 = Owner full access\n";

    std::cout
        << "\nEnter permission mode: ";

    std::cin >> std::oct >> mode;

    std::cin >> std::dec;

    if (
        chmod(
            path.c_str(),
            mode
        ) != 0
    ) {

        std::cout
            << "Unable to change permissions.\n";

        return;
    }

    Logger::log(
        "Permissions changed for: "
        + filename
    );

    std::cout
        << "Permissions changed successfully.\n";
}
