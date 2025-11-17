/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "libc/nt/shell32.h"
#include "libc/nt/enum/knownfolder.h"
#include "libc/nt/enum/e.h"
#include "libc/str/str.h"
#include "third_party/miniaudio/miniaudio.h"
#include "third_party/musl/passwd.h"
__static_yoink("musl_libc_notice");

#ifdef FTRACE
// if the default mode debugging tools are enabled, and we're linking
// something as substantive as this library, then we shall assume the
// application is meaty enough to benefit from the performance of the
// chromium zlib library (costs ~40kb binary) versus just having puff
__static_yoink("_Cz_inflateInit2");
__static_yoink("_Cz_inflate");
__static_yoink("_Cz_inflateEnd");
#endif

// stores home directory without trailing slash
// returns strlen(path) on success or -1 on error/truncation
static int
GetUserHomeDirectory(char *path, size_t size)
{
	int i, len;
	int32_t hResult;
	char16_t *path16;
	if (size < 8)
		return -1;
	hResult = SHGetKnownFolderPath(&FOLDERID_Profile, 0, 0, &path16);
	if (!SUCCEEDED(hResult))
		return -1;
	path16[1] = path16[0];
	path16[0] = '/';
	for (i = 0; path16[i]; ++i)
		if (path16[i] == '\\')
			path16[i] = '/';
	len = tprecode16to8(path, size, path16).ax;
	if (len >= size - 1)
		len = -1;
	CoTaskMemFree(path16);
	return len;
}

static char *
__create_synthetic_passwd_file(void)
{
	int uid, gid;
	char homebuf[PATH_MAX];
	char login[256], cwd[PATH_MAX];
	char *user, *home, *shell, *res = 0;
	uid = getuid();
	gid = getgid();
	user = getenv("USER");
	home = getenv("HOME");
	shell = getenv("SHELL");
	if (user && strchr(user, ':'))
		user = 0;
	if (home && strchr(home, ':'))
		user = 0;
	if (shell && strchr(shell, ':'))
		user = 0;
	if (!shell)
		shell = _PATH_BSHELL;
	if (!user && getlogin_r(login, sizeof(login)) != -1)
		user = login;
	if (IsWindows() && GetUserHomeDirectory(homebuf, sizeof(homebuf)) > 0) {
		home = homebuf;
	} else if (!home && getcwd(cwd, sizeof(cwd))) {
		if (!strchr(cwd, ':'))
			home = cwd;
		else
			home = "/";
	}
	if (uid)
		appendf(&res, "root:x:0:0:root:/root:%s\n", shell);
	if (user && home) {
		appendf(&res, "%s:x:%d:%d:%s:%s:%s\n",
			user, uid, gid, user, home, shell);
	}
	return res;
}

static FILE *
__fopen_passwd(void)
{
	FILE *f;
	char *s;
	// MacOS has a fake /etc/passwd file without any user details
	// GetFileAttributes(u"\\etc\\passwd") takes 2 seconds sometimes
	if (!IsXnu() && !IsWindows() && (f = fopen("/etc/passwd", "rbe")))
		return f;
	if (!(s = __create_synthetic_passwd_file()))
		return 0;
	if (!(f = fmemopen(s, strlen(s), "rbe")))
		free(s);
	return f;
}

static unsigned
atou(char **s)
{
	unsigned x;
	for (x = 0; **s - '0' < 10U; ++*s) {
		x = 10 * x + (**s - '0');
	}
	return x;
}

int
__getpwent_a(FILE *f, struct passwd *pw, char **line, size_t *size,
	     struct passwd **res)
{
	ssize_t l;
	char *s;
	int rv = 0;
	int cs;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	for (;;) {
		if ((l = getline(line, size, f)) < 0) {
			rv = ferror(f) ? errno : 0;
			free(*line);
			*line = 0;
			pw = 0;
			break;
		}
		line[0][l - 1] = 0;
		s = line[0];
		pw->pw_name = s++;
		if (!(s = strchr(s, ':'))) continue;
		*s++ = 0;
		pw->pw_passwd = s;
		if (!(s = strchr(s, ':'))) continue;
		*s++ = 0;
		pw->pw_uid = atou(&s);
		if (*s != ':') continue;
		*s++ = 0;
		pw->pw_gid = atou(&s);
		if (*s != ':') continue;
		*s++ = 0;
		pw->pw_gecos = s;
		if (!(s = strchr(s, ':'))) continue;
		*s++ = 0;
		pw->pw_dir = s;
		if (!(s = strchr(s, ':'))) continue;
		*s++ = 0;
		pw->pw_shell = s;
		break;
	}
	pthread_setcancelstate(cs, 0);
	*res = pw;
	if (rv) errno = rv;
	return rv;
}

