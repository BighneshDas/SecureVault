#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

class Encryption {
public:
    bool encryptFile(
        const std::string& filename,
        const std::string& password
    );

    bool decryptFile(
        const std::string& filename,
        const std::string& password
    );

private:
    bool deriveKey(
        const std::string& password,
        const unsigned char* salt,
        int saltLength,
        unsigned char* key
    );
};

#endif
