-- Tests for unix process management functions
local unix = require("cosmo.unix")

local function test_function_exists(name)
  assert(type(unix[name]) == "function", name .. " should be a function")
end

-- Test all new functions exist
test_function_exists("daemon")
test_function_exists("execvp")
test_function_exists("execvpe")
test_function_exists("fexecve")
test_function_exists("spawn")
test_function_exists("spawnp")
test_function_exists("nice")
test_function_exists("getpriority")
test_function_exists("setpriority")
test_function_exists("killpg")

-- Test constants exist
assert(type(unix.PRIO_PROCESS) == "number", "PRIO_PROCESS should be a number")
assert(type(unix.PRIO_PGRP) == "number", "PRIO_PGRP should be a number")
assert(type(unix.PRIO_USER) == "number", "PRIO_USER should be a number")

-- Test getpriority/setpriority
local prio, err = unix.getpriority(unix.PRIO_PROCESS, 0)
assert(prio ~= nil, "getpriority should return priority: " .. tostring(err))
assert(type(prio) == "number", "priority should be a number")
assert(prio >= -20 and prio <= 19, "priority should be in range -20 to 19")

-- Test nice (increase niceness, which always succeeds for unprivileged users)
local new_prio, err = unix.nice(0)
assert(new_prio ~= nil, "nice(0) should succeed: " .. tostring(err))
assert(type(new_prio) == "number", "nice should return a number")

-- Test spawn with /bin/true (or echo)
local true_path = unix.commandv("true")
if true_path then
  local pid, err = unix.spawn(true_path, {true_path})
  assert(pid ~= nil, "spawn should return pid: " .. tostring(err))
  assert(type(pid) == "number", "pid should be a number")
  assert(pid > 0, "pid should be positive")
  -- Wait for child to complete
  local wpid, wstatus = unix.wait(pid)
  assert(wpid == pid, "wait should return the spawned pid")
  assert(unix.WIFEXITED(wstatus), "child should exit normally")
  assert(unix.WEXITSTATUS(wstatus) == 0, "child should exit with 0")
end

-- Test spawnp (uses PATH search)
local pid, err = unix.spawnp("true", {"true"})
if pid then
  assert(type(pid) == "number", "pid should be a number")
  local wpid, wstatus = unix.wait(pid)
  assert(wpid == pid, "wait should return the spawned pid")
  assert(unix.WIFEXITED(wstatus), "child should exit normally")
end

-- Test spawn with arguments
local echo_path = unix.commandv("echo")
if echo_path then
  local pid, err = unix.spawn(echo_path, {echo_path, "hello", "world"})
  assert(pid ~= nil, "spawn with args should return pid: " .. tostring(err))
  local wpid, wstatus = unix.wait(pid)
  assert(unix.WIFEXITED(wstatus), "echo should exit normally")
  assert(unix.WEXITSTATUS(wstatus) == 0, "echo should exit with 0")
end

-- Test spawn with custom environment
local env_path = unix.commandv("env")
if env_path then
  local pid, err = unix.spawn(env_path, {env_path}, {"TEST_VAR=hello"})
  assert(pid ~= nil, "spawn with env should return pid: " .. tostring(err))
  local wpid, wstatus = unix.wait(pid)
  assert(unix.WIFEXITED(wstatus), "env should exit normally")
end

-- Test killpg with invalid pgrp (should fail with proper error)
local ok, err = unix.killpg(-1, 0)
assert(ok == nil, "killpg(-1, 0) should fail")
assert(err ~= nil, "killpg should return an error object")

-- Test error cases
local pid, err = unix.spawn("/nonexistent/path", {"/nonexistent/path"})
assert(pid == nil, "spawn of nonexistent path should fail")
assert(err ~= nil, "spawn failure should return error")

local pid, err = unix.spawnp("nonexistent_command_12345", {"nonexistent_command_12345"})
assert(pid == nil, "spawnp of nonexistent command should fail")

-- Test execvp error case (won't replace process since it fails)
local ok, err = unix.execvp("nonexistent_command_12345")
assert(ok == nil, "execvp of nonexistent command should fail")
assert(err ~= nil, "execvp failure should return error")

print("all unix proc tests passed")
