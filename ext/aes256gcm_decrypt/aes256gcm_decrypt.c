#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

VALUE Aes256GcmDecrypt = Qnil;

VALUE method_aes256gcm_decrypt_decrypt(VALUE self, VALUE rb_ciphertext, VALUE rb_tag, VALUE rb_key) {
  Check_Type(rb_ciphertext, T_STRING);
  Check_Type(rb_tag, T_STRING);
  Check_Type(rb_key, T_STRING);

  /* Prepare variables */
  unsigned int ciphertext_len = RSTRING_LEN(rb_ciphertext);
  unsigned char *ciphertext = calloc(ciphertext_len, sizeof(unsigned char));
  memcpy(ciphertext, StringValuePtr(rb_ciphertext), ciphertext_len);

  unsigned int tag_len = RSTRING_LEN(rb_tag);
  unsigned char *tag = calloc(tag_len, sizeof(unsigned char));
  memcpy(tag, StringValuePtr(rb_tag), tag_len);

  unsigned int key_len = RSTRING_LEN(rb_key);
  unsigned char *key = calloc(key_len, sizeof(unsigned char));
  memcpy(key, StringValuePtr(rb_key), key_len);

  unsigned int iv_len = 16;
  unsigned char *iv = calloc(iv_len, sizeof(unsigned char));
  for (int i = 0; i < iv_len; i++) {
    iv[i] = '\0';
  }

  unsigned int plaintext_len, len;
  unsigned char *plaintext = calloc(ciphertext_len * 2, sizeof(unsigned char));

  VALUE result = Qnil;
  EVP_CIPHER_CTX *ctx;

  /* Create and initialise context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    goto cleanup2;
  }

  /* Initialise decryption operation */
  if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
    goto cleanup1;
  }

  /* Set initialisation vector (IV) length */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
    goto cleanup1;
  }

  /* Initialise key and IV */
  if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
    goto cleanup1;
  }

  /* Provide empty string as additional authenticated data (AAD) */
  if (!EVP_DecryptUpdate(ctx, NULL, &len, "", 0)) {
    goto cleanup1;
  }

  /* Provide message to be decrypted */
  if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    goto cleanup1;
  }
  plaintext_len = len;

  /* Set expected tag */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len, tag)) {
    goto cleanup1;
  }

  /* Finalise decryption */
  if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) > 0) {
    plaintext_len += len;
    result = rb_str_new(plaintext, plaintext_len);
  } else {
    result = Qfalse;
  }

cleanup1:
  EVP_CIPHER_CTX_free(ctx);
cleanup2:
  free(plaintext);
  free(iv);
  free(key);
  free(tag);
  free(ciphertext);
  return result;
}

void Init_aes256gcm_decrypt() {
  Aes256GcmDecrypt = rb_define_module("Aes256GcmDecrypt");
  rb_define_singleton_method(Aes256GcmDecrypt, "decrypt", method_aes256gcm_decrypt_decrypt, 3);
}
