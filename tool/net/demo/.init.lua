mymodule = require "mymodule"
sqlite3 = require "lsqlite3"

-- ddos protection
ProgramTokenBucket()

-- /.init.lua is loaded at startup in redbean's main process
HidePath('/usr/share/zoneinfo/')
HidePath('/usr/share/ssl/')

-- open a browser tab using explorer/open/xdg-open
LaunchBrowser('/tool/net/demo/index.html')

-- sql database (see sql.lua)
db = sqlite3.open_memory()
db:exec[[
  CREATE TABLE test (
    id INTEGER PRIMARY KEY,
    content TEXT
  );
  INSERT INTO test (content) VALUES ('Hello World');
  INSERT INTO test (content) VALUES ('Hello Lua');
  INSERT INTO test (content) VALUES ('Hello Sqlite3');
]]

-- shared memory hit counter
SHM_LOCK = 0  -- index zero (first eight bytes) will hold mutex
SHM_JSON = 8  -- store json payload starting at the index eight
shm = unix.mapshared(65520)
function Lock()
    local ok, old = shm:cmpxchg(SHM_LOCK, 0, 1)
    if not ok then
        if old == 1 then
            old = shm:xchg(SHM_LOCK, 2)
        end
        while old > 0 do
            shm:wait(SHM_LOCK, 2)
            old = shm:xchg(SHM_LOCK, 2)
        end
    end
end
function Unlock()
    old = shm:fetch_add(SHM_LOCK, -1)
    if old == 2 then
        shm:store(SHM_LOCK, 0)
        shm:wake(SHM_LOCK, 1)
    end
end

function OnServerListen(fd, ip, port)
    unix.setsockopt(fd, unix.SOL_TCP, unix.TCP_SAVE_SYN, true)
    return false
end

function OnClientConnection(ip, port, serverip, serverport)
    syn, synerr = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SAVED_SYN)
end

function UpdateHitCounter()
    local s, t, k
    Lock()
    s = shm:read(SHM_JSON)
    if s == '' then s = '{}' end
    t = DecodeJson(s)
    k = GetPath()
    if not t[k] then t[k] = 0 end
    t[k] = t[k] + 1
    shm:write(SHM_JSON, EncodeJson(t))
    Unlock()
end

-- this intercepts all requests if it's defined
function OnHttpRequest()
    UpdateHitCounter()
    if GetHeader('User-Agent') then
        Log(kLogInfo, "client is running %s and reports %s" % {
                finger.GetSynFingerOs(finger.FingerSyn(syn)),
                VisualizeControlCodes(GetHeader('User-Agent'))})
    end
    if HasParam('magic') then
        Write('<p>\r\n')
        Write('OnHttpRequest() has intercepted your request<br>\r\n')
        Write('because you specified the magic parameter\r\n')
        Write('<pre>\r\n')
        Write(EscapeHtml(LoadAsset('/.init.lua')))
        Write('</pre>\r\n')
    else
        Route() -- this asks redbean to do the default thing
    end
    SetHeader('Server', 'redbean!')
end

function Adder(x, y)
    return x + y
end
