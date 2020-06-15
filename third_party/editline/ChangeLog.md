Change Log
==========

All notable changes to the project are documented in this file.


[1.16.1] - 2019-06-07
---------------------

### Changes
- Major updates to the `editline.3` man page
- Cleanup of examples `cli.c` and `fileman.c`
- Add example of hidden input prompt to `cli.c`

### Fixes
- Fix #20: `configure --disable-eof` does not bite
- Fix #23: Make Ctrl-L clear the screan instead of starting a new line  
  Like Ctrl-D, which exits, Ctrl-L only clears the screen when the line
  is empty and the cursor is at the start of the line, otherwise Ctrl-L
  will redraw/refresh the current line.
- Fix #24: Fix behavior when TTY is narrower than column width, by Will Dietz
- Fix #25: Avoid continuously duplicate commands in history
- Fix #31: Aborting i-search with Ctrl-C should not generate signal


[1.16.0][] - 2018-09-16
-----------------------

Event loop callback support.

### Changes
- `rl_unintialize()`, new function to free all memory, by Claus Fischer
- `rl_insert_text()`, new GNU Readline compat function
- `rl_refresh_line()`, new GNU Readline compat function
- `rl_callback_*()`, alternate interface to plain `readline()` for event
  loops.  Modeled after the GNU Readline API
- `rl_completion_entry_function`, and `rl_attempted_completion_function`
  are two new GNU Readline compat user hooks for the completion framework
- `rl_completion_matches()` and `rl_filename_completion_function()`
  are two new GNU Readline compat functions
- Add new example: `fileman.c` from GNU Readline to demonstrate the
  level of compatibility of the revamped completion framework
- Add support for Ctrl-Right and Ctrl-Left, forward/backward word
- Add .deb package to official release target

### Fixes
- Fixed header guards, avoid using leading `__`
- Spell check fixes
- Remove duplicate code in history check
- Use `NULL` instead of `0`, and `-1` instead of `NULL`, where applicable
- Misc. minor Coverity Scan fixes
- Misc. minor fixes to `testit.c` example code
- Add `-Wextra` to std `CFLAGS`
- Check `fclose()` return value in in `write_history()` and `read_history()`
- Initialize global variables and reset to `NULL` on `free()`
- Fix off-by-one in forward kill word, avoid deleting too much
- Skip (or kill) leading whitespace when skipping (or killing) forwards


[1.15.3][] - 2017-09-07
-----------------------

Bug fix release.

### Changes
- Refactor all enable/disable configure options, same problem as in #7

### Fixes
- Fix #7: `--enable-termcap` configure option does not work.  The script
  enabled termcap by default rather than the other way around.
  
  Also, check for terminfo as well, when `--enable-termcap` is selected.


[1.15.2][] - 2016-06-06
-----------------------

Bug fixes and minor feature creep in `pkg-config` support.

### Changes
- Prevent mangling of symbols when linking with C++.  Patch courtesy of
  Jakub Pawlowski
- Add `libeditline.pc` for `pkg-config`

### Fixes
- Do not assume a termcap library exists, check for `tgetent()` in
  curses, ncurses, tinfo and termcap libraries
- Call `tty_flush()` when user calls `rl_forced_update_display()`
  to prevent screen becoming garbled.  Patch by Jakub Pawlowski


[1.15.1][] - 2015-11-16
-----------------------

Bug fixes only.

### Changes
- Update README with origin of this version of editline

### Fixes
- Fix build system, don't force automake v1.11, require at least v1.11
- Fix build problem with examples using `--enable-termcap`


[1.15.0][] - 2015-09-10
-----------------------

### Changes
- Add support for `--disable-eof` and `--disable-sigint` to disable
  default Ctrl-D and Ctrl-C behavior
- Add support for `el_no_hist` to disable access to and auto-save of history
- GNU readline compat functions for prompt handling and redisplay
- Refactor: replace variables named 'new' with non-reserved word
- Add support for [Travis-CI][], continuous integration with GitHub
- Add support for [Coverity Scan][], the best static code analyzer,
  integrated with [Travis-CI][] -- scan runs for each push to master
- Rename NEWS.md --> ChangeLog.md, with symlinks for <kbd>make install</kbd>
- Attempt to align with http://keepachangelog.com/ for this file
- Cleanup and improve Markdown syntax in [README.md][]
- Add API and example to [README.md][], inspired by [libuEv][]
- Removed generated files from version control.  Use `./autogen.sh`
  to generate the `configure` script when working from GIT.  This
  does not affect distributed tarballs

### Fixes
- Fix issue #2, regression in Ctrl-D (EOF) behavior.  Regression
  introduced in [1.14.1][].  Fixed by @TobyGoodwin
