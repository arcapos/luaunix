/*
 * Copyright (c) 2017, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

/* Dynamic linker interface for Lua */

#include <lua.h>
#include <lauxlib.h>
#include <dlfcn.h>

#include "dl.h"

int
unix_dlopen(lua_State *L)
{
	void *p;
	void **pp;

	p = dlopen(luaL_checkstring(L, 1), luaL_checkinteger(L, 2));
	if (p == NULL)
		lua_pushnil(L);
	else {
		pp = lua_newuserdata(L, sizeof(void **));
		*pp = p;
		luaL_setmetatable(L, DL_METATABLE);
	}
	return 1;
}

int
unix_dlerror(lua_State *L)
{
	lua_pushstring(L, dlerror());
	return 1;
}

int
unix_dlsym(lua_State *L)
{
	void **p, **s;
	void *symbol;

	p = luaL_checkudata(L, 1, DL_METATABLE);
	symbol = dlsym(*p, luaL_checkstring(L, 2));
	if (symbol) {
		s = lua_newuserdata(L, sizeof(void **));
		*s = symbol;
		luaL_setmetatable(L, DLSYM_METATABLE);
	} else
		lua_pushnil(L);
	return 1;
}

int
unix_dlclose(lua_State *L)
{
	void **p = luaL_checkudata(L, 1, DL_METATABLE);

	if (*p) {
		lua_pushinteger(L, dlclose(*p));
		*p = NULL;
		return 1;
	} else
		return 0;
}
