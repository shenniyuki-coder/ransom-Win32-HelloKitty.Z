#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle for contexts
typedef struct vc_ctx vc_ctx;

// Lifecycle
vc_ctx* vc_init(void);
void    vc_free(vc_ctx*);

// Encrypt/decrypt buffers in memory
int vc_encrypt_buffer(vc_ctx* ctx,
                      const unsigned char* in,
                      unsigned char* out,
                      size_t len,
                      const char* password);

int vc_decrypt_buffer(vc_ctx* ctx,
                      const unsigned char* in,
                      unsigned char* out,
                      size_t len,
                      const char* password);

#ifdef __cplusplus
}
#endif