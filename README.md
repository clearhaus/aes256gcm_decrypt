# Aes256GcmDecrypt

Decrypt AES256GCM-encrypted data in Apple Pay Payment Tokens.

This library is necessary for Ruby < 2.4, as the OpenSSL bindings do not support setting the length of the initialisation vector (IV). Setting the IV length is necessary for decrypting Apple Pay data.

The library becomes obsolete when we start using Ruby >= 2.4.

## Usage

```
rake compile

irb -r base64 -r ./lib/aes256gcm_decrypt.so

token_data = Base64.decode64(File.read('test/token_data_base64.txt'))
ciphertext = token_data[0..-17]
tag = token_data[-16..-1]
key = Base64.decode64(File.read('test/key_base64.txt'))

puts Aes256GcmDecrypt::decrypt(ciphertext, tag, key)
```

## Inspirational sources

* [Your first Ruby native extension: C](https://blog.jcoglan.com/2012/07/29/your-first-ruby-native-extension-c/)
* [Step-by-Step Guide to Building Your First Ruby Gem](https://quickleft.com/blog/engineering-lunch-series-step-by-step-guide-to-building-your-first-ruby-gem/)
* [OpenSSL EVP Authenticated Decryption using GCM](https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode)
* [The Ruby C API](http://silverhammermba.github.io/emberb/c/)
