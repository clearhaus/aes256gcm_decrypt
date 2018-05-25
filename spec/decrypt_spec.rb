# frozen_string_literal: true

require 'aes256gcm_decrypt'
require 'base64'

describe 'Aes256GcmDecrypt::decrypt' do
  before(:each) do
    @ciphertext_and_tag = Base64.decode64(File.read('spec/token_data_base64.txt'))
    @key = [File.read('spec/key_hex.txt').strip].pack('H*')
  end

  it 'decrypts correctly' do
    plaintext = Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, @key)
    expect(plaintext).to eq \
      '{"applicationPrimaryAccountNumber":"4109370251004320","applicationExp' \
      'irationDate":"200731","currencyCode":"840","transactionAmount":100,"d' \
      'eviceManufacturerIdentifier":"040010030273","paymentDataType":"3DSecu' \
      're","paymentData":{"onlinePaymentCryptogram":"Af9x/QwAA/DjmU65oyc1MAA' \
      'BAAA=","eciIndicator":"5"}}'
  end

  it 'detects wrong parameter types' do
    expect { Aes256GcmDecrypt.decrypt(42, @key) }.to \
      raise_error(Aes256GcmDecrypt::InputError, 'ciphertext_and_tag must be a string')
    expect { Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, 42) }.to \
      raise_error(Aes256GcmDecrypt::InputError, 'key must be a string')
  end

  it 'detects too short ciphertext_and_tag' do
    (0..16).each do |i|
      ciphertext_and_tag = 'x' * i
      expect { Aes256GcmDecrypt.decrypt(ciphertext_and_tag, @key) }.to \
        raise_error(Aes256GcmDecrypt::InputError, 'ciphertext_and_tag too short')
    end
  end

  it 'detects wrong key length' do
    ((0..64).to_a - [32]).each do |i|
      key = 'x' * i
      expect { Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, key) }.to \
        raise_error(Aes256GcmDecrypt::InputError, 'length of key must be 32')
    end
  end

  it 'detects tampering with the ciphertext' do
    @ciphertext_and_tag[0] = 'x'
    expect { Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, @key) }.to \
      raise_error(Aes256GcmDecrypt::AuthenticationError, 'Authentication failed')
  end

  it 'detects an incorrect tag' do
    @ciphertext_and_tag[-1] = 'x'
    expect { Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, @key) }.to \
      raise_error(Aes256GcmDecrypt::AuthenticationError, 'Authentication failed')
  end

  it 'detects an incorrect key' do
    @key[0] = 'x'
    expect { Aes256GcmDecrypt.decrypt(@ciphertext_and_tag, @key) }.to \
      raise_error(Aes256GcmDecrypt::AuthenticationError, 'Authentication failed')
  end
end
