/*
 * Copyright (c) 2011 - 2016, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

/* Lua binding for Unix */

#include <grp.h>
#include <lua.h>
#include <lauxlib.h>
#include <pwd.h>

#ifdef __linux__
#include <bsd/bsd.h>
#include <shadow.h>
#endif

#include "pwd.h"

int
unix_setpwent(lua_State *L)
{
	setpwent();
	return 0;
}

int
unix_endpwent(lua_State *L)
{
	endpwent();
	return 0;
}

static void
unix_pushpasswd(lua_State *L, struct passwd *pwd)
{
	lua_newtable(L);
	lua_pushstring(L, pwd->pw_name);
	lua_setfield(L, -2, "pw_name");
	lua_pushstring(L, pwd->pw_passwd);
	lua_setfield(L, -2, "pw_passwd");
	lua_pushinteger(L, pwd->pw_uid);
	lua_setfield(L, -2, "pw_uid");
	lua_pushinteger(L, pwd->pw_gid);
	lua_setfield(L, -2, "pw_gid");
	lua_pushstring(L, pwd->pw_gecos);
	lua_setfield(L, -2, "pw_gecos");
	lua_pushstring(L, pwd->pw_dir);
	lua_setfield(L, -2, "pw_dir");
	lua_pushstring(L, pwd->pw_shell);
	lua_setfield(L, -2, "pw_shell");
}

int
unix_getpwent(lua_State *L)
{
	struct passwd *pwd;

	pwd = getpwent();
	if (pwd != NULL)
		unix_pushpasswd(L, pwd);
	else
		lua_pushnil(L);
	return 1;
}

int
unix_getpwnam(lua_State *L)
{
	struct passwd *pwd;

	pwd = getpwnam(luaL_checkstring(L, 1));
	if (pwd != NULL)
		unix_pushpasswd(L, pwd);
	else
		lua_pushnil(L);
	return 1;
}

int
unix_getpwuid(lua_State *L)
{
	struct passwd *pwd;

	pwd = getpwuid(luaL_checkinteger(L, 1));
	if (pwd != NULL)
		unix_pushpasswd(L, pwd);
	else
		lua_pushnil(L);
	return 1;
}

#ifdef __linux__
static void
unix_pushspasswd(lua_State *L, struct spwd *spwd)
{
	lua_newtable(L);
	lua_pushstring(L, spwd->sp_namp);
	lua_setfield(L, -2, "sp_namp");
	lua_pushstring(L, spwd->sp_pwdp);
	lua_setfield(L, -2, "sp_pwdp");
	lua_pushinteger(L, spwd->sp_lstchg);
	lua_setfield(L, -2, "sp_lstchg");
	lua_pushinteger(L, spwd->sp_min);
	lua_setfield(L, -2, "sp_min");
	lua_pushinteger(L, spwd->sp_max);
	lua_setfield(L, -2, "sp_max");
	lua_pushinteger(L, spwd->sp_warn);
	lua_setfield(L, -2, "sp_warn");
	lua_pushinteger(L, spwd->sp_inact);
	lua_setfield(L, -2, "sp_inact");
	lua_pushinteger(L, spwd->sp_expire);
	lua_setfield(L, -2, "sp_expire");
}

int
unix_getspnam(lua_State *L)
{
	struct spwd *spwd;

	spwd = getspnam(luaL_checkstring(L, 1));
	if (spwd != NULL)
		unix_pushspasswd(L, spwd);
	else
		lua_pushnil(L);
	return 1;
}
#endif

static void
unix_pushgroup(lua_State *L, struct group *grp)
{
	int n;
	char **mem;

	lua_newtable(L);
	lua_pushstring(L, grp->gr_name);
	lua_setfield(L, -2, "gr_name");
	lua_pushstring(L, grp->gr_passwd);
	lua_setfield(L, -2, "gr_passwd");
	lua_pushinteger(L, grp->gr_gid);
	lua_setfield(L, -2, "gr_gid");
	lua_newtable(L);

	for (n = 1, mem = grp->gr_mem; *mem != NULL; mem++, n++) {
		lua_pushinteger(L, n);
		lua_pushstring(L, *mem);
		lua_settable(L, -3);
	}
	lua_setfield(L, -2, "gr_mem");
}

int
unix_getgrnam(lua_State *L)
{
	struct group *grp;

	grp = getgrnam(luaL_checkstring(L, 1));
	if (grp != NULL)
		unix_pushgroup(L, grp);
	else
		lua_pushnil(L);
	return 1;
}

int
unix_getgrgid(lua_State *L)
{
	struct group *grp;

	grp = getgrgid(luaL_checkinteger(L, 1));
	if (grp != NULL)
		unix_pushgroup(L, grp);
	else
		lua_pushnil(L);
	return 1;
}
