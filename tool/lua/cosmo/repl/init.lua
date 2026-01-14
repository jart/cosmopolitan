---@meta cosmo.repl
error("Tried to evaluate definition file.")

---Interactive REPL (Read-Eval-Print Loop) module for Lua.
---
---This module exposes the enhanced interactive REPL that was previously only
---available when running `lua` without arguments. The REPL includes:
---- Tab completion for globals, table members, and keywords
---- Syntax hints while typing
---- Persistent command history
---- Automatic expression evaluation (prefix with `=` or just type an expression)
---- Multi-line editing for incomplete statements
---
---Example usage:
---```lua
---local repl = require("cosmo.repl")
---repl.start()  -- Starts the interactive REPL
---```
---
---The REPL will continue until the user presses Ctrl-D (EOF) or Ctrl-C twice.
---All global variables and functions are available in the REPL context.
---
---@class cosmo.repl
local repl = {}

---Start the interactive REPL.
---
---Enters an interactive Read-Eval-Print Loop with the following features:
---- Tab completion for global variables, table members (e.g., `math.<tab>`), and Lua keywords
---- Inline hints showing available completions
---- Command history saved to ~/.lua_history (or similar platform-specific location)
---- Automatic `return` prepending for expressions (e.g., typing `2+2` shows `4`)
---- Multi-line editing for incomplete statements
---- Proper signal handling (Ctrl-C interrupts current operation, Ctrl-D exits)
---
---The REPL respects custom `_PROMPT` and `_PROMPT2` global variables if defined.
---
---Example:
---```lua
---local repl = require("cosmo.repl")
---
---- Start an interactive session
---repl.start()
---
---- You can also customize the prompt before starting:
---_PROMPT = ">> "
---_PROMPT2 = "... "
---repl.start()
---```
---
---@return nil
function repl.start() end

return repl
