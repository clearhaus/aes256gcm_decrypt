require 'mkmf'
extension_name = 'aes256gcm_decrypt'
dir_config(extension_name)
$CFLAGS << ' -Wall'
$LDFLAGS << ' -lcrypto'
create_makefile(extension_name)
