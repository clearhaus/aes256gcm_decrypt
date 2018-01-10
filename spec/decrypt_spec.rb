require 'aes256gcm_decrypt'
require 'base64'
require 'digest'

describe 'Aes256GcmDecrypt::decrypt' do
  before(:each) do
    @ciphertext_and_tag = Base64.decode64(File.read('spec/token_data_base64.txt'))
    @key = [File.read('spec/key_hex.txt').strip].pack('H*')
  end

  it 'decrypts correctly' do
    plaintext = Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, @key)
    digest = Digest::SHA256.base64digest(plaintext)
    expect(digest).to eq '6Nltyg0FgJxJ8wt6D6XDZ0y4iRD53kLoQcTBQ0FEvyY='
  end

  it 'detects wrong parameter types' do
    expect{Aes256GcmDecrypt::decrypt(42, @key)}.to raise_error(TypeError)
    expect{Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, 42)}.to raise_error(TypeError)
  end

  it 'detects too short ciphertext_and_tag' do
    (0..16).each do |i|
      ciphertext_and_tag = 'x' * i
      expect(Aes256GcmDecrypt::decrypt(ciphertext_and_tag, @key)).to be nil
    end
  end

  it 'detects wrong key length' do
    ((0..64).to_a - [32]).each do |i|
      key = 'x' * i
      expect(Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, key)).to be nil
    end
  end

  it 'detects tampering with the ciphertext' do
    @ciphertext_and_tag[0] = 'x'
    expect(Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, @key)).to be false
  end

  it 'detects an incorrect tag' do
    @ciphertext_and_tag[-1] = 'x'
    expect(Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, @key)).to be false
  end

  it 'detects an incorrect key' do
    @key[0] = 'x'
    expect(Aes256GcmDecrypt::decrypt(@ciphertext_and_tag, @key)).to be false
  end
end
