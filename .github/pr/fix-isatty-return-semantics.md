# lua: fix unix.isatty() return semantics

Fixes a bug in `unix.isatty()` where it incorrectly returned `true` for non-tty file descriptors and generated warnings when checking actual ttys.

## Changes

- `third_party/lua/lunix.c` - rewrote `LuaUnixIsatty()` to handle isatty()'s tri-state return (1/0/-1) instead of using `SysretBool()` which expects binary returns (0/-1)
- `tool/lua/test_isatty.lua` - added test demonstrating the fix
- `tool/lua/BUILD.mk` - registered new test in build system

The `isatty()` syscall returns 1 for tty, 0 for non-tty, and -1 for error. The previous implementation used `SysretBool()` which expects 0 for success and -1 for error, causing two bugs:
1. Warning "syscall supposed to return 0 / -1 but got 1" when fd was a tty
2. Returned `true` when `isatty()` returned 0 (non-tty), should return `false`

The fix properly handles all three return states and includes tests to prevent regression.
