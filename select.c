/*
 * Copyright (c) 2014, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

/* select() for Lua */

#include <sys/select.h>

#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "select.h"

/* fd_set handling functions */
int
unix_fd_set(lua_State *L)
{
	fd_set *fdset;

	fdset = (fd_set *)lua_newuserdata(L, sizeof(fd_set));
	FD_ZERO(fdset);
	luaL_getmetatable(L, FD_SET_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

int
unix_fd_set_clr(lua_State *L)
{
	fd_set *fdset;

	fdset = luaL_checkudata(L, 1, FD_SET_METATABLE);
	FD_CLR(luaL_checkinteger(L, 2), fdset);
	return 0;
}

int
unix_fd_set_isset(lua_State *L)
{
	fd_set *fdset;

	fdset = luaL_checkudata(L, 1, FD_SET_METATABLE);
	lua_pushboolean(L, FD_ISSET(luaL_checkinteger(L, 2), fdset));
	return 1;
}

int
unix_fd_set_set(lua_State *L)
{
	fd_set *fdset;

	fdset = luaL_checkudata(L, 1, FD_SET_METATABLE);
	FD_SET(luaL_checkinteger(L, 2), fdset);
	return 0;
}

int
unix_fd_set_zero(lua_State *L)
{
	fd_set *fdset;

	fdset = luaL_checkudata(L, 1, FD_SET_METATABLE);
	FD_ZERO(fdset);
	return 0;
}

/* select itself */
int
unix_select(lua_State *L)
{
	struct timeval *tv;
	fd_set *readfds, *writefds, *errorfds;
	int nfds;

	tv = NULL;
	readfds = writefds = errorfds = NULL;

	nfds = luaL_checkinteger(L, 1);
	if (lua_isuserdata(L, 2))
		readfds = luaL_checkudata(L, 2, FD_SET_METATABLE);
	if (lua_isuserdata(L, 3))
		writefds = luaL_checkudata(L, 3, FD_SET_METATABLE);
	if (lua_isuserdata(L, 4))
		errorfds = luaL_checkudata(L, 4, FD_SET_METATABLE);

	switch (lua_gettop(L)) {
	case 5:
		tv = malloc(sizeof(struct timeval));
		tv->tv_sec = 0;
		tv->tv_usec = lua_tointeger(L, 5);
		break;
	case 6:
		tv = malloc(sizeof(struct timeval));
		tv->tv_sec = lua_tointeger(L, 5);
		tv->tv_usec = lua_tointeger(L, 6);
		break;
	default:
		tv = NULL;
	}

	lua_pushinteger(L, select(nfds, readfds, writefds, errorfds, tv));
	free(tv);

	return 1;
}
