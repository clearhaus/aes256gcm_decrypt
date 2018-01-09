require 'mkmf'
extension_name = 'aes256gcm_decrypt'
dir_config(extension_name)
$LDFLAGS << ' -lcrypto'
create_makefile(extension_name)