- Fix memory leak in completion handler.  Found by [Coverity Scan][].
- Fix suspicious use of `sizeof(char **)`, same as `sizeof(char *)` but
  non-portable.  Found by [Coverity Scan][]
- Fix out-of-bounds access in user key binding routines
  Found by [Coverity Scan][].
- Fix invisible example code in man page


[1.14.2][] - 2014-09-14
-----------------------

Bug fixes only.

### Fixes
  - Fix `el_no_echo` bug causing secrets to leak when disabling no-echo
  - Handle `EINTR` in syscalls better


[1.14.1][] - 2014-09-14
-----------------------

Minor fixes and additions.

### Changes
- Don't print status message on `stderr` in key binding funcions
- Export `el_del_char()`
- Check for and return pending signals when detected
- Allow custom key bindings ...

### Fixes
- Bug fixes ...


[1.14.0][] - 2010-08-10
-----------------------

Major cleanups and further merges with Debian editline package.

### Changes
- Merge in changes to `debian/` from `editline_1.12-6.debian.tar.gz`
- Migrate to use libtool
- Make `UNIQUE_HISTORY` configurable
- Make scrollback history (`HIST_SIZE`) configurable
- Configure options for toggling terminal bell and `SIGSTOP` (Ctrl-Z)
- Configure option for using termcap to read/control terminal size
- Rename Signal to `el_intr_pending`, from Festival speech-tools
- Merge support for capitalizing words (`M-c`) from Festival
  speech-tools by Alan W Black <mailto:awb()cstr!ed!ac!uk>
- Fallback backspace handling, in case `tgetstr("le")` fails

### Fixes
- Cleanups and fixes thanks to the Sparse static code analysis tool
- Merge `el_no_echo` patch from Festival speech-tools
- Merge fixes from Heimdal project
- Completely refactor `rl_complete()` and `rl_list_possib()` with
  fixes from the Heimdal project.  Use `rl_set_complete_func()` and
  `rl_set_list_possib_func()`.  Default completion callbacks are now
  available as a configure option `--enable-default-complete`
- Memory leak fixes
- Actually fix 8-bit handling by reverting old Debian patch
- Merge patch to improve compatibility with GNU readline, thanks to
  Steve Tell from way back in 1997 and 1998


[1.13.0][] - 2010-03-09
-----------------------

Adaptations to Debian editline package.

### Changes
- Major version number bump, adapt to Jim Studt's v1.12
- Import `debian/` directory and adapt it to configure et al.
- Change library name to libeditline to distinguish it from BSD libedit


[0.3.0][] - 2009-02-08
----------------------

### Changes
- Support for ANSI arrow keys using <kbd>configure --enable-arrow-keys</kbd>


[0.2.3][] - 2008-12-02
----------------------

### Changes
- Patches from Debian package merged
- Support for custom command completion


[0.1.0][] - 2008-06-07
----------------------

### Changes
- First version, forked from Minix current 2008-06-06


[UNRELEASED]:    https://github.com/troglobit/finit/compare/1.16.0...HEAD
[1.16.1]:        https://github.com/troglobit/finit/compare/1.16.0...1.16.1
[1.16.0]:        https://github.com/troglobit/finit/compare/1.15.3...1.16.0
[1.15.3]:        https://github.com/troglobit/finit/compare/1.15.2...1.15.3
[1.15.2]:        https://github.com/troglobit/finit/compare/1.15.1...1.15.2
[1.15.1]:        https://github.com/troglobit/finit/compare/1.15.0...1.15.1
[1.15.0]:        https://github.com/troglobit/finit/compare/1.14.2...1.15.0
[1.14.2]:        https://github.com/troglobit/finit/compare/1.14.1...1.14.2
[1.14.1]:        https://github.com/troglobit/finit/compare/1.14.0...1.14.1
[1.14.0]:        https://github.com/troglobit/finit/compare/1.13.0...1.14.0
[1.13.0]:        https://github.com/troglobit/finit/compare/0.3.0...1.13.0
[0.3.0]:         https://github.com/troglobit/finit/compare/0.2.3...0.3.0
[0.2.3]:         https://github.com/troglobit/finit/compare/0.1.0...0.2.3
[0.1.0]:         https://github.com/troglobit/finit/compare/0.0.0...0.1.0
[libuEv]:        http://github.com/troglobit/libuev
[Travis-CI]:     https://travis-ci.org/troglobit/uftpd
[Coverity Scan]: https://scan.coverity.com/projects/2947
[README.md]:     https://github.com/troglobit/editline/blob/master/README.md

<!--
  -- Local Variables:
  -- mode: markdown
  -- End:
  -->
