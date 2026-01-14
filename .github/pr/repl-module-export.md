# lua: expose interactive REPL as cosmo.repl module

Exposes the enhanced interactive REPL as a module that can be required and invoked programmatically via `require("cosmo.repl").start()`.

## Changes

- `third_party/lua/lrepl.c` - added shared `lua_doREPL()` function with REPL loop logic
- `third_party/lua/lrepl.h` - declared `lua_doREPL()`
- `third_party/lua/lreplmod.c` - thin wrapper calling `lua_doREPL()`
- `third_party/lua/lua.main.c` - use shared `lua_doREPL()` instead of duplicated code
- `third_party/lua/BUILD.mk` - added new files to build
- `tool/lua/lcosmo.c` - register `cosmo.repl` submodule
- `tool/lua/cosmo/repl/init.lua` - LuaLS type annotations
- `tool/lua/cosmo/help/init.lua` - added to help system
- `test_repl.lua` - basic module loading test

## Refactoring note

The REPL loop was previously duplicated between `lua.main.c` and `lreplmod.c`. This has been refactored to use a single shared implementation in `lrepl.c`, reducing code duplication by ~50 lines.
