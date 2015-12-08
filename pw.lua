local unix = require 'unix'

io.write('username: ')
local user = io.read()

local t = unix.getpwnam(user)
local s = unix.getspnam(user)
local salt = string.match(s.sp_pwdp, '(%$[^%$]+%$[^%$]+%$)')

local pw = unix.getpass('password: ')

cpw = unix.crypt(pw, salt)

if cpw == s.sp_pwdp then
	print('match')
else
	print('no match')
end

