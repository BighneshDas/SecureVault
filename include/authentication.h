#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <string>

class Authentication {
public:
    bool hasPassword() const;
    bool createPassword();
    bool login();

    static std::string getHiddenPassword();

private:
    std::string hashPassword(
        const std::string& password
    ) const;

    std::string getStoredHash() const;

    bool saveHash(
        const std::string& hash
    ) const;
};

#endif
