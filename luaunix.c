/*
 * Copyright (c) 2011 - 2018, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

#ifdef __linux__
#include <alloca.h>
#include <bsd/bsd.h>
#endif
#include <errno.h>
#include <grp.h>
#include <lua.h>
#include <lauxlib.h>
#include <pwd.h>
#ifdef __linux__
#include <shadow.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include "luaunix.h"
#include "dirent.h"
#include "pwd.h"
#include "select.h"
#include "dl.h"

extern char *crypt(const char *key, const char *salt);
typedef void (*sighandler_t)(int);

static void
reaper(int signal)
{
	wait(NULL);
}

static int
unix_arc4random(lua_State *L)
{
	lua_pushinteger(L, arc4random());
	return 1;
}

static int
unix_chdir(lua_State *L)
{
	lua_pushinteger(L, chdir(luaL_checkstring(L, 1)));
	return 1;
}

static int
unix_dup2(lua_State *L)
{
	lua_pushinteger(L, dup2(luaL_checkinteger(L, 1),
	    luaL_checkinteger(L, 2)));
	return 1;
}

static int
unix_errno(lua_State *L)
{
	lua_pushinteger(L, errno);
	return 1;
}

static int
unix_fork(lua_State *L)
{
	lua_pushinteger(L, fork());
	return 1;
}

static int
unix_kill(lua_State *L)
{
	lua_pushinteger(L, kill((pid_t)luaL_checkinteger(L, 1),
	    luaL_checkinteger(L, 2)));
	return 1;
}

static int
unix_getcwd(lua_State *L)
{
	char *cwd;

	cwd = alloca(PATH_MAX);
	if (getcwd(cwd, PATH_MAX) != NULL)
		lua_pushstring(L, cwd);
	else
		lua_pushnil(L);
	return 1;
}

static int
unix_getpass(lua_State *L)
{
	lua_pushstring(L, getpass(luaL_checkstring(L, 1)));
	return 1;
}

static int
unix_getpid(lua_State *L)
{
	lua_pushinteger(L, getpid());
	return 1;
}

static int
unix_setpgid(lua_State *L)
{
	lua_pushinteger(L, setpgid(luaL_checkinteger(L, 1),
	    luaL_checkinteger(L, 2)));
	return 1;
}

static int
unix_sleep(lua_State *L)
{
	lua_pushinteger(L, sleep(luaL_checkinteger(L, 1)));
	return 1;
}

static int
unix_unlink(lua_State *L)
{
	lua_pushinteger(L, unlink(luaL_checkstring(L, 1)));
	return 1;
}

static int
unix_getuid(lua_State *L)
{
	lua_pushinteger(L, getuid());
	return 1;
}

static int
unix_getgid(lua_State *L)
{
	lua_pushinteger(L, getgid());
	return 1;
}

static int
unix_chown(lua_State *L)
{
	lua_pushinteger(L, chown(luaL_checkstring(L, 1),
	    luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)));
	return 1;
}

static int
unix_chmod(lua_State *L)
{
	lua_pushinteger(L, chmod(luaL_checkstring(L, 1),
	    luaL_checkinteger(L, 2)));
	return 1;
}

static int
unix_rename(lua_State *L)
{
	lua_pushinteger(L, rename(luaL_checkstring(L, 1),
	    luaL_checkstring(L, 2)));
	return 1;
}

static int
unix_stat(lua_State *L)
{
	struct stat statbuf;

	if (stat(luaL_checkstring(L, 1), &statbuf))
		lua_pushnil(L);
	else {
		lua_newtable(L);
		lua_pushinteger(L, statbuf.st_dev);
		lua_setfield(L, -2, "st_uid");
		lua_pushinteger(L, statbuf.st_ino);
		lua_setfield(L, -2, "st_ino");
		lua_pushinteger(L, statbuf.st_mode);
		lua_setfield(L, -2, "st_mode");
		lua_pushinteger(L, statbuf.st_nlink);
		lua_setfield(L, -2, "st_nlink");
		lua_pushinteger(L, statbuf.st_uid);
		lua_setfield(L, -2, "st_uid");
		lua_pushinteger(L, statbuf.st_gid);
		lua_setfield(L, -2, "st_gid");
		lua_pushinteger(L, statbuf.st_rdev);
		lua_setfield(L, -2, "st_rdev");
		lua_pushinteger(L, statbuf.st_size);
		lua_setfield(L, -2, "st_size");
		lua_pushinteger(L, statbuf.st_blksize);
		lua_setfield(L, -2, "st_blksize");
		lua_pushinteger(L, statbuf.st_blocks);
		lua_setfield(L, -2, "st_blocks");
		lua_pushinteger(L, statbuf.st_atime);
		lua_setfield(L, -2, "st_atime");
		lua_pushinteger(L, statbuf.st_mtime);
		lua_setfield(L, -2, "st_mtime");
		lua_pushinteger(L, statbuf.st_ctime);
		lua_setfield(L, -2, "st_ctime");
	}
	return 1;
}

static int
unix_mkstemp(lua_State *L)
{
	int fd;
	char *tmpnam;

	tmpnam = strdup(luaL_checkstring(L, 1));

	fd = mkstemp(tmpnam);
	if (fd == -1) {
		lua_pushnil(L);
		lua_pushnil(L);
	} else {
		lua_pushinteger(L, fd);
		lua_pushstring(L, tmpnam);
	}
	free(tmpnam);
	return 2;
}

static int
unix_ftruncate(lua_State *L)
{
	lua_pushboolean(L,
	    ftruncate(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2))
	    ? 0 : 1);
	return 1;
}

static int
unix_crypt(lua_State *L)
{
	lua_pushstring(L, crypt(luaL_checkstring(L, 1),
	    luaL_checkstring(L, 2)));
	return 1;
}

static int
unix_signal(lua_State *L)
{
	sighandler_t old, new;

	new = (sighandler_t)lua_tocfunction(L, 2);
	old = signal(luaL_checkinteger(L, 1), new);

	lua_pushcfunction(L, (lua_CFunction)old);
	return 1;
}

static int
unix_gethostname(lua_State *L)
{
	char name[128];

	if (!gethostname(name, sizeof name))
		lua_pushstring(L, name);
	else
		lua_pushnil(L);
	return 1;
}

static int
unix_sethostname(lua_State *L)
{
	const char *name;
	size_t len;

	name = luaL_checklstring(L, 1, &len);
	if (sethostname(name, len))
		lua_pushnil(L);
	else
		lua_pushboolean(L, 1);
	return 1;
}

static int
unix_openlog(lua_State *L)
{
	const char *ident;
	int option;
	int facility;

	ident = luaL_checkstring(L, 1);
	option = luaL_checkinteger(L, 2);
	facility = luaL_checkinteger(L, 3);
	openlog(ident, option, facility);
	return 0;
}

static int
unix_syslog(lua_State *L)
{
	syslog(luaL_checkinteger(L, 1), "%s", luaL_checkstring(L, 2));
	return 0;
}

static int
unix_closelog(lua_State *L)
{
	closelog();
	return 0;
}

static int
unix_setlogmask(lua_State *L)
{
	lua_pushinteger(L, setlogmask(luaL_checkinteger(L, 1)));
	return 1;
}

static void
unix_set_info(lua_State *L)
{
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2012 - 2018 by "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "Unix binding for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "unix 1.3.1");
	lua_settable(L, -3);
}

static struct constant unix_constant[] = {
	/* file modes */
	CONSTANT(S_IRUSR),
	CONSTANT(S_IWUSR),
	CONSTANT(S_IXUSR),
	CONSTANT(S_IRGRP),
	CONSTANT(S_IWGRP),
	CONSTANT(S_IXGRP),
	CONSTANT(S_IROTH),
	CONSTANT(S_IWOTH),
	CONSTANT(S_IXOTH),

	/* signals */
	CONSTANT(SIGHUP),
	CONSTANT(SIGINT),
	CONSTANT(SIGQUIT),
	CONSTANT(SIGILL),
	CONSTANT(SIGTRAP),
	CONSTANT(SIGABRT),
	CONSTANT(SIGIOT),
	CONSTANT(SIGBUS),
	CONSTANT(SIGFPE),
	CONSTANT(SIGKILL),
	CONSTANT(SIGUSR1),
	CONSTANT(SIGSEGV),
	CONSTANT(SIGUSR2),
	CONSTANT(SIGPIPE),
	CONSTANT(SIGALRM),
	CONSTANT(SIGTERM),
