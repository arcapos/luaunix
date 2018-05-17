/*
 * Copyright (c) 2016 - 2018, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Micro Systems Marc Balmer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Directory functions for Lua */

#include <lua.h>
#include <lauxlib.h>
#include <dirent.h>

#include "dirent.h"

int
unix_opendir(lua_State *L)
{
	DIR *dirp;
	DIR **dirpp;

	dirp = opendir(luaL_checkstring(L, 1));
	if (dirp == NULL)
		lua_pushnil(L);
	else {
		dirpp = lua_newuserdata(L, sizeof(DIR **));
		*dirpp = dirp;
		luaL_setmetatable(L, DIR_METATABLE);
	}
	return 1;
}

int
unix_readdir(lua_State *L)
{
	DIR **dirp;
	struct dirent *dirent;

	dirp = luaL_checkudata(L, 1, DIR_METATABLE);
	dirent = readdir(*dirp);
	if (dirent != NULL) {
		size_t len = 1;

		lua_newtable(L);
		lua_pushinteger(L, dirent->d_ino);
		lua_setfield(L, -2, "d_ino");
#ifdef __linux__
		lua_pushinteger(L, dirent->d_off);
		lua_setfield(L, -2, "d_off");
#endif
		lua_pushinteger(L, dirent->d_reclen);
		lua_setfield(L, -2, "d_reclen");
		lua_pushinteger(L, dirent->d_type);
		lua_setfield(L, -2, "d_type");
		lua_pushstring(L, dirent->d_name);
		lua_setfield(L, -2, "d_name");
	} else
		lua_pushnil(L);
	return 1;
}

int
unix_telldir(lua_State *L)
{
	DIR **dirp = luaL_checkudata(L, 1, DIR_METATABLE);
	lua_pushinteger(L, telldir(*dirp));
	return 1;
}

int
unix_seekdir(lua_State *L)
{
	DIR **dirp = luaL_checkudata(L, 1, DIR_METATABLE);
	seekdir(*dirp, luaL_checkinteger(L, 2));
	return 0;
}

int
unix_rewinddir(lua_State *L)
{
	DIR **dirp = luaL_checkudata(L, 1, DIR_METATABLE);
	rewinddir(*dirp);
	return 0;
}

int
unix_closedir(lua_State *L)
{
	DIR **dirp = luaL_checkudata(L, 1, DIR_METATABLE);
	if (*dirp) {
		lua_pushboolean(L, closedir(*dirp) == 0);
		*dirp = NULL;
	} else
		lua_pushboolean(L, 0);
	return 0;
}
