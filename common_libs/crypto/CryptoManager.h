#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <Arduino.h>
#include <ChaCha.h>
#include <base64.hpp>

class CryptoManager {
private:
    ChaCha chacha;
    uint8_t key[32];
    bool initialized = false;

    // Statistics
    uint32_t encryptCount = 0;
    uint32_t decryptCount = 0;
    uint32_t errorCount = 0;

public:
    bool init(const char* base64Key) {
        if (!base64Key || strlen(base64Key) != 44) {
            Serial.printf("[CRYPTO] ERROR: Invalid key length: %d\n",
                         base64Key ? strlen(base64Key) : 0);
            return false;
        }

        unsigned char decodedKey[64];
        unsigned int keyLen = decode_base64(
            (const unsigned char*)base64Key,
            decodedKey
        );

        if (keyLen != 32) {
            Serial.printf("[CRYPTO] ERROR: Decoded key length: %d (expected 32)\n", keyLen);
            return false;
        }

        memcpy(key, decodedKey, 32);
        initialized = true;

        Serial.println("[CRYPTO] ✓ Initialized");
        Serial.printf("[CRYPTO]   Key fingerprint: %02X%02X...%02X%02X\n",
                     key[0], key[1], key[30], key[31]);

        return true;
    }

    String encrypt(const String& plaintext) {
        if (!initialized) {
            Serial.println("[CRYPTO] ERROR: Not initialized");
            errorCount++;
            return "";
        }

        if (plaintext.length() == 0) {
            Serial.println("[CRYPTO] ERROR: Empty plaintext");
            errorCount++;
            return "";
        }

        // Generate nonce: 4 bytes timestamp + 8 bytes random
        uint8_t nonce[12];
        uint32_t timestamp = millis();
        memcpy(nonce, &timestamp, 4);

        for (int i = 4; i < 12; i += 4) {
            uint32_t random = os_random();  // FIXED: esp_random() -> os_random() for ESP8266
            memcpy(nonce + i, &random, 4);
        }

        // Encrypt
        size_t len = plaintext.length();
        uint8_t* buffer = new uint8_t[len];
        if (!buffer) {
            Serial.println("[CRYPTO] ERROR: Memory allocation failed");
            errorCount++;
            return "";
        }

        memcpy(buffer, plaintext.c_str(), len);

        chacha.setKey(key, 32);
        chacha.setIV(nonce, 12);
        chacha.encrypt(buffer, buffer, len);

        // Combine: nonce + ciphertext
        size_t totalLen = 12 + len;
        uint8_t* output = new uint8_t[totalLen];
        if (!output) {
            delete[] buffer;
            Serial.println("[CRYPTO] ERROR: Memory allocation failed");
            errorCount++;
            return "";
        }

        memcpy(output, nonce, 12);
        memcpy(output + 12, buffer, len);

        // Base64 encode
        unsigned int b64Len = encode_base64_length(totalLen);
        unsigned char* b64Output = new unsigned char[b64Len + 1];
        if (!b64Output) {
            delete[] buffer;
            delete[] output;
            Serial.println("[CRYPTO] ERROR: Memory allocation failed");
            errorCount++;
            return "";
        }

        encode_base64(output, totalLen, b64Output);
        b64Output[b64Len] = '\0';

        String result = String((char*)b64Output);

        delete[] buffer;
        delete[] output;
        delete[] b64Output;

        encryptCount++;
        Serial.printf("[CRYPTO] ✓ Encrypted %d bytes → %d bytes (total: %d)\n",
                     len, result.length(), encryptCount);

        return result;
    }

    String decrypt(const String& ciphertext) {
        if (!initialized) {
            Serial.println("[CRYPTO] ERROR: Not initialized");
            errorCount++;
            return "";
        }

        if (ciphertext.length() == 0) {
            Serial.println("[CRYPTO] ERROR: Empty ciphertext");
            errorCount++;
            return "";
        }

        // Decode base64
        unsigned char decoded[1024];
        unsigned int decodedLen = decode_base64(
            (const unsigned char*)ciphertext.c_str(),
            decoded
        );

        if (decodedLen < 12) {
            Serial.printf("[CRYPTO] ERROR: Invalid ciphertext length: %d\n", decodedLen);
            errorCount++;
            return "";
        }

        // Extract nonce
        uint8_t nonce[12];
        memcpy(nonce, decoded, 12);

        // Decrypt
        size_t dataLen = decodedLen - 12;
        uint8_t* buffer = new uint8_t[dataLen + 1];
        if (!buffer) {
            Serial.println("[CRYPTO] ERROR: Memory allocation failed");
            errorCount++;
            return "";
        }

        memcpy(buffer, decoded + 12, dataLen);

        chacha.setKey(key, 32);
        chacha.setIV(nonce, 12);
        chacha.decrypt(buffer, buffer, dataLen);

        buffer[dataLen] = '\0';
        String result = String((char*)buffer);

        delete[] buffer;

        decryptCount++;
        Serial.printf("[CRYPTO] ✓ Decrypted %d bytes → %d bytes (total: %d)\n",
                     ciphertext.length(), result.length(), decryptCount);

        return result;
    }

    bool isInitialized() const { return initialized; }

    void printStats() {
        Serial.println("[CRYPTO] Statistics:");
        Serial.printf("  Encrypted: %d\n", encryptCount);
        Serial.printf("  Decrypted: %d\n", decryptCount);
        Serial.printf("  Errors: %d\n", errorCount);
        Serial.printf("  Free heap: %d bytes\n", ESP.getFreeHeap());
    }
};

#endif
