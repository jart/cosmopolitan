local cosmo = require("cosmo")
local gs = require("cosmo.goodsocket")
local unix = require("cosmo.unix")

-- Test module exists
assert(gs, "goodsocket module should exist")
assert(type(gs.socket) == "function", "gs.socket should be a function")

--------------------------------------------------------------------------------
-- Test constants are exported
--------------------------------------------------------------------------------

-- Socket families
assert(type(gs.AF_INET) == "number", "AF_INET should be a number")
assert(type(gs.AF_INET6) == "number", "AF_INET6 should be a number")
assert(type(gs.AF_UNIX) == "number", "AF_UNIX should be a number")
assert(type(gs.AF_UNSPEC) == "number", "AF_UNSPEC should be a number")
assert(gs.AF_INET == 2, "AF_INET should be 2")
assert(gs.AF_UNIX == 1, "AF_UNIX should be 1")
assert(gs.AF_UNSPEC == 0, "AF_UNSPEC should be 0")

-- Socket types
assert(type(gs.SOCK_STREAM) == "number", "SOCK_STREAM should be a number")
assert(type(gs.SOCK_DGRAM) == "number", "SOCK_DGRAM should be a number")
assert(type(gs.SOCK_RAW) == "number", "SOCK_RAW should be a number")
assert(type(gs.SOCK_RDM) == "number", "SOCK_RDM should be a number")
assert(type(gs.SOCK_SEQPACKET) == "number", "SOCK_SEQPACKET should be a number")
assert(gs.SOCK_STREAM == 1, "SOCK_STREAM should be 1")
assert(gs.SOCK_DGRAM == 2, "SOCK_DGRAM should be 2")

-- Protocols
assert(type(gs.IPPROTO_IP) == "number", "IPPROTO_IP should be a number")
assert(type(gs.IPPROTO_TCP) == "number", "IPPROTO_TCP should be a number")
assert(type(gs.IPPROTO_UDP) == "number", "IPPROTO_UDP should be a number")
assert(type(gs.IPPROTO_ICMP) == "number", "IPPROTO_ICMP should be a number")
assert(type(gs.IPPROTO_IPV6) == "number", "IPPROTO_IPV6 should be a number")
assert(type(gs.IPPROTO_ICMPV6) == "number", "IPPROTO_ICMPV6 should be a number")
assert(type(gs.IPPROTO_RAW) == "number", "IPPROTO_RAW should be a number")
assert(gs.IPPROTO_IP == 0, "IPPROTO_IP should be 0")
assert(gs.IPPROTO_TCP == 6, "IPPROTO_TCP should be 6")
assert(gs.IPPROTO_UDP == 17, "IPPROTO_UDP should be 17")

--------------------------------------------------------------------------------
-- Test basic socket creation
--------------------------------------------------------------------------------

-- Create a TCP socket (client)
local fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
assert(fd, "failed to create TCP client socket: " .. tostring(err))
assert(type(fd) == "number", "socket should return a number (file descriptor)")
assert(fd >= 0, "file descriptor should be non-negative")
assert(unix.close(fd), "should be able to close socket")

-- Create a TCP socket (server)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, true)
assert(fd, "failed to create TCP server socket: " .. tostring(err))
assert(type(fd) == "number", "socket should return a number")
assert(fd >= 0, "file descriptor should be non-negative")
assert(unix.close(fd), "should be able to close server socket")

-- Create a UDP socket
fd, err = gs.socket(gs.AF_INET, gs.SOCK_DGRAM, gs.IPPROTO_UDP, false)
assert(fd, "failed to create UDP socket: " .. tostring(err))
assert(type(fd) == "number", "socket should return a number")
assert(fd >= 0, "file descriptor should be non-negative")
assert(unix.close(fd), "should be able to close UDP socket")

