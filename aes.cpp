#include "aes.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <fstream>
#include <stdexcept>

Myaes::Myaes(){
	std::string key_str = "abcdefghijklmnopqrstu0123456789q";
	std::string iv_str = "1234567890afdwst";
	key.assign(key_str.begin(), key_str.end());
	iv.assign(iv_str.begin(), iv_str.end());
	if (key.size() != 32) {
            throw std::runtime_error("Key must be 32 bytes (256 bits) for AES-256");
        }
	if (iv.size() != 16) {
            throw std::runtime_error("IV must be 16 bytes (128 bits)");
        }	
}

Myaes::Myaes(const std::string& key_str, const std::string& iv_str){
	key.assign(key_str.begin(), key_str.end());
	iv.assign(iv_str.begin(), iv_str.end());
	if (key.size() != 32) {
            throw std::runtime_error("Key must be 32 bytes (256 bits) for AES-256");
        }
	if (iv.size() != 16) {
            throw std::runtime_error("IV must be 16 bytes (128 bits)");
        }
}

Myaes::~Myaes(){}

    
std::string Myaes::encrypt(const std::string &plaintext){
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) handleErrors();
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
        int len = 0;
        int ciphertext_len = 0;
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
		reinterpret_cast<const unsigned char*>(plaintext.data()), 
                plaintext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        ciphertext_len = len;
        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        ciphertext_len += len;
        ciphertext.resize(ciphertext_len);
        EVP_CIPHER_CTX_free(ctx);
        return std::string(ciphertext.begin(), ciphertext.end());
}

std::string Myaes::decrypt(const std::string &ciphertext) {
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) 
		handleErrors();
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        std::vector<unsigned char> plaintext(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
        int len = 0;
        int plaintext_len = 0;
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                             reinterpret_cast<const unsigned char*>(ciphertext.data()),
                             ciphertext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        plaintext_len = len;
        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleErrors();
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len);
        EVP_CIPHER_CTX_free(ctx);
	return std::string(plaintext.begin(), plaintext.end());
}

void Myaes::handleErrors(){
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("OpenSSL error occurred");
}

