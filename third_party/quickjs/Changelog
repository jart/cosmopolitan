2021-03-27:

- faster Array.prototype.push and Array.prototype.unshift
- added JS_UpdateStackTop()
- fixed Windows console
- misc bug fixes

2020-11-08:

- improved function parameter initializers
- added std.setenv(), std.unsetenv() and std.getenviron()
- added JS_EvalThis()
- misc bug fixes

2020-09-06:

- added logical assignment operators
- added IsHTMLDDA support
- faster for-of loops
- os.Worker now takes a module filename as parameter
- qjsc: added -D option to compile dynamically loaded modules or workers
- misc bug fixes

2020-07-05:

- modified JS_GetPrototype() to return a live value
- REPL: support unicode characters larger than 16 bits
- added os.Worker
- improved object serialization
- added std.parseExtJSON
- misc bug fixes

2020-04-12:

- added cross realm support
- added AggregateError and Promise.any
- added env, uid and gid options in os.exec()
- misc bug fixes

2020-03-16:

- reworked error handling in std and os libraries: suppressed I/O
  exceptions in std FILE functions and return a positive errno value
  when it is explicit
- output exception messages to stderr
- added std.loadFile(), std.strerror(), std.FILE.prototype.tello()
- added JS_GetRuntimeOpaque(), JS_SetRuntimeOpaque(), JS_NewUint32()
- updated to Unicode 13.0.0
- misc bug fixes

2020-01-19:

- keep CONFIG_BIGNUM in the makefile
- added os.chdir()
- qjs: added -I option
- more memory checks in the bignum operations
- modified operator overloading semantics to be closer to the TC39
  proposal
- suppressed "use bigint" mode. Simplified "use math" mode
- BigDecimal: changed suffix from 'd' to 'm'
- misc bug fixes

2020-01-05:

- always compile the bignum code. Added '--bignum' option to qjs.
- added BigDecimal
- added String.prototype.replaceAll
- misc bug fixes

2019-12-21:

- added nullish coalescing operator (ES2020)
- added optional chaining (ES2020)
- removed recursions in garbage collector
- test stack overflow in the parser
- improved backtrace logic
- added JS_SetHostPromiseRejectionTracker()
- allow exotic constructors
- improved c++ compatibility
- misc bug fixes

2019-10-27:

- added example of C class in a module (examples/test_point.js)
- added JS_GetTypedArrayBuffer()
- misc bug fixes

2019-09-18:

- added os.exec and other system calls
- exported JS_ValueToAtom()
- qjsc: added 'qjsc_' prefix to the generated C identifiers
- added cross-compilation support
- misc bug fixes

2019-09-01:

- added globalThis
- documented JS_EVAL_FLAG_COMPILE_ONLY
- added import.meta.url and import.meta.main
- added 'debugger' statement
- misc bug fixes

2019-08-18:

- added os.realpath, os.getcwd, os.mkdir, os.stat, os.lstat,
  os.readlink, os.readdir, os.utimes, std.popen
- module autodetection
- added import.meta
- misc bug fixes

2019-08-10:

- added public class fields and private class fields, methods and
  accessors (TC39 proposal)
- changed JS_ToCStringLen() prototype
- qjsc: handle '-' in module names and modules with the same filename
- added std.urlGet
- exported JS_GetOwnPropertyNames() and JS_GetOwnProperty()
- exported some bigint C functions
- added support for eshost in run-test262
- misc bug fixes

2019-07-28:

- added dynamic import
- added Promise.allSettled
- added String.prototype.matchAll
- added Object.fromEntries
- reduced number of ticks in await
- added BigInt support in Atomics
- exported JS_NewPromiseCapability()
- misc async function and async generator fixes
- enabled hashbang support by default

2019-07-21:

- updated test262 tests
- updated to Unicode version 12.1.0
- fixed missing Date object in qjsc
- fixed multi-context creation
- misc ES2020 related fixes
- simplified power and division operators in bignum extension
- fixed several crash conditions

2019-07-09:

- first public release
