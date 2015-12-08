/*
 * Copyright (c) 2011 - 2015, Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
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

#include "select.h"

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
		lua_setfield(L, -2, "st_uid");
		lua_pushinteger(L, statbuf.st_mode);
		lua_setfield(L, -2, "st_uid");
		lua_pushinteger(L, statbuf.st_nlink);
		lua_setfield(L, -2, "st_uid");
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
unix_setpwent(lua_State *L)
{
	setpwent();
	return 0;
}

static int
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

static int
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

static int
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

static int
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

static int
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

static int
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

static int
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
	lua_pushliteral(L, "Copyright (C) 2012 - 2015 by "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "Unix binding for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "unix 1.2.6");
	lua_settable(L, -3);
}

struct constant {
	char *name;
	int value;
};

#define CONSTANT(NAME)		{ #NAME, NAME }

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

		/* crypt */
		{ "crypt",	unix_crypt },

		/* signals */
		{ "signal",	unix_signal },

		{ "setpwent",	unix_setpwent },
		{ "endpwent",	unix_endpwent },
		{ "getpwent",	unix_getpwent },
		{ "getpwnam",	unix_getpwnam },
		{ "getpwuid",	unix_getpwuid },
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

	if (luaL_newmetatable(L, FD_SET_METATABLE)) {
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(L, fd_set_methods, 0);
#else
		luaL_register(L, NULL, fd_set_methods);
#endif
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

#if LUA_VERSION_NUM >= 502
	luaL_newlib(L, luaunix);
#else
	luaL_register(L, "unix", luaunix);
#endif
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
