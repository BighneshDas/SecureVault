#include "../include/file_manager.h"
#include "../include/logger.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void FileManager::addFile() {

    std::string source;

    std::cout
        << "\nEnter path of file to add: ";

    std::cin >> source;

    fs::path sourcePath(source);

    if (!fs::exists(sourcePath)) {

        std::cout
            << "File does not exist.\n";

        return;
    }

    if (!fs::is_regular_file(sourcePath)) {

        std::cout
            << "Selected path is not a regular file.\n";

        return;
    }

    fs::path destination =
        fs::path("vault")
        / sourcePath.filename();

    try {

        fs::copy_file(
            sourcePath,
            destination,
            fs::copy_options::overwrite_existing
        );

        Logger::log(
            "File added: "
            + sourcePath.filename().string()
        );

        std::cout
            << "File added successfully.\n";

    }
    catch (
        const fs::filesystem_error& e
    ) {

        std::cout
            << "File operation failed: "
            << e.what()
            << '\n';
    }
}

void FileManager::listFiles() const {

    std::cout
        << "\n========== VAULT FILES ==========\n";

    bool found = false;

    try {

        for (
            const auto& entry :
            fs::directory_iterator("vault")
        ) {

            if (
                entry.path().filename()
                == ".password"
            ) {
                continue;
            }

            if (
                fs::is_regular_file(
                    entry.path()
                )
            ) {

                std::cout
                    << "- "
                    << entry.path()
                           .filename()
                           .string()
                    << '\n';

                found = true;
            }
        }
    }
    catch (
        const fs::filesystem_error& e
    ) {

        std::cout
            << "Unable to access vault: "
            << e.what()
            << '\n';
    }

    if (!found) {

        std::cout
            << "Vault is empty.\n";
    }
}

bool FileManager::fileExists(
    const std::string& path
) const {

    return fs::exists(path);
}
