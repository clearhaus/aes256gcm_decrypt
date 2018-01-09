Gem::Specification.new do |s|
  s.name    = 'aes256gcm_decrypt'
  s.version = '0.0.1'
  s.summary = 'Decrypt AES256GCM-encrypted data in Apple Pay Payment Tokens'
  s.author  = 'Clearhaus'

  s.files = Dir.glob("ext/**/*.{c,rb}") +
            Dir.glob("lib/**/*.rb")

  s.extensions << "ext/aes256gcm_decrypt/extconf.rb"

  s.add_development_dependency "rake-compiler"
end
