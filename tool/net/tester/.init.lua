-- special script called by main redbean process at startup
HidePath('/usr/share/zoneinfo/')
HidePath('/usr/share/ssl/')

-- 20Ki, for certain autobahn tests
ProgramMaxPayloadSize(20 * 1024 * 1024)

function OnHttpRequest()
  if GetPath() == "/ws" then
    ws.Write(nil) -- upgrade without sending a response
    coroutine.yield()

    local fd = GetClientFd()
    local client_exit = unix.POLLHUP | unix.POLLRDHUP | unix.POLLERR
    local fds = {[fd] = unix.POLLIN | client_exit}
    -- simple echo server
    while true do
      res = unix.poll(fds)
      if res[fd] & client_exit > 0 then
        return
      end
      local s, t = ws.Read()
      if t == ws.CLOSE then
        return
      elseif t == ws.TEXT then
        ws.Write(s, ws.TEXT)
        coroutine.yield()
      elseif t == ws.BIN then
        ws.Write(s, ws.BIN)
        coroutine.yield()
      end
    end
  else
    Route()
  end
end


