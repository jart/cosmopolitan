-- special script called by main redbean process at startup
HidePath('/usr/share/zoneinfo/')
HidePath('/usr/share/ssl/')

-- 20Ki, for certain autobahn tests
ProgramMaxPayloadSize(20 * 1024 * 1024)

function OnHttpRequest()
  if GetPath() == "/ws" then
    ws.Write(nil) -- upgrade without sending a response
    coroutine.yield()

    local fds = {[GetClientFd()] = unix.POLLIN}
    -- simple echo server
    while true do
      unix.poll(fds)
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


