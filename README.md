# Aes256GcmDecrypt

Decrypt AES256GCM-encrypted data in Apple Pay Payment Tokens.

This library is necessary for Ruby < 2.4 (if you use the stdlib openssl rather than the [openssl gem](https://rubygems.org/gems/openssl)), as the OpenSSL bindings do not support setting the length of the initialisation vector (IV). Setting the IV length is necessary for decrypting Apple Pay data.

The library becomes obsolete when we start using Ruby >= 2.4.

## Usage

```
bundle install
bundle exec rake test

irb -r base64 -I lib -r aes256gcm_decrypt

ciphertext_and_tag = Base64.decode64(File.read('spec/token_data_base64.txt'))
key = [File.read('spec/key_hex.txt').strip].pack('H*')

begin
  puts Aes256GcmDecrypt::decrypt(ciphertext_and_tag, key)
rescue Aes256GcmDecrypt::AuthenticationError => e
  # somebody is up to something
rescue Aes256GcmDecrypt::Error => e
  # super class for the possible errors; Aes256GcmDecrypt::InputError and
  # Aes256GcmDecrypt::OpenSSLError are left, i.e. either you supplied invalid
  # input or we got an unexpected error from OpenSSL
end
```

See also [the specs](spec/decrypt_spec.rb).

## Inspirational sources

* [Your first Ruby native extension: C](https://blog.jcoglan.com/2012/07/29/your-first-ruby-native-extension-c/)
* [Step-by-Step Guide to Building Your First Ruby Gem](https://quickleft.com/blog/engineering-lunch-series-step-by-step-guide-to-building-your-first-ruby-gem/)
* [OpenSSL EVP Authenticated Decryption using GCM](https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode)
* [The Ruby C API](http://silverhammermba.github.io/emberb/c/)
* [Apple Pay Payment Token Format Reference](https://developer.apple.com/library/content/documentation/PassKit/Reference/PaymentTokenJSON/PaymentTokenJSON.html)
* [Spreedly's gala gem](https://github.com/spreedly/gala)
