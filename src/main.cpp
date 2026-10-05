#include "../include/authentication.h"
#include "../include/file_manager.h"
#include "../include/encryption.h"
#include "../include/permissions.h"
#include "../include/logger.h"

#include <fstream>
#include <iostream>
#include <string>

int main() {

    Logger::log("SecureVault started.");

    Authentication authentication;
    FileManager fileManager;
    Encryption encryption;
    Permissions permissions;

    std::cout
        << "\n========================================\n"
        << "              SECUREVAULT\n"
        << "       Linux File Security Tool\n"
        << "========================================\n";

    if (!authentication.hasPassword()) {

        std::cout
            << "\nNo master password found.\n";

        if (!authentication.createPassword()) {
            return 1;
        }
    }

    if (!authentication.login()) {

        std::cout
            << "Access denied.\n";

        return 1;
    }

    int choice;

    do {

        std::cout
            << "\n========================================\n"
            << "                 MENU\n"
            << "========================================\n"
            << "1. Add File\n"
            << "2. List Protected Files\n"
            << "3. Encrypt File\n"
            << "4. Decrypt File\n"
            << "5. View File Permissions\n"
            << "6. Change File Permissions\n"
            << "7. View Activity Log\n"
            << "8. Exit\n"
            << "\nEnter choice: ";

        std::cin >> choice;

        switch (choice) {

            case 1:
                fileManager.addFile();
                break;

            case 2:
                fileManager.listFiles();
                break;

            case 3: {

                std::string filename;
                std::string encryptionPassword;

                std::cout
                    << "\nEnter filename to encrypt: ";

                std::cin >> filename;

                std::cout
                    << "Enter encryption password: ";

                std::cin >> encryptionPassword;

                encryption.encryptFile(
                    filename,
                    encryptionPassword
                );

                break;
            }

            case 4: {

                std::string filename;
                std::string decryptionPassword;

                std::cout
                    << "\nEnter encrypted filename: ";

                std::cin >> filename;

                std::cout
                    << "Enter decryption password: ";

                std::cin >> decryptionPassword;

                encryption.decryptFile(
                    filename,
                    decryptionPassword
                );

                break;
            }

            case 5:
                permissions.viewPermissions();
                break;

            case 6:
                permissions.changePermissions();
                break;

            case 7: {

                std::ifstream logFile(
                    "logs/activity.log"
                );

                if (!logFile) {

                    std::cout
                        << "No activity log available.\n";

                    break;
                }

                std::string line;

                std::cout
                    << "\n========== ACTIVITY LOG ==========\n";

                while (std::getline(logFile, line)) {
                    std::cout << line << '\n';
                }

                break;
            }

            case 8:

                Logger::log(
                    "SecureVault closed."
                );

                std::cout
                    << "Goodbye.\n";

                break;

            default:

                std::cout
                    << "Invalid choice.\n";
        }

    } while (choice != 8);

    return 0;
}
