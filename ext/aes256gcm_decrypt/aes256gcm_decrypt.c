#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

VALUE aes256gcm_input_error;
VALUE aes256gcm_openssl_error;
VALUE aes256gcm_auth_error;

VALUE aes256gcm_decrypt(VALUE self, VALUE rb_ciphertext_and_tag, VALUE rb_key) {

  /* Declare variables */
  VALUE result;
  unsigned int ciphertext_and_tag_len, ciphertext_len, tag_len;
  unsigned int block_len, key_len, iv_len;
  int plaintext_len, len;
  unsigned char *ciphertext, *tag, *key, *plaintext;
  char *rb_ciphertext_p, *openssl_error_message;
  unsigned char iv[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  EVP_CIPHER_CTX *ctx;

  /* Check parameters */
  if (!RB_TYPE_P(rb_ciphertext_and_tag, T_STRING)) {
    rb_raise(aes256gcm_input_error, "ciphertext_and_tag must be a string");
  }
  if (!RB_TYPE_P(rb_key, T_STRING)) {
    rb_raise(aes256gcm_input_error, "key must be a string");
  }
  ciphertext_and_tag_len = RSTRING_LEN(rb_ciphertext_and_tag);
  tag_len = 16;
  if (ciphertext_and_tag_len <= tag_len) {
    rb_raise(aes256gcm_input_error, "ciphertext_and_tag too short");
  }
  key_len = RSTRING_LEN(rb_key);
  if (key_len != 32) {
    rb_raise(aes256gcm_input_error, "length of key must be 32");
  }

  /* Prepare variables */
  result = Qnil;
  block_len = 16;
  iv_len = 16;

  ciphertext_len = ciphertext_and_tag_len - tag_len;
  ciphertext = calloc(ciphertext_len, sizeof(unsigned char));
  rb_ciphertext_p = StringValuePtr(rb_ciphertext_and_tag);
  memcpy(ciphertext, rb_ciphertext_p, ciphertext_len);

  tag = calloc(tag_len, sizeof(unsigned char));
  memcpy(tag, &rb_ciphertext_p[ciphertext_len], tag_len);

  key = calloc(key_len, sizeof(unsigned char));
  memcpy(key, StringValuePtr(rb_key), key_len);

  plaintext = calloc(ciphertext_len + block_len, sizeof(unsigned char));

  /* Create and initialise context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    openssl_error_message = "Could not create and initialise context";
    goto cleanup1;
  }

  /* Initialise decryption operation */
  if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
    openssl_error_message = "Could not initialise decryption operation";
    goto cleanup2;
  }

  /* Set initialisation vector (IV) length */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
    openssl_error_message = "Could not set IV length";
    goto cleanup2;
  }

  /* Initialise key and IV */
  if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
    openssl_error_message = "Could not initialise key and IV";
    goto cleanup2;
  }

  /* Provide message to be decrypted */
  if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    openssl_error_message = "DecryptUpdate failed";
    goto cleanup2;
  }
  plaintext_len = len;

  /* Set expected tag */
  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len, tag)) {
    openssl_error_message = "Could not set expected tag";
    goto cleanup2;
  }

  /* Finalise decryption */
  if (EVP_DecryptFinal_ex(ctx, &plaintext[len], &len) > 0) {
    plaintext_len += len;
    if ((unsigned int)plaintext_len > ciphertext_len + block_len) {
      fprintf(stderr, "Plaintext overflow in AES256GCM decryption! Aborting.\n");
      abort();
    }
    result = rb_str_new((char *)plaintext, plaintext_len);
  } else {
    result = Qfalse;
  }

cleanup2:
  EVP_CIPHER_CTX_free(ctx);

cleanup1:
  free(plaintext);
  free(key);
  free(tag);
  free(ciphertext);

  switch (result) {
    case Qnil:
      rb_raise(aes256gcm_openssl_error, "%s", openssl_error_message);
    case Qfalse:
      rb_raise(aes256gcm_auth_error, "Authentication failed");
    default:
      return result;
  }
}

void Init_aes256gcm_decrypt() {
  VALUE module, error;

  module = rb_define_module("Aes256GcmDecrypt");
  error = rb_define_class_under(module, "Error", rb_eStandardError);
  aes256gcm_input_error = rb_define_class_under(module, "InputError", error);
  aes256gcm_openssl_error = rb_define_class_under(module, "OpenSSLError", error);
  aes256gcm_auth_error = rb_define_class_under(module, "AuthenticationError", error);
  rb_define_singleton_method(module, "decrypt", aes256gcm_decrypt, 2);
}