#ifdef __linux__
	CONSTANT(SIGSTKFLT),
#endif
	CONSTANT(SIGCHLD),
	CONSTANT(SIGCONT),
	CONSTANT(SIGSTOP),
	CONSTANT(SIGTSTP),
	CONSTANT(SIGTTIN),
	CONSTANT(SIGTTOU),
	CONSTANT(SIGURG),
	CONSTANT(SIGXCPU),
	CONSTANT(SIGXFSZ),
	CONSTANT(SIGVTALRM),
	CONSTANT(SIGPROF),
	CONSTANT(SIGWINCH),
#ifdef __linux__
	CONSTANT(SIGPOLL),
#endif
	CONSTANT(SIGIO),
	CONSTANT(SIGPWR),
	CONSTANT(SIGSYS),

	/* syslog options */
	CONSTANT(LOG_CONS),
	CONSTANT(LOG_NDELAY),
	CONSTANT(LOG_NOWAIT),
	CONSTANT(LOG_ODELAY),
	CONSTANT(LOG_PERROR),
	CONSTANT(LOG_PID),

	/* syslog facilities */
	CONSTANT(LOG_AUTH),
	CONSTANT(LOG_AUTHPRIV),
	CONSTANT(LOG_CRON),
	CONSTANT(LOG_DAEMON),
	CONSTANT(LOG_FTP),
	CONSTANT(LOG_KERN),
	CONSTANT(LOG_LOCAL0),
	CONSTANT(LOG_LOCAL1),
	CONSTANT(LOG_LOCAL2),
	CONSTANT(LOG_LOCAL3),
	CONSTANT(LOG_LOCAL4),
	CONSTANT(LOG_LOCAL5),
	CONSTANT(LOG_LOCAL6),
	CONSTANT(LOG_LOCAL7),
	CONSTANT(LOG_LPR),
	CONSTANT(LOG_MAIL),
	CONSTANT(LOG_NEWS),
	CONSTANT(LOG_SYSLOG),
	CONSTANT(LOG_USER),
	CONSTANT(LOG_UUCP),

	/* syslog levels */
	CONSTANT(LOG_EMERG),
	CONSTANT(LOG_ALERT),
	CONSTANT(LOG_CRIT),
	CONSTANT(LOG_ERR),
	CONSTANT(LOG_WARNING),
	CONSTANT(LOG_NOTICE),
	CONSTANT(LOG_INFO),
	CONSTANT(LOG_DEBUG),

	/* dlopen flags */
	CONSTANT(RTLD_LAZY),
	CONSTANT(RTLD_NOW),
	CONSTANT(RTLD_GLOBAL),
	CONSTANT(RTLD_LOCAL),
	CONSTANT(RTLD_NODELETE),
	CONSTANT(RTLD_NOLOAD),
