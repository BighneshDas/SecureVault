#include "../include/encryption.h"
#include "../include/logger.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr int SALT_SIZE = 16;
constexpr int IV_SIZE = 12;
constexpr int KEY_SIZE = 32;
constexpr int TAG_SIZE = 16;

}

bool Encryption::deriveKey(
    const std::string& password,
    const unsigned char* salt,
    int saltLength,
    unsigned char* key
) {
    return PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.size()),
        salt,
        saltLength,
        100000,
        EVP_sha256(),
        KEY_SIZE,
        key
    ) == 1;
}

bool Encryption::encryptFile(
    const std::string& filename,
    const std::string& password
) {
    fs::path inputPath =
        fs::path("vault") / filename;

    if (!fs::exists(inputPath)) {
        std::cout << "File not found.\n";
        return false;
    }

    if (filename.size() >= 4 &&
        filename.substr(filename.size() - 4) == ".enc") {
        std::cout << "File is already encrypted.\n";
        return false;
    }

    std::ifstream input(
        inputPath,
        std::ios::binary
    );

    if (!input) {
        std::cout << "Unable to open file.\n";
        return false;
    }

    std::vector<unsigned char> plaintext(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>()
    );

    input.close();

    unsigned char salt[SALT_SIZE];
    unsigned char iv[IV_SIZE];
    unsigned char key[KEY_SIZE];
    unsigned char tag[TAG_SIZE];

    if (RAND_bytes(salt, SALT_SIZE) != 1 ||
        RAND_bytes(iv, IV_SIZE) != 1) {

        std::cout << "Unable to generate secure random data.\n";
        return false;
    }

    if (!deriveKey(
            password,
            salt,
            SALT_SIZE,
            key)) {

        std::cout << "Key derivation failed.\n";
        return false;
    }

    EVP_CIPHER_CTX* ctx =
        EVP_CIPHER_CTX_new();

    if (!ctx) {
        std::cout << "Unable to create encryption context.\n";
        return false;
    }

    if (EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            nullptr,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_IVLEN,
            IV_SIZE,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_EncryptInit_ex(
            ctx,
            nullptr,
            nullptr,
            key,
            iv) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> ciphertext(
        plaintext.size() + 16
    );

    int len = 0;
    int ciphertextLength = 0;

    if (!plaintext.empty()) {

        if (EVP_EncryptUpdate(
                ctx,
                ciphertext.data(),
                &len,
                plaintext.data(),
                static_cast<int>(plaintext.size())
            ) != 1) {

            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        ciphertextLength = len;
    }

    if (EVP_EncryptFinal_ex(
            ctx,
            ciphertext.data() + ciphertextLength,
            &len
        ) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    ciphertextLength += len;

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_GET_TAG,
            TAG_SIZE,
            tag
        ) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);

    fs::path outputPath =
        inputPath.string() + ".enc";

    std::ofstream output(
        outputPath,
        std::ios::binary
    );

    if (!output) {
        std::cout << "Unable to create encrypted file.\n";
        return false;
    }

    output.write(
        reinterpret_cast<char*>(salt),
        SALT_SIZE
    );

    output.write(
        reinterpret_cast<char*>(iv),
        IV_SIZE
    );

    output.write(
        reinterpret_cast<char*>(ciphertext.data()),
        ciphertextLength
    );

    output.write(
        reinterpret_cast<char*>(tag),
        TAG_SIZE
    );

    output.close();

    Logger::log(
        "File encrypted: " + filename
    );

    std::cout
        << "File encrypted successfully.\n";

    std::cout
        << "Created: "
        << outputPath.filename().string()
        << '\n';

    return true;
}

bool Encryption::decryptFile(
    const std::string& filename,
    const std::string& password
) {
    fs::path inputPath =
        fs::path("vault") / filename;

    if (!fs::exists(inputPath)) {
        std::cout << "Encrypted file not found.\n";
        return false;
    }

    std::ifstream input(
        inputPath,
        std::ios::binary
    );

    if (!input) {
        std::cout << "Unable to open encrypted file.\n";
        return false;
    }

    std::vector<unsigned char> data(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>()
    );

    input.close();

    if (data.size() <
        SALT_SIZE + IV_SIZE + TAG_SIZE) {

        std::cout << "Invalid encrypted file.\n";
        return false;
    }

    unsigned char* salt = data.data();

    unsigned char* iv =
        data.data() + SALT_SIZE;

    std::size_t ciphertextOffset =
        SALT_SIZE + IV_SIZE;

    std::size_t ciphertextSize =
        data.size()
        - SALT_SIZE
        - IV_SIZE
        - TAG_SIZE;

    unsigned char* ciphertext =
        data.data() + ciphertextOffset;

    unsigned char* tag =
        data.data()
        + data.size()
        - TAG_SIZE;

    unsigned char key[KEY_SIZE];

    if (!deriveKey(
            password,
            salt,
            SALT_SIZE,
            key)) {

        std::cout << "Key derivation failed.\n";
        return false;
    }

    EVP_CIPHER_CTX* ctx =
        EVP_CIPHER_CTX_new();

    if (!ctx) {
        return false;
    }

    if (EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            nullptr,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_IVLEN,
            IV_SIZE,
            nullptr) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_DecryptInit_ex(
            ctx,
            nullptr,
            nullptr,
            key,
            iv) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> plaintext(
        ciphertextSize + 16
    );

    int len = 0;
    int plaintextLength = 0;

    if (ciphertextSize > 0) {

        if (EVP_DecryptUpdate(
                ctx,
                plaintext.data(),
                &len,
                ciphertext,
                static_cast<int>(ciphertextSize)
            ) != 1) {

            EVP_CIPHER_CTX_free(ctx);
            return false;
        }

        plaintextLength = len;
    }

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_TAG,
            TAG_SIZE,
            tag
        ) != 1) {

        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalResult =
        EVP_DecryptFinal_ex(
            ctx,
            plaintext.data() + plaintextLength,
            &len
        );

    EVP_CIPHER_CTX_free(ctx);

    if (finalResult != 1) {

        std::cout
            << "Decryption failed.\n";

        std::cout
            << "Incorrect password or corrupted file.\n";

        Logger::log(
            "Failed decryption attempt: "
            + filename
        );

        return false;
    }

    plaintextLength += len;

    std::string originalName = filename;

    if (originalName.size() >= 4 &&
        originalName.substr(
            originalName.size() - 4
        ) == ".enc") {

        originalName =
            originalName.substr(
                0,
                originalName.size() - 4
            );
    }

    fs::path outputPath =
        fs::path("vault") / originalName;

    std::ofstream output(
        outputPath,
        std::ios::binary
    );

    if (!output) {
        std::cout
            << "Unable to create decrypted file.\n";
        return false;
    }

    output.write(
        reinterpret_cast<char*>(
            plaintext.data()
        ),
        plaintextLength
    );

    output.close();

    Logger::log(
        "File decrypted: " + filename
    );

    std::cout
        << "File decrypted successfully.\n";

    std::cout
        << "Created: "
        << outputPath.filename().string()
        << '\n';

    return true;
}
