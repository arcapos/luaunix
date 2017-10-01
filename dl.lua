local unix = require 'unix'

local libm = unix.dlopen('/usr/lib64/libm.so', 1)
print(libm)

local f = libm.floor
print(f)
print(unix.dlsym(libm, 'floor'))
