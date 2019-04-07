local unix = require 'unix'

local libm, error = unix.dlopen('/usr/lib64/libm.so.6', 'lazy')
print(libm, error)

local f, error = libm.floor
print(f, error)
print(unix.dlsym(libm, 'floor'))