#ifdef __GLIBC__
	CONSTANT(RTLD_DEEPBIND),
#endif
	{ NULL, 0 }
};

int
luaopen_unix(lua_State *L)
{
	int n;
	struct luaL_Reg luaunix[] = {
		{ "arc4random",	unix_arc4random },
		{ "chdir",	unix_chdir },
		{ "dup2",	unix_dup2 },
		{ "errno",	unix_errno },
		{ "fork",	unix_fork },
		{ "kill",	unix_kill },
		{ "getcwd",	unix_getcwd },
		{ "getpass",	unix_getpass },
		{ "getpid",	unix_getpid },
		{ "setpgid",	unix_setpgid },
		{ "sleep",	unix_sleep },
		{ "unlink",	unix_unlink },
		{ "getuid",	unix_getuid },
		{ "getgid",	unix_getgid },
		{ "chown",	unix_chown },
		{ "chmod",	unix_chmod },
		{ "rename",	unix_rename },
		{ "stat",	unix_stat },
		{ "mkstemp",	unix_mkstemp },
		{ "ftruncate",	unix_ftruncate },

		/* crypt */
		{ "crypt",	unix_crypt },

		/* signals */
		{ "signal",	unix_signal },

		{ "setpwent",	unix_setpwent },
		{ "endpwent",	unix_endpwent },
		{ "getpwent",	unix_getpwent },
		{ "getpwnam",	unix_getpwnam },
		{ "getpwuid",	unix_getpwuid },

		/* dirent */
		{ "opendir",	unix_opendir },

		/* dynamic linker */
		{ "dlopen",	unix_dlopen },
		{ "dlerror",	unix_dlerror },
		{ "dlsym",	unix_dlsym },
		{ "dlclose",	unix_dlclose },

#ifdef __linux__
		/* shadow password */
		{ "getspnam",	unix_getspnam },
#endif

		{ "getgrnam",	unix_getgrnam },
		{ "getgrgid",	unix_getgrgid },

		/* hostname */
		{ "gethostname",	unix_gethostname },
		{ "sethostname",	unix_sethostname },

		/* syslog */
		{ "openlog",	unix_openlog },
		{ "syslog",	unix_syslog },
		{ "closelog",	unix_closelog },
		{ "setlogmask",	unix_setlogmask },

		/* select */
		{ "select",	unix_select },
		{ "fd_set",	unix_fd_set },
		{ NULL, NULL }
	};
	struct luaL_Reg fd_set_methods[] = {
		{ "clr",	unix_fd_set_clr },
		{ "isset",	unix_fd_set_isset },
		{ "set",	unix_fd_set_set },
		{ "zero",	unix_fd_set_zero },
		{ NULL,		NULL }
	};
	struct luaL_Reg dir_methods[] = {
		{ "__gc",	unix_closedir },
		{ "readdir",	unix_readdir },
		{ "telldir",	unix_telldir },
		{ "seekdir",	unix_seekdir },
		{ "rewinddir",	unix_rewinddir },
		{ "closedir",	unix_closedir },
		{ NULL,		NULL }
	};
	struct luaL_Reg dl_methods[] = {
		{ "__gc",	unix_dlclose },
		{ "__index",	unix_dlsym },
		{ NULL,		NULL }
	};
	if (luaL_newmetatable(L, FD_SET_METATABLE)) {
		luaL_setfuncs(L, fd_set_methods, 0);
#if 0
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, fd_set_clear);
		lua_settable(L, -3);
#endif
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	if (luaL_newmetatable(L, DIR_METATABLE)) {
		luaL_setfuncs(L, dir_methods, 0);

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);
	if (luaL_newmetatable(L, DL_METATABLE)) {
		luaL_setfuncs(L, dl_methods, 0);

#if 0
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);
#endif

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);
	if (luaL_newmetatable(L, DLSYM_METATABLE)) {
		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	luaL_newlib(L, luaunix);
	unix_set_info(L);
	for (n = 0; unix_constant[n].name != NULL; n++) {
		lua_pushinteger(L, unix_constant[n].value);
		lua_setfield(L, -2, unix_constant[n].name);
	};
	lua_pushcfunction(L, (lua_CFunction)SIG_IGN);
	lua_setfield(L, -2, "SIG_IGN");
	lua_pushcfunction(L, (lua_CFunction)SIG_DFL);
	lua_setfield(L, -2, "SIG_DFL");
	lua_pushcfunction(L, (lua_CFunction)reaper);
	lua_setfield(L, -2, "SIG_REAPER");
	return 1;
}
