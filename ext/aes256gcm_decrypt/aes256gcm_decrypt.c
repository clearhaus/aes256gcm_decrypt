#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

VALUE method_aes256gcm_decrypt_decrypt(VALUE self, VALUE rb_ciphertext_and_tag, VALUE rb_key) {
  Check_Type(rb_ciphertext_and_tag, T_STRING);
  Check_Type(rb_key, T_STRING);

  /* Prepare variables */
  VALUE result = Qnil;

  unsigned int tag_len = 16;
  unsigned int ciphertext_and_tag_len = RSTRING_LEN(rb_ciphertext_and_tag);
  if (ciphertext_and_tag_len <= tag_len) {
    goto exit;
  }

  unsigned int ciphertext_len = ciphertext_and_tag_len - tag_len;

  unsigned char *ciphertext = calloc(ciphertext_len, sizeof(unsigned char));
  char *rb_ciphertext_p = StringValuePtr(rb_ciphertext_and_tag);
  memcpy(ciphertext, rb_ciphertext_p, ciphertext_len);

  unsigned char *tag = calloc(tag_len, sizeof(unsigned char));
  memcpy(tag, &rb_ciphertext_p[ciphertext_len], tag_len);

  unsigned int key_len = RSTRING_LEN(rb_key);
  if (key_len != 32) {
    goto cleanup1;
  }

  unsigned char *key = calloc(key_len, sizeof(unsigned char));
  memcpy(key, StringValuePtr(rb_key), key_len);

  unsigned int iv_len = 16;
  unsigned char iv[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  int plaintext_len, len;
  unsigned char *plaintext = calloc(ciphertext_len + 16, sizeof(unsigned char));

  EVP_CIPHER_CTX *ctx;

  /* Create and initialise context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    goto cleanup2;
  }

  /* Initialise decryption operation */
  if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
    goto cleanup3;
  }

  /* Set initialisation vector (IV) length */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
    goto cleanup3;
  }

  /* Initialise key and IV */
  if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
    goto cleanup3;
  }

  /* Provide message to be decrypted */
  if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    goto cleanup3;
  }
  plaintext_len = len;

  /* Set expected tag */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len, tag)) {
    goto cleanup3;
  }

  /* Finalise decryption */
  if (EVP_DecryptFinal_ex(ctx, &plaintext[len], &len) > 0) {
    plaintext_len += len;
    if (plaintext_len > ciphertext_len + 16) {
      fprintf(stderr, "Plaintext overflow in AES256GCM decryption! Aborting.\n");
      abort();
    }
    result = rb_str_new((char *)plaintext, plaintext_len);
  } else {
    result = Qfalse;
  }

cleanup3:
  EVP_CIPHER_CTX_free(ctx);
cleanup2:
  free(plaintext);
  free(key);
cleanup1:
  free(tag);
  free(ciphertext);
exit:
  return result;
}

void Init_aes256gcm_decrypt() {
  VALUE Aes256GcmDecrypt = rb_define_module("Aes256GcmDecrypt");
  rb_define_singleton_method(Aes256GcmDecrypt, "decrypt", method_aes256gcm_decrypt_decrypt, 2);
}