int
__getpw_a(const char *name, uid_t uid, struct passwd *pw, char **buf,
	  size_t *size, struct passwd **res)
{
	FILE *f;
	int cs;
	int rv = 0;
	*res = 0;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	if ((f = __fopen_passwd())) {
		while (!(rv = __getpwent_a(f, pw, buf, size, res)) && *res) {
			if ((name && !strcmp(name, (*res)->pw_name)) ||
			    (!name && (*res)->pw_uid == uid)) {
				break;
			}
		}
		fclose(f);
	}
	pthread_setcancelstate(cs, 0);
	if (rv) errno = rv;
	return rv;
}

static int
getpw_r(const char *name, uid_t uid, struct passwd *pw, char *buf,
	size_t size, struct passwd **res)
{
#define FIX(x) (pw->pw_##x = pw->pw_##x - line + buf)
	char *line = 0;
	size_t len = 0;
	int rv = 0;
	int cs;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	rv = __getpw_a(name, uid, pw, &line, &len, res);
	if (*res && size < len) {
		*res = 0;
		rv = ERANGE;
	}
	if (*res) {
		memcpy(buf, line, len);
		FIX(name);
		FIX(passwd);
		FIX(gecos);
		FIX(dir);
		FIX(shell);
	}
	free(line);
	pthread_setcancelstate(cs, 0);
	if (rv) errno = rv;
	return rv;
#undef FIX
}

int
getpwnam_r(const char *name, struct passwd *pw, char *buf, size_t size,
	   struct passwd **res)
{
	return getpw_r(name, 0, pw, buf, size, res);
}

int
getpwuid_r(uid_t uid, struct passwd *pw, char *buf, size_t size,
	   struct passwd **res)
{
	return getpw_r(0, uid, pw, buf, size, res);
}

static struct GetpwentState {
	FILE *f;
	char *line;
	struct passwd pw;
	size_t size;
} g_getpwent;

/* [jart] free memory at exit */
static void pwd_atexit(void) {
	if (g_getpwent.f)
		fclose(g_getpwent.f);
	free(g_getpwent.line);
	memset(&g_getpwent, 0, sizeof(g_getpwent));
}
__attribute__((__constructor__)) static void pwd_ctor(void) {
	atexit(pwd_atexit);
}

/**
 * Closes global handle to password database.
 *
 * @see getpwent()
 * @threadunsafe
 */
void
endpwent(void)
{
	setpwent();
}

/**
 * Rewinds global handle to password database.
 *
 * @see getpwent()
 * @threadunsafe
 */
void
setpwent(void)
{
	if (g_getpwent.f)
		fclose(g_getpwent.f);
	g_getpwent.f = 0;
}

/**
 * Returns next entry in password database.
 *
 * @return pointer to entry static memory, or NULL on EOF
 * @see setpwent()
 * @threadunsafe
 */
struct passwd *
getpwent()
{
	struct passwd *res;
	if (!g_getpwent.f)
		g_getpwent.f = __fopen_passwd();
	if (!g_getpwent.f)
		return 0;
	__getpwent_a(g_getpwent.f, &g_getpwent.pw, &g_getpwent.line,
		     &g_getpwent.size, &res);
	return res;
}

/**
 * Returns password database entry for user id.
 *
 * This is essentially guaranteed to succeed if `uid == getuid()`, since
 * this implementation will generate an entry based on the environment
 * if `/etc/passwd` doesn't exist, or is fake (e.g. MacOS).
 *
 * @return pointer to passwd entry static memory, or NULL if not found
 * @threadunsafe
 */
struct passwd *
getpwuid(uid_t uid)
{
	struct passwd *res;
	__getpw_a(0, uid, &g_getpwent.pw, &g_getpwent.line, &g_getpwent.size,
		  &res);
	return res;
}

/**
 * Returns password database entry for user name.
 *
 * This is essentially guaranteed to succeed if `uid == getenv("USER")`,
 * since this implementation will generate an entry based on `environ`
 * if `/etc/passwd` doesn't exist, or is fake (e.g. MacOS).
 *
 * @return pointer to passwd entry static memory, or NULL if not found
 * @threadunsafe
 */
struct passwd *
getpwnam(const char *name)
{
	struct passwd *res;
	__getpw_a(name, 0, &g_getpwent.pw, &g_getpwent.line,
		  &g_getpwent.size, &res);
	return res;
}
