Gem::Specification.new do |s|
  s.name     = 'aes256gcm_decrypt'
  s.version  = '0.0.2'
  s.summary  = 'Decrypt AES256GCM-encrypted data in Apple Pay Payment Tokens'
  s.author   = 'Clearhaus'
  s.homepage = 'https://github.com/clearhaus/aes256gcm_decrypt'
  s.license  = 'MIT'

  s.files    = `git ls-files -z`.split("\x0")

  s.extensions << "ext/aes256gcm_decrypt/extconf.rb"

  s.required_ruby_version = '< 2.4'

  s.add_development_dependency "rake-compiler"
  s.add_development_dependency "rspec"
end
