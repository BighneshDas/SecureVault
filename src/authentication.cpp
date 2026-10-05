#include "../include/authentication.h"
#include "../include/logger.h"

#include <openssl/sha.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <termios.h>
#include <unistd.h>

std::string Authentication::getHiddenPassword()
{
    std::string password;

    struct termios oldSettings{};
    struct termios newSettings{};

    tcgetattr(STDIN_FILENO, &oldSettings);

    newSettings = oldSettings;

    // Disable terminal echo
    newSettings.c_lflag &= ~ECHO;

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &newSettings
    );

    std::getline(
        std::cin >> std::ws,
        password
    );

    // Restore terminal settings
    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &oldSettings
    );

    std::cout << '\n';

    return password;
}


std::string Authentication::hashPassword(
    const std::string& password
) const
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(
        reinterpret_cast<const unsigned char*>(
            password.c_str()
        ),
        password.length(),
        hash
    );

    std::stringstream ss;

    for (unsigned char byte : hash) {

        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<int>(byte);
    }

    return ss.str();
}


bool Authentication::hasPassword() const
{
    std::ifstream file(
        "vault/.password"
    );

    return file.good();
}


bool Authentication::createPassword()
{
    std::string password;
    std::string confirmPassword;

    std::cout
        << "Create master password: ";

    password = getHiddenPassword();

    std::cout
        << "Confirm master password: ";

    confirmPassword = getHiddenPassword();

    if (password != confirmPassword) {

        std::cout
            << "Passwords do not match.\n";

        Logger::log(
            "Master password creation failed: "
            "passwords did not match."
        );

        return false;
    }

    if (password.empty()) {

        std::cout
            << "Password cannot be empty.\n";

        Logger::log(
            "Master password creation failed: "
            "empty password."
        );

        return false;
    }

    std::ofstream file(
        "vault/.password"
    );

    if (!file) {

        std::cout
            << "Unable to create password file.\n";

        Logger::log(
            "Master password creation failed: "
            "unable to create password file."
        );

        return false;
    }

    file << hashPassword(password);

    file.close();

    Logger::log(
        "Master password created successfully."
    );

    std::cout
        << "Master password created successfully.\n";

    return true;
}


bool Authentication::login()
{
    std::ifstream file(
        "vault/.password"
    );

    if (!file) {

        std::cout
            << "Password file not found.\n";

        return false;
    }

    std::string storedHash;
    std::string password;

    std::getline(
        file,
        storedHash
    );

    std::cout
        << "\nEnter master password: ";

    password = getHiddenPassword();

    if (hashPassword(password) == storedHash) {

        std::cout
            << "Authentication successful.\n";

        Logger::log(
            "User authentication successful."
        );

        return true;
    }

    std::cout
        << "Incorrect password.\n";

    Logger::log(
        "User authentication failed."
    );

    return false;
}
