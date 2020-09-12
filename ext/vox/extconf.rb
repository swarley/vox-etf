# frozen_string_literal: true

require('mkmf')

find_header('ruby.h')
find_header('zlib.h')
find_header('stddef.h')
find_header('stdlib.h')
find_header('stdint.h')
find_header('limits.h')
find_header('string.h')

create_makefile('vox/etf')
