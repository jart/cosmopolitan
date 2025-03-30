--[[
XTERM Proof of concept
This shows how redbean can be used to launch a powershell (windows)
or bash (other OS) xterm session

The demo loads an HTML page that loads https://xtermjs.org/ and
defines a unique xtermid. An SSE EventSource is then defined which
starts a SSEServer that takes care of starting the shell command
As SSE is mostly a Server=>Client channel, the keyboard events/commands
are sent via side-channel POST requests

SSE Events :
 - noname : stdout from the shell, encodeURIComponent encoded
 - prompt : a way for the server to write a prompt on the client
            (different shells can have different interactive behavior)
 - exit   : a way to ask the client to stop its EventSource.
            this will trigger a POST.exit to close the SSEServer

POST Events :
 - command : a command that was typed in the xterm.js
 - exit : a way to ask the SSE server to shut down
 - ping : a way to tell the server that the xterm.js session is still
          alive. A vanished browser session will lead to the closing of
	  the SSE server

In order to share and reconcile the POSTed Events between the different
processes (POST processes and SSE server), a json shm is used where
the xtermid is used as a key to store the queue of commands.

Notes:
- security: Use at your own risk as it opens a shell on the server and
  makes it available on the listening ip/ports
- limitations: the demo does not use a real pty so there are certainly
  a lot of limitations and it is not made to be keyboard spammed !
- special chars are currently not handled. Things like CTRL-C to stop 
  a command is not implemented
- CTRL-V is not implemented
- history is not implemented
- tab completion is not implemented
- some mechanisms have been put in place to try to garbage collect the
  shm (tab closing, sending the 'exit' command, .. but there are probably
  a lot of other corner cases that need to be tested and handled
- the session state management and supervision could certainly be improved

]]--

ipcat = CategorizeIp(GetServerAddr())
if ipcat ~= 'PRIVATE' and ipcat ~= 'LOOPBACK' then
    Write('As a first level of security, the xterm/shell demo is only available on IPs categorized as private or loopback')
    Write('The current category is ' .. ipcat)
    return
end




function HTMLPage() 

Write('<link rel="stylesheet" href="https://xtermjs.org/css/xterm.css" />\n')
Write('<script src="https://xtermjs.org/js/xterm.js"></script>\n')
Write('<script src="https://xtermjs.org/js/addon-webgl.js"></script>\n')
Write('<h1>Redbean xterm.js/shell demo</h1>\n')
Write('<p>you should be able to interact with powershell on windows or bash on other OSs</p>')
Write('<span>Try usual commands like:</span>')
Write('<ul>')
Write('<li>ls</li>')
Write('<li>whoami</li>')
Write('<li>ps</li>')
Write('<li>or any command you dare try ;-)</li>')

Write('</ul>')
Write('<div class="demo"><div class="inner"></div></div>\n')
Write('<script>var xtermid = "'.. UuidV4() ..'"</script>\n')

script = [[
var baseTheme = {
    foreground: '#F8F8F8',
    background: '#2D2E2C',
    selection: '#5DA5D533',
    black: '#1E1E1D',
    brightBlack: '#262625',
    red: '#CE5C5C',
    brightRed: '#FF7272',
    green: '#5BCC5B',
    brightGreen: '#72FF72',
    yellow: '#CCCC5B',
    brightYellow: '#FFFF72',
    blue: '#5D5DD3',
    brightBlue: '#7279FF',
    magenta: '#BC5ED1',
    brightMagenta: '#E572FF',
    cyan: '#5DA5D5',
    brightCyan: '#72F0FF',
    white: '#F8F8F8',
    brightWhite: '#FFFFFF'
  };
var isBaseTheme = true;
var term = new window.Terminal({
    fontFamily: '"Cascadia Code", Menlo, monospace',
    theme: baseTheme,
    cursorBlink: true,
    allowProposedApi: true,
    cols: 150,
    convertEol: true
  });
term.open(document.querySelector('.demo .inner'));
var isWebglEnabled = false;
try {
    const webgl = new window.WebglAddon.WebglAddon();
    term.loadAddon(webgl);
    isWebglEnabled = true;
} catch (e) {
    console.warn('WebGL addon threw an exception during load', e);
}


prompt_str = 'redbean shell \ud83e\udd9e  > '
function prompt(term, str) {
    command = '';
    prompt_str = str || prompt_str;
    term.write(prompt_str);
  }

var command = '';
var running = true;

function sendCommand(type, data) {
    return fetch('xterm.lua?xtermid='+xtermid, {
        method: "POST",
        body: JSON.stringify({ type: type, data: data })
    });
}

function runCommand(term, text) {
    const command = text.trim();
    if (command.length > 0) {
        term.writeln('');
        sendCommand("command", command);
    } else {
        term.write("\r\n");
        prompt(term);
    }
}
  
term.onData(e => {
    if (!running) return;
    switch (e) {
        case '\u0003': // Ctrl+C
            term.write('^C');
            prompt(term);
            break;
        case '\r': // Enter
            runCommand(term, command);
            command = '';
            break;
        case '\u007F': // Backspace (DEL)
            // Do not delete the prompt
            if (term._core.buffer.x > 2) {
                term.write('\b \b');
                if (command.length > 0) {
                    command = command.substr(0, command.length - 1);
                }
            }
            break;
        default: // Print all other characters for demo
            if (e >= String.fromCharCode(0x20) && e <= String.fromCharCode(0x7E) || e >= '\u00a0') {
              command += e;
              term.write(e);
            }
    }
});



var evtSource = new EventSource('xterm.lua?xtermid='+xtermid+'&sse');
evtSource.onmessage = (event) => {
    term.write(decodeURIComponent(event.data))
}; 
evtSource.addEventListener("prompt", (event) => {
    prompt(term, decodeURIComponent(event.data))
});
evtSource.addEventListener("exit", (event) => {
    console.log("exit", event.data);
    evtSource.close();
    running = false;
    term.write('..TERMINATED..');
    sendCommand('exit', null);
});


function ping() {
    if (running) {
        sendCommand('ping', null);
    }
}

// we ping the SSE server to avoid a dangling a SSE process on the server
window.addEventListener('load', function () {
  var fetchInterval = 2000;
  setInterval(ping, fetchInterval);
});

]]

Write('<script>'..script..'</script><body>')

end

-- SSE Events needs yielding to be sent as event-stream
local function streamWrap(func)
  return function(...) return coroutine.yield(func(...)) or true end
end
local function writeEvent(event, data)
    if event then Write("event: " .. event .. "\n") end
    if data then Write("data: " .. EscapeParam(data) .. "\n") end
    Write("\n")
end
local streamEvent = streamWrap(writeEvent)

function SSEServer()
    xtermid = GetParam('xtermid')
    if GetHostOs() == 'WINDOWS' then
        cmd = { 'pwsh.exe', "-Interactive", "-Command", "-" }
        eol = '\r\n'
    else
        cmd = { '/bin/bash' }
        eol = '\n'
    end
    cmd[1] = assert(unix.commandv(cmd[1]))

    readerIn, writerIn = assert(unix.pipe(unix.O_CLOEXEC))
    readerOut, writerOut = assert(unix.pipe(unix.O_CLOEXEC))
    child = assert(unix.fork())
    
    if child == 0 then
        unix.close(0)
        unix.dup(readerIn)
        unix.close(1)
        unix.dup(writerOut)
        unix.close(2)
        unix.dup(writerOut)
        unix.close(writerIn)
        unix.close(readerOut)
        unix.execve(cmd[1], cmd)
        unix.exit(127)
    else
        SetHeader('Content-Type', 'text/event-stream')
        SetHeader('Connection','Close')
        unix.close(readerIn)
        unix.close(writerOut)

        pollfds = {}
        pollfds[readerOut] = unix.POLLIN
        polling = true
        promptblock = GetTime()
        needprompt = true
        lastping = GetTime()
  
        while polling do
            evs, errno = unix.poll(pollfds, 100)
            if not evs then
              break
            end

            hasStdout = false
            for fd,revents in pairs(evs) do
                if fd == readerOut then
                    hasStdout =true
                    data, errno = unix.read(fd)
                    if data ~= '' then
                        streamEvent(nil, data)
                        promptblock = GetTime() + 0.1
                    end
                end
            end
            command = nextCommand(xtermid)
            if command then lastping = GetTime() end
            if command and command.type == "command" then
                if command.data == 'exit' then
                    -- ask browser to close its SSE EventSource
                    streamEvent('exit', '')
                end
                unix.write(writerIn, command.data .. eol)
                promptblock = GetTime() + 1
                needprompt = true
            end
            if (command and command.type == "exit") or lastping < GetTime() - 5 then
                polling = false
            end
            if needprompt and not hasStdout and GetTime() > promptblock then
                streamEvent('prompt', 'redbean shell '..utf8.char(129438)..'  > ')
                needprompt = false
            end
        end
        unix.close(readerOut)
        unix.close(writerIn)
        deleteSession(xtermid)
        unix.wait(-1)
        Log(kLogWarn, 'xterm SSE server '..xtermid.. 'has been closed')
        return
    end
end

function registerCommand(xtermid)
    posted = GetPayload()
    local s, t, k
    Lock()
    s = shm:read(SHM_JSON)
    if s == '' then s = '{}' end
    t = DecodeJson(s)
    k = xtermid
    if not t[k] then t[k] = {} end
    table.insert(t[k], 1, DecodeJson(posted))
    shm:write(SHM_JSON, EncodeJson(t))
    Unlock()
end
function nextCommand(xtermid)
    local s, t, k
    Lock()
    s = shm:read(SHM_JSON)
    if s == '' then s = '{}' end
    t = DecodeJson(s)
    k = xtermid
    if not t[k] then t[k] = {} end
    command = table.remove(t[k])
    shm:write(SHM_JSON, EncodeJson(t))
    Unlock()
    return command
end
function deleteSession(xtermid)
    local s, t, k
    Lock()
    s = shm:read(SHM_JSON)
    if s == '' then s = '{}' end
    t = DecodeJson(s)
    k = xtermid
    t[k] = nil
    shm:write(SHM_JSON, EncodeJson(t))
    Unlock()
end


if HasParam('sse') then
    SSEServer()
elseif GetMethod() == "POST" then
    registerCommand(GetParam('xtermid'))
else
    HTMLPage()
end


