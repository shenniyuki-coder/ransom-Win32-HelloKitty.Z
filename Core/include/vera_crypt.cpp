#include "vera_crypt.h"
#include <cstring>

struct vc_ctx {
    unsigned char key[32]; // placeholder
};

vc_ctx* vc_init(void) {
    return new vc_ctx();
}

void vc_free(vc_ctx* ctx) {
    delete ctx;
}

int vc_encrypt_buffer(vc_ctx* ctx,
                      const unsigned char* in,
                      unsigned char* out,
                      size_t len,
                      const char* password) {
    // TODO: Replace with real VeraCrypt PBKDF2 + AES-XTS
    std::memcpy(out, in, len);
    return 0;
}

int vc_decrypt_buffer(vc_ctx* ctx,
                      const unsigned char* in,
                      unsigned char* out,
                      size_t len,
                      const char* password) {
    // TODO: Replace with real decryption
    std::memcpy(out, in, len);
    return 0;
}