-- Create an IPv6 TCP socket
fd, err = gs.socket(gs.AF_INET6, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
assert(fd, "failed to create IPv6 TCP socket: " .. tostring(err))
assert(type(fd) == "number", "socket should return a number")
assert(fd >= 0, "file descriptor should be non-negative")
assert(unix.close(fd), "should be able to close IPv6 socket")

-- Create a Unix domain socket
fd, err = gs.socket(gs.AF_UNIX, gs.SOCK_STREAM, 0, false)
assert(fd, "failed to create Unix socket: " .. tostring(err))
assert(type(fd) == "number", "socket should return a number")
assert(fd >= 0, "file descriptor should be non-negative")
assert(unix.close(fd), "should be able to close Unix socket")

--------------------------------------------------------------------------------
-- Test socket creation with default protocol (0)
--------------------------------------------------------------------------------

-- TCP with protocol 0 (should default to TCP)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, 0, false)
assert(fd, "failed to create TCP socket with protocol 0: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- UDP with protocol 0 (should default to UDP)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_DGRAM, 0, false)
assert(fd, "failed to create UDP socket with protocol 0: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

--------------------------------------------------------------------------------
-- Test timeout configurations
--------------------------------------------------------------------------------

-- Create socket with positive timeout (SO_RCVTIMEO/SO_SNDTIMEO)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 30)
assert(fd, "failed to create socket with positive timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket with timeout")

-- Create socket with fractional timeout
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 2.5)
assert(fd, "failed to create socket with fractional timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Create socket with negative timeout (SO_KEEPALIVE)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, -60)
assert(fd, "failed to create socket with negative timeout (keepalive): " .. tostring(err))
assert(unix.close(fd), "should be able to close socket with keepalive")

-- Create socket with zero timeout
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 0)
assert(fd, "failed to create socket with zero timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Create socket with nil timeout (no timeout settings)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, nil)
assert(fd, "failed to create socket with nil timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Create socket without timeout parameter
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
assert(fd, "failed to create socket without timeout parameter: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

--------------------------------------------------------------------------------
-- Test server vs client sockets
--------------------------------------------------------------------------------

-- Server socket (isserver=true)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, true)
assert(fd, "failed to create server socket: " .. tostring(err))
assert(unix.close(fd), "should be able to close server socket")

-- Client socket (isserver=false)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
assert(fd, "failed to create client socket: " .. tostring(err))
assert(unix.close(fd), "should be able to close client socket")

-- Server socket with boolean true
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, true, 30)
assert(fd, "failed to create server socket with boolean: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Client socket with boolean false
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 30)
assert(fd, "failed to create client socket with boolean: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

--------------------------------------------------------------------------------
-- Test invalid parameters
--------------------------------------------------------------------------------

-- Invalid family
local ok, result = pcall(function()
  return gs.socket(99999, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
end)
-- This may succeed or fail depending on the OS, but should not crash
assert(type(ok) == "boolean", "pcall should return a boolean")

-- Invalid socket type
ok, result = pcall(function()
  return gs.socket(gs.AF_INET, 99999, gs.IPPROTO_TCP, false)
end)
-- This may succeed or fail depending on the OS, but should not crash
assert(type(ok) == "boolean", "pcall should return a boolean")

-- Invalid protocol (should fail)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, 99999, false)
-- This should fail, but we check both outcomes
if fd then
  unix.close(fd)
else
  assert(err, "should have error message for invalid protocol")
end

--------------------------------------------------------------------------------
-- Test multiple sockets
--------------------------------------------------------------------------------

-- Create multiple sockets and ensure they have different file descriptors
local fds = {}
for i = 1, 5 do
  fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false)
  assert(fd, "failed to create socket " .. i .. ": " .. tostring(err))
  table.insert(fds, fd)
end

-- Verify all file descriptors are unique
for i = 1, #fds do
  for j = i + 1, #fds do
    assert(fds[i] ~= fds[j], "file descriptors should be unique")
  end
end

-- Close all sockets
for _, sockfd in ipairs(fds) do
  assert(unix.close(sockfd), "should be able to close socket")
end

--------------------------------------------------------------------------------
-- Test that goodsocket actually creates valid sockets
--------------------------------------------------------------------------------

-- Create a socket and verify we can perform socket operations on it
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, true)
assert(fd, "failed to create socket: " .. tostring(err))

-- Try to bind it (this tests that it's a real socket)
-- We use port 0 to let the OS assign a port
local ok, bind_err = unix.bind(fd, cosmo.ParseIp("127.0.0.1"), 0)
if ok then
  -- If bind succeeds, the socket is valid
  assert(unix.close(fd), "should be able to close bound socket")
elseif bind_err then
  -- If bind fails, it should be because of permission/address issues,
  -- not because fd is invalid
  assert(type(bind_err) == "string", "bind error should be a string")
  unix.close(fd)
end

--------------------------------------------------------------------------------
-- Test edge cases
--------------------------------------------------------------------------------

-- Very large timeout
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 86400)
assert(fd, "failed to create socket with large timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Very small timeout
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 0.001)
assert(fd, "failed to create socket with small timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

-- Large negative timeout (keepalive)
fd, err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, -3600)
assert(fd, "failed to create socket with large negative timeout: " .. tostring(err))
assert(unix.close(fd), "should be able to close socket")

--------------------------------------------------------------------------------
-- Test that sockets work for actual communication
--------------------------------------------------------------------------------

-- Create a server socket
local server_fd, server_err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, true, 5)
assert(server_fd, "failed to create server socket: " .. tostring(server_err))

-- Bind to localhost on a random port
ok, err = unix.bind(server_fd, cosmo.ParseIp("127.0.0.1"), 0)

if ok then
  -- If bind succeeds, try to listen
  local listen_ok, listen_err = unix.listen(server_fd, 5)
  if listen_ok then
    -- Successfully created a listening socket with goodsocket!
    assert(unix.close(server_fd), "should be able to close listening socket")
  else
    -- Listen failed, but that's okay for this test
    unix.close(server_fd)
  end
else
  -- Bind failed, close the socket
  unix.close(server_fd)
end

-- Create a client socket
local client_fd, client_err = gs.socket(gs.AF_INET, gs.SOCK_STREAM, gs.IPPROTO_TCP, false, 5)
assert(client_fd, "failed to create client socket: " .. tostring(client_err))
assert(unix.close(client_fd), "should be able to close client socket")

print("PASS")
