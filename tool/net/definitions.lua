---@meta
error("Tried to evaluate definition file.")
--Documents redbean 2.2, lsqlite3 0.9.5

--[[

SYNOPSIS

  redbean.com [-?BVabdfghjkmsuvz] [-p PORT] [-D DIR] [-- SCRIPTARGS...]

DESCRIPTION

  redbean - single-file distributable web server

OVERVIEW

  redbean makes it possible to share web applications that run offline
  as a single-file Actually Portable Executable PKZIP archive which
  contains your assets. All you need to do is download the redbean.com
  program below, change the filename to .zip, add your content in a zip
  editing tool, and then change the extension back to .com.

  redbean can serve 1 million+ gzip encoded responses per second on a
  cheap personal computer. That performance is thanks to zip and gzip
  using the same compression format, which enables kernelspace copies.
  Another reason redbean goes fast is that it's a tiny static binary,
  which makes fork memory paging nearly free.

  redbean is also easy to modify to suit your own needs. The program
  itself is written as a single .c file. It embeds the Lua programming
  language and SQLite which let you write dynamic pages.

FEATURES

  - Lua v5.4
  - SQLite 3.35.5
  - TLS v1.2 / v1.1 / v1.0
  - HTTP v1.1 / v1.0 / v0.9
  - Chromium-Zlib Compression
  - Statusz Monitoring Statistics
  - Self-Modifying PKZIP Object Store
  - Linux + Windows + Mac + FreeBSD + OpenBSD + NetBSD

FLAGS

  -h or -?  help
  -d        daemonize
  -u        uniprocess
  -z        print port
  -m        log messages
  -i        interpreter mode
  -b        log message bodies
  -a        log resource usage
  -g        log handler latency
  -E        show crash reports to public ips
  -j        enable ssl client verify
  -k        disable ssl fetch verify
  -Z        log worker system calls
  -f        log worker function calls
  -B        only use stronger cryptography
  -X        disable ssl server and client support
  -*        permit self-modification of executable
  -J        disable non-ssl server and client support
  -%        hasten startup by not generating an rsa key
  -s        increase silence                  [repeatable]
  -v        increase verbosity                [repeatable]
  -V        increase ssl verbosity            [repeatable]
  -S        increase pledge sandboxing        [repeatable]
  -e CODE   eval Lua code in arg              [repeatable]
  -F PATH   eval Lua code in file             [repeatable]
  -H K:V    sets http header globally         [repeatable]
  -D DIR    overlay assets in local directory [repeatable]
  -r /X=/Y  redirect X to Y                   [repeatable]
  -R /X=/Y  rewrites X to Y                   [repeatable]
  -K PATH   tls private key path              [repeatable]
  -C PATH   tls certificate(s) path           [repeatable]
  -A PATH   add assets with path (recursive)  [repeatable]
  -M INT    tunes max message payload size    [def. 65536]
  -t INT    timeout ms or keepalive sec if <0 [def. 60000]
  -p PORT   listen port                       [def. 8080; repeatable]
  -l ADDR   listen addr                       [def. 0.0.0.0; repeatable]
  -c SEC    configures static cache-control
  -W TTY    use tty path to monitor memory pages
  -L PATH   log file location
  -P PATH   pid file location
  -U INT    daemon set user id
  -G INT    daemon set group id
  -w PATH   launch browser on startup
  --strace  enables system call tracing (see also -Z)
  --ftrace  enables function call tracing (see also -f)

KEYBOARD

  CTRL-D         EXIT
  CTRL-C CTRL-C  EXIT
  CTRL-E         END
  CTRL-A         START
  CTRL-B         BACK
  CTRL-F         FORWARD
  CTRL-L         CLEAR
  CTRL-H         BACKSPACE
  CTRL-D         DELETE
  CTRL-N         NEXT HISTORY
  CTRL-P         PREVIOUS HISTORY
  CTRL-R         SEARCH HISTORY
  CTRL-G         CANCEL SEARCH
  ALT-<          BEGINNING OF HISTORY
  ALT->          END OF HISTORY
  ALT-F          FORWARD WORD
  ALT-B          BACKWARD WORD
  CTRL-RIGHT     FORWARD WORD
  CTRL-LEFT      BACKWARD WORD
  CTRL-K         KILL LINE FORWARDS
  CTRL-U         KILL LINE BACKWARDS
  ALT-H          KILL WORD BACKWARDS
  CTRL-W         KILL WORD BACKWARDS
  CTRL-ALT-H     KILL WORD BACKWARDS
  ALT-D          KILL WORD FORWARDS
  CTRL-Y         YANK
  ALT-Y          ROTATE KILL RING AND YANK AGAIN
  CTRL-T         TRANSPOSE
  ALT-T          TRANSPOSE WORD
  ALT-U          UPPERCASE WORD
  ALT-L          LOWERCASE WORD
  ALT-C          CAPITALIZE WORD
  CTRL-\         QUIT PROCESS
  CTRL-S         PAUSE OUTPUT
  CTRL-Q         UNPAUSE OUTPUT (IF PAUSED)
  CTRL-Q         ESCAPED INSERT
  CTRL-ALT-F     FORWARD EXPR
  CTRL-ALT-B     BACKWARD EXPR
  ALT-RIGHT      FORWARD EXPR
  ALT-LEFT       BACKWARD EXPR
  ALT-SHIFT-B    BARF EXPR
  ALT-SHIFT-S    SLURP EXPR
  CTRL-SPACE     SET MARK
  CTRL-X CTRL-X  GOTO MARK
  CTRL-Z         SUSPEND PROCESS
  ALT-\          SQUEEZE ADJACENT WHITESPACE
  PROTIP         REMAP CAPS LOCK TO CTRL

────────────────────────────────────────────────────────────────────────────────
USAGE

  This executable is also a ZIP file that contains static assets.
  You can run redbean interactively in your terminal as follows:

    ./redbean.com -vvvmbag        # starts server verbosely
    open http://127.0.0.1:8080/   # shows zip listing page
    CTRL-C                        # 1x: graceful shutdown
    CTRL-C                        # 2x: forceful shutdown

  You can override the default listing page by adding:

    zip redbean.com index.lua     # lua server pages take priority
    zip redbean.com index.html    # default page for directory

  The listing page only applies to the root directory. However the
  default index page applies to subdirectories too. In order for it
  to work, there needs to be an empty directory entry in the zip.
  That should already be the default practice of your zip editor.

    wget                     \
      --mirror               \
      --convert-links        \
      --adjust-extension     \
      --page-requisites      \
      --no-parent            \
      --no-if-modified-since \
      http://a.example/index.html
    zip -r redbean.com a.example/  # default page for directory

  redbean normalizes the trailing slash for you automatically:

    $ printf 'GET /a.example HTTP/1.0\n\n' | nc 127.0.0.1 8080
    HTTP/1.0 307 Temporary Redirect
    Location: /a.example/

  Virtual hosting is accomplished this way too. The Host is simply
  prepended to the path, and if it doesn't exist, it gets removed.

    $ printf 'GET / HTTP/1.1\nHost:a.example\n\n' | nc 127.0.0.1 8080
    HTTP/1.1 200 OK
    Link: <http://127.0.0.1/a.example/index.html>; rel="canonical"

  If you mirror a lot of websites within your redbean then you can
  actually tell your browser that redbean is your proxy server, in
  which redbean will act as your private version of the Internet.

    $ printf 'GET http://a.example HTTP/1.0\n\n' | nc 127.0.0.1 8080
    HTTP/1.0 200 OK
    Link: <http://127.0.0.1/a.example/index.html>; rel="canonical"

  If you use a reverse proxy, then redbean recognizes the following
  provided that the proxy forwards requests over the local network:

    X-Forwarded-For: 203.0.113.42:31337
    X-Forwarded-Host: foo.example:80

  There's a text/plain statistics page called /statusz that makes
  it easy to track and monitor the health of your redbean:

    printf 'GET /statusz\n\n' | nc 127.0.0.1 8080

  redbean will display an error page using the /redbean.png logo
  by default, embedded as a bas64 data uri. You can override the
  custom page for various errors by adding files to the zip root.

    zip redbean.com 404.html      # custom not found page

  Audio video content should not be compressed in your ZIP files.
  Uncompressed assets enable browsers to send Range HTTP request.
  On the other hand compressed assets are best for gzip encoding.

    zip redbean.com index.html    # adds file
    zip -0 redbean.com video.mp4  # adds without compression

  You can have redbean run as a daemon by doing the following:

    sudo ./redbean.com -vvdp80 -p443 -L redbean.log -P redbean.pid
    kill -TERM $(cat redbean.pid) # 1x: graceful shutdown
    kill -TERM $(cat redbean.pid) # 2x: forceful shutdown

  redbean currently has a 32kb limit on request messages and 64kb
  including the payload. redbean will grow to whatever the system
  limits allow. Should fork() or accept() fail redbean will react
  by going into "meltdown mode" which closes lingering workers.
  You can trigger this at any time using:

    kill -USR2 $(cat redbean.pid)

  Another failure condition is running out of disk space in which
  case redbean reacts by truncating the log file. Lastly, redbean
  does the best job possible reporting on resource usage when the
  logger is in debug mode noting that NetBSD is the best at this.

  Your redbean is an actually portable executable, that's able to
  run on six different operating systems. To do that, it needs to
  extract a 4kb loader program to ${TMPDIR:-${HOME:-.}}/.ape that'll
  map your redbean into memory. It does however check to see if `ape`
  is on the system path beforehand. You can also "assimilate" any
  redbean into the platform-local executable format by running:

      $ file redbean.com
      redbean.com: DOS/MBR boot sector
      $ ./redbean.com --assimilate
      $ file redbean.com
      redbean.com: ELF 64-bit LSB executable

────────────────────────────────────────────────────────────────────────────────
SECURITY

  redbean uses a protocol polyglot for serving HTTP and HTTPS on
  the same port numbers. For example, both of these are valid:

    http://127.0.0.1:8080/
    https://127.0.0.1:8080/

  SSL verbosity is controlled as follows for troubleshooting:

    -V       log ssl errors
    -VV      log ssl state changes too
    -VVV     log ssl informational messages too
    -VVVV    log ssl verbose details too

  Redbean supports sandboxing flags on Linux and OpenBSD.

    -S (online policy)

      This causes unix.pledge("stdio rpath inet dns") to be called on
      workers after fork() is called. This permits read-only operations
      and APIs like Fetch() that let workers send and receive data with
      private and public Internet hosts. Access to the unix module is
      somewhat restricted, disallowing its more powerful APIs like exec.

    -SS (offline policy)

      This causes unix.pledge("stdio rpath") to be called on workers
      after after fork() is called. This prevents workers from talking
      to the network (other than the client) and allows read-only file
      system access (e.g. `-D DIR` flag).

    -SSS (contained policy)

      This causes unix.pledge("stdio") to be called on workers after
      after fork() is called. This prevents workers from communicating
      with the network (other than the client connection) and prevents
      file system access (with some exceptions like logging). Redbean
      should only be able to serve from its own zip file in this mode.
      Lua script access to the unix module is highly restricted.

  See http://redbean.dev for further details.

────────────────────────────────────────────────────────────────────────────────
LUA SERVER PAGES

  Any files with the extension .lua will be dynamically served by redbean.
  Here's the simplest possible example:

    Write('<b>Hello World</b>')

  The Lua Server Page above should be able to perform at 700,000 responses
  per second on a Core i9, without any sort of caching. If you want a Lua
  handler that can do 1,000,000 responses per second, then try adding the
  following global handler to your /.init.lua file:

    function OnHttpRequest()
       Write('<b>Hello World</b>')
    end

  Here's an example of a more typical workflow for Lua Server Pages using
  the redbean API:

    SetStatus(200)
    SetHeader('Content-Type', 'text/plain; charset=utf-8')
    Write('<p>Hello ')
    Write(EscapeHtml(GetParam('name')))

  We didn't need the first two lines in the previous example, because
  they're implied by redbean automatically if you don't set them. Responses
  are also buffered until the script finishes executing. That enables
  redbean to make HTTP as easy as possible. In the future, API capabilities
  will be expanded to make possible things like websockets.

  redbean embeds the Lua standard library. You can use packages such as io
  to persist and share state across requests and connections, as well as the
  StoreAsset function, and the lsqlite3 module.

  Your Lua interpreter begins its life in the main process at startup in the
  .init.lua, which is likely where you'll want to perform all your expensive
  one-time operations like importing modules. Then, as requests roll in,
  isolated processes are cloned from the blueprint you created.

────────────────────────────────────────────────────────────────────────────────
REPL

  Your redbean displays a Read-Eval-Print-Loop that lets you modify the
  state of the main server process while your server is running. Any
  changes will propagate into forked clients.

  Your REPL is displayed only when redbean is run as a non-daemon in a
  Unix terminal or the Windows 10 command prompt or PowerShell. Since
  the REPL is a Lua REPL it's not included in a redbean-static builds.

  redbean uses the same keyboard shortcuts as GNU Readline and Emacs.
  Some of its keyboard commands (listed in a previous section) were
  inspired by Paredit.

  A history of your commands is saved to `~/.redbean_history`.

  If you love the redbean repl and want to use it as your language
  interpreter then you can pass the `-i` flag to put redbean into
  interpreter mode.

      redbean.com -i binarytrees.lua 15

  When the `-i` flag is passed (for interpreter mode), redbean won't
  start a web server and instead functions like the `lua` command. The
  first command line argument becomes the script you want to run. If you
  don't supply a script, then the repl without a web server is
  displayed. This is useful for testing since redbean extensions and
  modules for the Lua language, are still made available. You can also
  write redbean scripts with shebang lines:

      #!/usr/bin/redbean -i
      print('hello world')

  However UNIX operating systems usually require that interpreters be
  encoded in its preferred executable format. You can assimilate your
  redbean into the local format using the following commands:

      $ file redbean.com
      redbean.com: DOS/MBR boot sector
      $ ./redbean.com --assimilate
      $ file redbean.com
      redbean.com: ELF 64-bit LSB executable
      $ sudo cp redbean.com /usr/bin/redbean

  By following the above steps, redbean can be installed systemwide for
  multiple user accounts. It's also possible to chmod the binary to have
  setuid privileges. Please note that, if you do this, the UNIX section
  provides further details on APIs like `unix.setuid` that will help you
  remove root privileges from the process in the appropriate manner.


────────────────────────────────────────────────────────────────────────────────
LUA ENHANCEMENTS

  We've made some enhancements to the Lua language that should make it
  more comfortable for C/C++ and Python developers. Some of these

    - redbean supports a printf modulus operator, like Python. For
      example, you can say `"hello %s" % {"world"}` instead of
      `string.format("hello %s", "world")`.
      
      --
      - redbean supports a string multiply operator, like Python. For
      example, you can say `"hi" * 2` instead of `string.rep("hi", 2)`.
      
      - redbean supports octal (base 8) integer literals. For example
      `0644 == 420` is the case in redbean, whereas in upstream Lua
      `0644 == 644` would be the case.
      
      - redbean supports binary (base 2) integer literals. For example
      `0b1010 == 10` is the case in redbean, whereas in upstream Lua
      `0b1010` would result in an error.
      
      - redbean supports the GNU syntax for the ASCII ESC character in
      string literals. For example, `"\e"` is the same as `"\x1b"`.
      
]]

---@class string
---@operator mod(any[]): string
---@operator mul(integer): string

-- GLOBALS

--- Array of command line arguments, excluding those parsed by
--- getopt() in the C code, which stops parsing at the first
--- non-hyphenated arg. In some cases you can use the magic --
--- argument to delimit C from Lua arguments.
---
--- For example, if you launch your redbean as follows:
---
---     redbean.com -v arg1 arg2
---
--- Then your `/.init.lua` file will have the `arg` array like:
---
---     arg[-1] = '/usr/bin/redbean.com'
---     arg[ 0] = '/zip/.init.lua'
---     arg[ 1] = 'arg1'
---     arg[ 2] = 'arg2'
---
--- If you launch redbean in interpreter mode (rather than web
--- server) mode, then an invocation like this:
---
---     ./redbean.com -i script.lua arg1 arg2
---
--- Would have an `arg` array like this:
---
---     arg[-1] = './redbean.com'
---     arg[ 0] = 'script.lua'
---     arg[ 1] = 'arg1'
---     arg[ 2] = 'arg2'
---@type string[]
arg = nil

---@deprecated Use `arg` instead.
argv = nil


--[[
────────────────────────────────────────────────────────────────────────────────
SPECIAL PATHS

/
        redbean will generate a zip central directory listing for this
        page, and this page only, but only if there isn't an /index.lua or
        /index.html file defined.

/.init.lua
        This script is run once in the main process at startup. This lets
        you modify the state of the Lua interpreter before connection
        processes are forked off. For example, it's a good idea to do
        expensive one-time computations here. You can also use this file
        to call the ProgramFOO() functions below. The init module load
        happens after redbean's arguments and zip assets have been parsed,
        but before calling functions like socket() and fork(). Note that
        this path is a hidden file so that it can't be unintentionally run
        by the network client.

/.reload.lua (deprecated; use OnServerReload instead)
        This script is run from the main process when SIGHUP is received.
        This only applies to redbean when running in daemon mode. Any
        changes that are made to the Lua interpreter state will be
        inherited by future forked connection processes. Note that this
        path is a hidden file so that it can't be unintentionally run by
        the network client.

/.lua/...
        Your Lua modules go in this directory. The way it works is redbean
        sets Lua's package.path to /zip/.lua/?.lua;/zip/.lua/?/init.lua by
        default. Cosmopolitan Libc lets system calls like open read from
        the ZIP structure, if the filename is prefixed with /zip/. So this
        works like magic.

/redbean.png
        If it exists, it'll be used as the / listing page icon, embedded
        as a base64 URI.

/usr/share/zoneinfo
        This directory contains a subset of the timezone database.
        Your `TZ` environment variable controls which one of these
        files is used by functions such as unix.localtime().

/usr/share/ssl/root
        This directory contains your root certificate authorities. It is
        needed so the Fetch() HTTPS client API can verify that a remote
        certificate was signed by a third party. You can add your own
        certificate files to this directory within the ZIP executable.
        If you enable HTTPS client verification then redbean will check
        that HTTPS clients (a) have a certificate and (b) it was signed.

/.args
        Specifies default command-line arguments.

        There's one argument per line. Trailing newline is ignored. If
        the special argument `...` is *not* encountered, then the
        replacement will only happen if *no* CLI args are specified.
        If the special argument `...` *is* encountered, then it'll be
        replaced with whatever CLI args were specified by the user.

        For example, you might want to use redbean.com in interpreter
        mode, where your script file is inside the zip. Then, if your
        redbean is run, what you want is to have the default behavior
        be running your script. In that case, you might:

            $ cat <<'EOF' >.args
            -i
            /zip/hello.lua
            EOF

            $ cat <<'EOF' >hello.lua
            print("hello world")
            EOF

            $ zip redbean.com .args hello.lua
            $ ./redbean.com
            hello world

        Please note that if you ran:

            $ ./redbean.com -vv

        Then the default mode of redbean will kick back in. To prevent
        that from happening, simply add the magic arg `...` to the end
        of your `.args` file.
]]

-- HOOKS

--- Hooks HTTP message handling.
---
--- If this function is defined in the global scope by your `/.init.lua`
--- then redbean will call it at the earliest possible moment to
--- hand over control for all messages (with the exception of `OPTIONS--*`
--- See functions like `Route` which asks redbean to do its default
--- thing from the handler.
---
function OnHttpRequest() end

--- Hooks client connection creation.
---
--- If this function is defined it'll be called from the main process
--- each time redbean accepts a new client connection.
---
---@param ip uint32
---@param port uint16
---@param serverip uint32
---@param serverport uint16
---@return boolean # If it returns `true`, redbean will close the connection without calling `fork`.
function OnClientConnection(ip, port, serverip, serverport) end

--- Hook latency logging.
---
--- If this function is defined it'll be called from the main process
--- each time redbean completes handling of a request, but before the
--- response is sent. The handler received the time (in µs) since the
--- request handling and connection handling started.
---
---@param reqtimeus integer
---@param contimeus integer
function OnLogLatency(reqtimeus, contimeus) end

--- Hooks process creation.
---
--- If this function is defined it'll be called from the main process
--- each time redbean forks a connection handler worker process. The
--- ip/port of the remote client is provided, along with the ip/port
--- of the listening interface that accepted the connection. This may
--- be used to create a server activity dashboard, in which case the
--- data provider handler should set `SetHeader('Connection','Close')`.
--- This won't be called in uniprocess mode.
---
---@param pid integer
---@param ip uint32
---@param port uint16
---@param serverip uint32
---@param serverport uint16
function OnProcessCreate(pid, ip, port, serverip, serverport) end

--- If this function is defined it'll be called from the main process
--- each time redbean reaps a child connection process using `wait4()`.
--- This won't be called in uniprocess mode.
---@param pid integer
function OnProcessDestroy(pid) end

--- If this function is defined it'll be called from the main process
--- on each server heartbeat. The heartbeat interval is configurable
--- with `ProgramHeartbeatInterval`.
function OnServerHeartbeat() end

--- If this function is defined it'll be called from the main process
--- before redbean starts listening on a port. This hook can be used
--- to modify socket configuration to set `SO_REUSEPORT`, for example.
---@param socketdescriptor integer
---@param serverip uint32
---@param serverport uint16
---@return boolean ignore If `true`, redbean will not listen to that ip/port.
function OnServerListen(socketdescriptor, serverip, serverport) end

--- If this function is defined it'll be called from the main process
--- on each server reload triggered by SIGHUP (for daemonized) and
--- SIGUSR1 (for all) redbean instances. `reindex` indicates if redbean
--- assets have been re-indexed following the signal.
---@param reindex boolean
function OnServerReload(reindex) end

--- If this function is defined it'll be called from the main process
--- right before the main event loop starts.
function OnServerStart() end

--- If this function is defined it'll be called from the main process
--- after all the connection processes have been reaped and `exit()` is
--- ready to be called.
function OnServerStop() end

--- If this function is defined it'll be called from the child worker
--- process after it's been forked and before messages are handled.
--- This won't be called in uniprocess mode.
function OnWorkerStart() end

--- If this function is defined it'll be called from the child worker
--- process once `_exit()` is ready to be called. This won't be called
--- in uniprocess mode.
function OnWorkerStop() end

-- DATATYPES

---@class Url
---@field scheme string e.g. `"http"`
---@field user string? the username string, or nil if absent
---@field pass string? the password string, or nil if absent
---@field host string? the hostname string, or nil if `url` was a path
---@field port string? the port string, or nil if absent
---@field path string? the path string, or nil if absent
---@field params string[][]? the URL paramaters e.g. `/?a=b&c` would be
--- represented as the data structure `{{"a", "b"}, {"c"}, ...}`
---@field fragment string? the stuff after the `#` character

---@alias uint32 integer Unsigned 32-bit integer
---@alias uint16 integer Unsigned 16-bit integer
---@alias uint8 integer Unsigned 8-bit integer
---@alias int8 integer Signed 8-bit integer

---@class EncoderOptions
---@field useoutput boolean? defaults to `false`. Encodes the result directly to the output buffer and returns `nil` value. This option is ignored if used outside of request handling code.
---@field sorted boolean? defaults to `true`. Lua uses hash tables so the order of object keys is lost in a Lua table. So, by default, we use strcmp to impose a deterministic output order. If you don't care about ordering then setting sorted=false should yield a performance boost in serialization.
---@field pretty boolean? defaults to `false`. Setting this option to true will cause tables with more than one entry to be formatted across multiple lines for readability.
---@field indent string? defaults to " ". This option controls the indentation of pretty formatting. This field is ignored if pretty isn't true.
---@field maxdepth integer? defaults to 64. This option controls the maximum amount of recursion the serializer is allowed to perform. The max is 32767. You might not be able to set it that high if there isn't enough C stack memory. Your serializer checks for this and will return an error rather than crashing.

-- FUNCTIONS

--- Appends data to HTTP response payload buffer.
---
--- This is buffered independently of headers.
---
---@param data string
function Write(data) end

--- Starts an HTTP response, specifying the parameters on its first line.
---
--- This method will reset the response and is therefore mutually
--- exclusive with `ServeAsset` and other `Serve*` functions. If a
--- status setting function isn't called, then the default behavior is
--- to send `200 OK`.
---
---@param code integer
---@param reason? string is optional since redbean can fill in the appropriate text for well-known magic numbers, e.g. `200`, `404`, etc.
function SetStatus(code, reason) end

--- Appends HTTP header to response header buffer.
---
--- Leading and trailing whitespace is trimmed automatically. Overlong
--- characters are canonicalized. C0 and C1 control codes are forbidden,
--- with the exception of tab. This function automatically calls
--- `SetStatus(200, "OK")` if a status has not yet been set. As
--- `SetStatus` and `Serve*` functions reset the response, `SetHeader`
--- needs to be called after `SetStatus` and `Serve*` functions are
--- called. The header buffer is independent of the payload buffer.
--- Neither is written to the wire until the Lua Server Page has
--- finished executing. This function disallows the setting of certain
--- headers such as and Content-Range which are abstracted by the
--- transport layer. In such cases, consider calling `ServeAsset`.
---
---@param name string is case-insensitive and restricted to non-space ASCII
---@param value string is a UTF-8 string that must be encodable as ISO-8859-1.
function SetHeader(name, value) end

--- Appends Set-Cookie HTTP header to the response header buffer.
---
--- Several Set-Cookie headers can be added to the same response.
--- `__Host-` and `__Secure-` prefixes are supported and may set or
--- overwrite some of the options (for example, specifying `__Host-`
--- prefix sets the Secure option to `true`, sets the path to `"/"`, and
--- removes the Domain option). The following options can be used (their
--- lowercase equivalents are supported as well):
---
--- - `Expires` sets the maximum lifetime of the cookie as an HTTP-date timestamp. Can be specified as a Date in the RFC1123 (string) format or as a UNIX timestamp (number of seconds).
--- - `MaxAge` sets number of seconds until the cookie expires. A zero or negative number will expire the cookie immediately. If both Expires and MaxAge are set, MaxAge has precedence.
--- - `Domain` sets the host to which the cookie will be sent.
--- - `Path` sets the path that must be present in the request URL, or the client will not send the Cookie header.
--- - `Secure` (boolean) requests the cookie to be only send to the server when a request is made with the https: scheme.
--- - `HttpOnly` (boolean) forbids JavaScript from accessing the cookie.
--- - `SameSite` (Strict, Lax, or None) controls whether a cookie is sent with cross-origin requests, providing some protection against cross-site request forgery attacks.
---
---@param name string
---@param value string
---@param options { Expires: string|integer?, MaxAge: integer?, Domain: string?, Path: string?, Secure: boolean?, HttpOnly: boolean?, SameSite: "Strict"|"Lax"|"None"? }?
function SetCookie(name, value, options) end

--- Returns first value associated with name. name is handled in a case-sensitive manner. This function checks Request-URL parameters first. Then it checks `application/x-www-form-urlencoded` from the message body, if it exists, which is common for HTML forms sending `POST` requests. If a parameter is supplied matching name that has no value, e.g. `foo` in `?foo&bar=value`, then the returned value will be `nil`, whereas for `?foo=&bar=value` it would be `""`. To differentiate between no-equal and absent, use the `HasParam` function. The returned value is decoded from ISO-8859-1 (only in the case of Request-URL) and we assume that percent-encoded characters were supplied by the client as UTF-8 sequences, which are returned exactly as the client supplied them, and may therefore may contain overlong sequences, control codes, `NUL` characters, and even numbers which have been banned by the IETF. It is the responsibility of the caller to impose further restrictions on validity, if they're desired.
---@param name string
---@return string value
---@nodiscard
function GetParam(name) end

--- Escapes HTML entities: The set of entities is `&><"'` which become `&amp;&gt;&lt;&quot;&#39;`. This function is charset agnostic and will not canonicalize overlong encodings. It is assumed that a UTF-8 string will be supplied. See `escapehtml.c`.
---@param str string
---@return string
---@nodiscard
function EscapeHtml(str) end

--- Launches web browser on local machine with URL to this redbean server. This function may be called from your `/.init.lua`.
---@param path string?
function LaunchBrowser(path) end

---@param ip uint32
---@return string # a string describing the IP address. This is currently Class A granular. It can tell you if traffic originated from private networks, ARIN, APNIC, DOD, etc.
---@nodiscard
function CategorizeIp(ip) end

--- Turns ISO-8859-1 string into UTF-8.
---@param iso_8859_1 string
---@return string UTF8
---@nodiscard
function DecodeLatin1(iso_8859_1) end

--- Turns binary into ASCII base-16 hexadecimal lowercase string.
---@param binary string
---@return string ascii
function EncodeHex(binary) end

--- Turns ASCII base-16 hexadecimal byte string into binary string,
--- case-insensitively. Non-hex characters may not appear in string.
---@param ascii string
---@return string binary
function DecodeHex(ascii) end

--- Decodes binary data encoded as base64.
---
--- This turns ASCII into binary, in a permissive way that ignores
--- characters outside the base64 alphabet, such as whitespace. See
--- `decodebase64.c`.
---
---@param ascii string
---@return string binary
---@nodiscard
function DecodeBase64(ascii) end

--- Turns binary into ASCII. This can be used to create HTML data:
--- URIs that do things like embed a PNG file in a web page. See
--- encodebase64.c.
---@param binary string
---@return string ascii
---@nodiscard
function EncodeBase64(binary) end

---@alias JsonEl<T> string|number|boolean|nil|{ [integer]: T }|{ [string]: T }
---@alias JsonValue JsonEl<JsonEl<JsonEl<JsonEl<JsonEl<JsonEl<JsonEl<any>>>>>>>

--- Turns JSON string into a Lua data structure.
---
--- This is a generally permissive parser, in the sense that like
--- v8, it permits scalars as top-level values. Therefore we must
--- note that this API can be thought of as special, in the sense
---
---     val = assert(DecodeJson(str))
---
--- will usually do the right thing, except in cases where `false`
--- or `null` are the top-level value. In those cases, it's needed
--- to check the second value too in order to discern from error
---
---     val, err = DecodeJson(str)
---     if not val then
---        if err then
---           print('bad json', err)
---        elseif val == nil then
---           print('val is null')
---        elseif val == false then
---           print('val is false')
---        end
---     end
---
--- This parser supports 64-bit signed integers. If an overflow
--- happens, then the integer is silently coerced to double, as
--- consistent with v8. If a double overflows into `Infinity`, we
--- coerce it to `null` since that's what v8 does, and the same
--- goes for underflows which, like v8, are coerced to `0.0`.
---
--- When objects are parsed, your Lua object can't preserve the
--- original ordering of fields. As such, they'll be sorted by
--- `EncodeJson()` and may not round-trip with original intent.
---
--- This parser has perfect conformance with JSONTestSuite.
---
--- This parser validates utf-8 and utf-16.
---@param input string
---@return JsonValue
---@nodiscard
---@overload fun(input: string): nil, error: string
function DecodeJson(input) end

--- Turns Lua data structure into JSON string.
---
--- Since Lua uses tables are both hashmaps and arrays, we use a
--- simple fast algorithm for telling the two apart. Tables with
--- non-zero length (as reported by `#`) are encoded as arrays,
--- and any non-array elements are ignored. For example:
---
---     >: EncodeJson({2})
---     "[2]"
---     >: EncodeJson({[1]=2, ["hi"]=1})
---     "[2]"
---
--- If there are holes in your array, then the serialized array
--- will exclude everything after the first hole. If the beginning
--- of your array is a hole, then an error is returned.
---
---     >: EncodeJson({[1]=1, [3]=3})
---     "[1]"
---     >: EncodeJson({[2]=1, [3]=3})
---     "[]"
---     >: EncodeJson({[2]=1, [3]=3})
---     nil     "json objects must only use string keys"
---
--- If the raw length of a table is reported as zero, then we
--- check for the magic element `[0]=false`. If it's present, then
--- your table will be serialized as empty array `[]`. An entry is
--- inserted by `DecodeJson()` automatically, only when encountering
--- empty arrays, and it's necessary in order to make empty arrays
--- round-trip. If raw length is zero and `[0]=false` is absent,
--- then your table will be serialized as an iterated object.
---
---     >: EncodeJson({})
---     "{}"
---     >: EncodeJson({[0]=false})
---     "[]"
---     >: EncodeJson({["hi"]=1})
---     "{\"hi\":1}"
---     >: EncodeJson({["hi"]=1, [0]=false})
---     "[]"
---     >: EncodeJson({["hi"]=1, [7]=false})
---     nil     "json objects must only use string keys"
---
--- The following options may be used:
---
--- - `useoutput`: `(bool=false)` encodes the result directly to the output buffer
---   and returns nil value. This option is ignored if used outside of request
---   handling code.
--- - `sorted`: `(bool=true)` Lua uses hash tables so the order of object keys is
---   lost in a Lua table. So, by default, we use strcmp to impose a deterministic
---   output order. If you don't care about ordering then setting `sorted=false`
---   should yield a performance boost in serialization.
--- - `pretty`: `(bool=false)` Setting this option to true will cause tables with
---   more than one entry to be formatted across multiple lines for readability.
--- - `indent`: `(str=" ")` This option controls the indentation of pretty
---    formatting. This field is ignored if pretty isn't `true`.
--- - `maxdepth`: `(int=64)` This option controls the maximum amount of recursion
---   the serializer is allowed to perform. The max is 32767. You might not be able
---   to set it that high if there isn't enough C stack memory. Your serializer
---   checks for this and will return an error rather than crashing.
---
--- If the raw length of a table is reported as zero, then we
--- check for the magic element `[0]=false`. If it's present, then
--- your table will be serialized as empty array `[]`. An entry is
--- inserted by `DecodeJson()` automatically, only when encountering
--- empty arrays, and it's necessary in order to make empty arrays
--- round-trip. If raw length is zero and `[0]=false` is absent,
--- then your table will be serialized as an iterated object.
---
--- This function will return an error if:
---
--- - value is cyclic
--- - value has depth greater than 64
--- - value contains functions, user data, or threads
--- - value is table that blends string / non-string keys
--- - Your serializer runs out of C heap memory (setrlimit)
---
--- We assume strings in value contain UTF-8. This serializer currently does not
--- produce UTF-8 output. The output format is right now ASCII. Your UTF-8 data
--- will be safely transcoded to `\uXXXX` sequences which are UTF-16. Overlong
--- encodings in your input strings will be canonicalized rather than validated.
---
--- NaNs are serialized as `null` and Infinities are `null` which is consistent
--- with the v8 behavior.
---@param value JsonValue
---@param options { useoutput: false?, sorted: boolean?, pretty: boolean?, indent: string?, maxdepth: integer? }?
---@return string
---@nodiscard
---@overload fun(value: JsonValue, options: { useoutput: true, sorted: boolean?, pretty: boolean?, indent: string?, maxdepth: integer? }): true
---@overload fun(value: JsonValue, options: { useoutput: boolean?, sorted: boolean?, pretty: boolean?, indent: string?, maxdepth: integer? }? ): nil, error: string
function EncodeJson(value, options) end

--- Turns Lua data structure into Lua code string.
---
--- Since Lua uses tables as both hashmaps and arrays, tables will only be
--- serialized as an array with determinate order, if it's an array in the
--- strictest possible sense.
---
--- 1. for all 𝑘=𝑣 in table, 𝑘 is an integer ≥1
--- 2. no holes exist between MIN(𝑘) and MAX(𝑘)
--- 3. if non-empty, MIN(𝑘) is 1
---
--- In all other cases, your table will be serialized as an object which is
--- iterated and displayed as a list of (possibly) sorted entries that have
--- equal signs.
---
---     >: EncodeLua({3, 2})
---     "{3, 2}"
---     >: EncodeLua({[1]=3, [2]=3})
---     "{3, 2}"
---     >: EncodeLua({[1]=3, [3]=3})
---     "{[1]=3, [3]=3}"
---     >: EncodeLua({["hi"]=1, [1]=2})
---     "{[1]=2, hi=1}"
---
--- The following options may be used:
---
--- - `useoutput`: `(bool=false)` encodes the result directly to the output buffer
---   and returns nil value. This option is ignored if used outside of request
---   handling code.
--- - `sorted`: `(bool=true)` Lua uses hash tables so the order of object keys is
---   lost in a Lua table. So, by default, we use strcmp to impose a deterministic
---   output order. If you don't care about ordering then setting `sorted=false`
---   should yield a performance boost in serialization.
--- - `pretty`: `(bool=false)` Setting this option to true will cause tables with
---   more than one entry to be formatted across multiple lines for readability.
--- - `indent`: `(str=" ")` This option controls the indentation of pretty
---    formatting. This field is ignored if pretty isn't `true`.
--- - `maxdepth`: `(int=64)` This option controls the maximum amount of recursion
---   the serializer is allowed to perform. The max is 32767. You might not be able
---   to set it that high if there isn't enough C stack memory. Your serializer
---   checks for this and will return an error rather than crashing.
---
--- If a user data object has a `__repr` or `__tostring` meta method, then that'll
--- be used to encode the Lua code.
---
--- This serializer is designed primarily to describe data. For example, it's used
--- by the REPL where we need to be able to ignore errors when displaying data
--- structures, since showing most things imperfectly is better than crashing.
--- Therefore this isn't the kind of serializer you'd want to use to persist data
--- in prod. Try using the JSON serializer for that purpose.
---
--- Non-encodable value types (e.g. threads, functions) will be represented as a
--- string literal with the type name and pointer address. The string description
--- is of an unspecified format that could most likely change. This encoder detects
--- cyclic tables; however instead of failing, it embeds a string of unspecified
--- layout describing the cycle.
---
--- Integer literals are encoded as decimal. However if the int64 number is ≥256
--- and has a population count of 1 then we switch to representating the number in
--- hexadecimal, for readability. Hex numbers have leading zeroes added in order
--- to visualize whether the number fits in a uint16, uint32, or int64. Also some
--- numbers can only be encoded expressionally. For example, `NaN`s are serialized
--- as `0/0`, and `Infinity` is `math.huge`.
---
---     >: 7000
---     7000
---     >: 0x100
---     0x0100
---     >: 0x10000
---     0x00010000
---     >: 0x100000000
---     0x0000000100000000
---     >: 0/0
---     0/0
---     >: 1.5e+9999
---     math.huge
---     >: -9223372036854775807 - 1
---     -9223372036854775807 - 1
---
--- The only failure return condition currently implemented is when C runs out of heap memory.
---@param options { useoutput: false?, sorted: boolean?, pretty: boolean?, indent: string?, maxdepth: integer? }?
---@return string
---@nodiscard
---@overload fun(value, options: { useoutput: true, sorted: boolean?, pretty: boolean?, indent: string?, maxdepth: integer? }): true
---@overload fun(value, options: EncoderOptions? ): nil, error: string
function EncodeLua(value, options) end

--- Turns UTF-8 into ISO-8859-1 string.
---@param utf8 string
---@param flags integer
---@return string iso_8859_1
---@nodiscard
function EncodeLatin1(utf8, flags) end

--- Escapes URL #fragment. The allowed characters are `-/?.~_@:!$&'()*+,;=0-9A-Za-z`
--- and everything else gets `%XX` encoded. Please note that `'&` can still break
--- HTML and that `'()` can still break CSS URLs. This function is charset agnostic
--- and will not canonicalize overlong encodings. It is assumed that a UTF-8 string
--- will be supplied. See `kescapefragment.S`.
---@param str string
---@return string
---@nodiscard
function EscapeFragment(str) end

--- Escapes URL host. See `kescapeauthority.S`.
---@param str string
---@return string
---@nodiscard
function EscapeHost(str) end

--- Escapes JavaScript or JSON string literal content. The caller is responsible
--- for adding the surrounding quotation marks. This implementation \uxxxx sequences
--- for all non-ASCII sequences. HTML entities are also encoded, so the output
--- doesn't need `EscapeHtml`. This function assumes UTF-8 input. Overlong
--- encodings are canonicalized. Invalid input sequences are assumed to
--- be ISO-8859-1. The output is UTF-16 since that's what JavaScript uses. For
--- example, some individual codepoints such as emoji characters will encode as
--- multiple `\uxxxx` sequences. Ints that are impossible to encode as UTF-16 are
--- substituted with the `\xFFFD` replacement character.
--- See `escapejsstringliteral.c`.
---@param str string
---@return string
---@nodiscard
function EscapeLiteral(str) end

--- Escapes URL parameter name or value. The allowed characters are `-.*_0-9A-Za-z`
--- and everything else gets `%XX` encoded. This function is charset agnostic and
--- will not canonicalize overlong encodings. It is assumed that a UTF-8 string
--- will be supplied. See `kescapeparam.S`.
---@param str string
---@return string
---@nodiscard
function EscapeParam(str) end

--- Escapes URL password. See `kescapeauthority.S`.
---@param str string
---@return string
---@nodiscard
function EscapePass(str) end

--- Escapes URL path. This is the same as EscapeSegment except slash is allowed.
--- The allowed characters are `-.~_@:!$&'()*+,;=0-9A-Za-z/` and everything else
--- gets `%XX` encoded. Please note that `'&` can still break HTML, so the output
--- may need EscapeHtml too. Also note that `'()` can still break CSS URLs. This
--- function is charset agnostic and will not canonicalize overlong encodings.
--- It is assumed that a UTF-8 string will be supplied. See `kescapepath.S`.
---@param str string
---@return string
---@nodiscard
function EscapePath(str) end

--- Escapes URL path segment. This is the same as EscapePath except slash isn't
--- allowed. The allowed characters are `-.~_@:!$&'()*+,;=0-9A-Za-z` and everything
--- else gets `%XX` encoded. Please note that `'&` can still break HTML, so the
--- output may need EscapeHtml too. Also note that `'()` can still break CSS URLs.
--- This function is charset agnostic and will not canonicalize overlong encodings.
--- It is assumed that a UTF-8 string will be supplied. See `kescapesegment.S`.
---@param str string
---@return string
---@nodiscard
function EscapeSegment(str) end

--- Escapes URL username. See `kescapeauthority.S`.
---@param str string
---@return string
---@nodiscard
function EscapeUser(str) end

--- If this option is programmed then redbean will not transmit a Server Name
--- Indicator (SNI) when performing `Fetch()` requests. This function is not
--- available in unsecure mode.
---@param bool boolean
function EvadeDragnetSurveillance(bool) end

--- Sends an HTTP/HTTPS request to the specified URL. If only the URL is provided,
--- then a GET request is sent. If both URL and body parameters are specified, then
--- a POST request is sent. If any other method needs to be specified (for example,
--- PUT or DELETE), then passing a table as the second value allows setting method
--- and body values as well other options:
---
--- - `method` (default: `"GET"`): sets the method to be used for the request.
---   The specified method is converted to uppercase.
--- - `body` (default: `""`): sets the body value to be sent.
--- - `followredirect` (default: `true`): forces temporary and permanent redirects
---    to be followed. This behavior can be disabled by passing `false`.
--- - `maxredirects` (default: `5`): sets the number of allowed redirects to
---   minimize looping due to misconfigured servers. When the number is exceeded,
---   the result of the last redirect is returned.
--- - `keepalive` (default = `false`): configures each request to keep the
---   connection open (unless closed by the server) and reuse for the
---   next request to the same host. This option is disabled when SSL
---   connection is used.
---   The mapping of hosts and their sockets is stored in a table
---   assigned to the `keepalive` field itself, so it can be passed to
---   the next call.
---   If the table includes the `close` field set to a true value,
---   then the connection is closed after the request is made and the
---   host is removed from the mapping table.
---
--- When the redirect is being followed, the same method and body values are being
--- sent in all cases except when 303 status is returned. In that case the method
--- is set to GET and the body is removed before the redirect is followed. Note
--- that if these (method/body) values are provided as table fields, they will be
--- modified in place.
---@param url string
---@param body? string|{ headers: table<string,string>, value: string, method: string, body: string, maxredirects: integer?, keepalive: boolean? }
---@return integer status, table<string,string> headers, string body/
---@nodiscard
---@overload fun(url:string, body?: string|{ headers: table<string,string>, value: string, method: string, body: string, maxredirects?: integer, keepalive: boolean? }): nil, error: string
function Fetch(url, body) end

--- Converts UNIX timestamp to an RFC1123 string that looks like this:
--- `Mon, 29 Mar 2021 15:37:13 GMT`. See `formathttpdatetime.c`.
---@param seconds integer
---@return string
---@nodiscard rfc1123
function FormatHttpDateTime(seconds) end

--- Turns integer like `0x01020304` into a string like `"1.2.3.4"`. See also
--- `ParseIp` for the inverse operation.
---@param uint32 integer
---@return string
---@nodiscard
function FormatIp(uint32) end

--- Returns client ip4 address and port, e.g. `0x01020304`,`31337` would represent
--- `1.2.3.4:31337`. This is the same as `GetClientAddr` except it will use the
--- ip:port from the `X-Forwarded-For` header, only if `IsPrivateIp` or
--- `IsPrivateIp` returns `true`.
---@return integer ip, integer port uint32 and uint16 respectively
---@nodiscard
function GetRemoteAddr() end

---@return string
---@nodiscard # the response message body if present or an empty string. Also
--- returns an empty string during streaming.
function GetResponseBody() end

--- Returns client socket ip4 address and port, e.g. `0x01020304,31337` would
--- represent `1.2.3.4:31337`. Please consider using `GetRemoteAddr` instead,
--- since the latter takes into consideration reverse proxy scenarios.
---@return uint32 ip uint32
---@return uint16 port uint16
---@nodiscard
function GetClientAddr() end

--- Returns address to which listening server socket is bound, e.g.
--- `0x01020304,8080` would represent `1.2.3.4:8080`. If `-p 0` was supplied as
--- the listening port, then the port in this string will be whatever number the
--- operating system assigned.
---@return uint32 ip uint32
---@return uint16 port uint16
---@nodiscard
function GetServerAddr() end

---@return integer clientfd
---@nodiscard
function GetClientFd() end

---@return integer unixts date associated with request that's used to generate the
--- Date header, which is now, give or take a second. The returned value is a UNIX
--- timestamp.
---@nodiscard
function GetDate() end

---@return string?
---@nodiscard
function GetFragment() end

--- Returns HTTP header. name is case-insensitive. The header value is returned as
--- a canonical UTF-8 string, with leading and trailing whitespace trimmed, which
--- was decoded from ISO-8859-1, which is guaranteed to not have C0/C1 control
--- sequences, with the exception of the tab character. Leading and trailing
--- whitespace is automatically removed. In the event that the client suplies raw
--- UTF-8 in the HTTP message headers, the original UTF-8 sequence can be
--- losslessly restored by counter-intuitively recoding the returned string back
--- to Latin1. If the requested header is defined by the RFCs as storing
--- comma-separated values (e.g. Allow, Accept-Encoding) and the field name occurs
--- multiple times in the message, then this function will fold those multiple
--- entries into a single string.
---@param name string
---@return string
---@nodiscard value
function GetHeader(name) end

--- Returns HTTP headers as dictionary mapping header key strings to their UTF-8
--- decoded values. The ordering of headers from the request message is not
--- preserved. Whether or not the same key can repeat depends on whether or not
--- it's a standard header, and if so, if it's one of the ones that the RFCs
--- define as repeatable. See `khttprepeatable.c`. Those headers will not be
--- folded. Standard headers which aren't on that list, will be overwritten with
--- the last-occurring one during parsing. Extended headers are always passed
--- through exactly as they're received. Please consider using `GetHeader` API if
--- possible since it does a better job abstracting these issues.
---@return table<string, string>
---@nodiscard
function GetHeaders() end

--- Returns logger verbosity level. Likely return values are `kLogDebug` >
--- `kLogVerbose` > `kLogInfo` > `kLogWarn` > `kLogError` > `kLogFatal`.
---@return integer
---@nodiscard
function GetLogLevel() end

---@return string # host associated with request. This will be the Host header, if it's supplied. Otherwise it's the bind address.
---@nodiscard
function GetHost() end

---@return "LINUX"|"METAL"|"WINDOWS"|"XNU"|"NETBSD"|"FREEBSD"|"OPENBSD" osname string that describes the host OS.
---@nodiscard
function GetHostOs() end

--- Returns string describing host instruction set architecture.
---
--- This can return:
---
--- - `"X86_64"` for Intel and AMD systems
--- - `"AARCH64"` for ARM64, M1, and Raspberry Pi systems
--- - `"POWERPC64"` for OpenPOWER Raptor Computing Systems
---@return "X86_64"|"AARCH64"|"POWERPC64"
---@nodiscard
function GetHostIsa() end

---@param str string|integer monospace display width of string.
--- This is useful for fixed-width formatting. For example, CJK characters
--- typically take up two cells. This function takes into consideration combining
--- characters, which are discounted, as well as control codes and ANSI escape
--- sequences.
---@return integer
---@nodiscard
function GetMonospaceWidth(str) end

---@return string method HTTP method.
--- Normally this will be GET, HEAD, or POST in which case redbean normalizes this
--- value to its uppercase form. Anything else that the RFC classifies as a "token"
--- string is accepted too, which might contain characters like &".
---@nodiscard
function GetMethod() end

---@return { [1]: string, [2]: string? }[] # name=value parameters from Request-URL and `application/x-www-form-urlencoded` message body in the order they were received.
--- This may contain duplicates. The inner array will have either one or two items,
--- depending on whether or not the equals sign was used.
---@nodiscard
function GetParams() end

---@return string? pass
---@nodiscard
function GetPass() end

---@return string path the Request-URL path.
--- This is guaranteed to begin with `"/"` It is further guaranteed that no `"//"`
--- or `"/."` exists in the path. The returned value is returned as a UTF-8 string
---  which was decoded from ISO-8859-1. We assume that percent-encoded characters
--- were supplied by the client as UTF-8 sequences, which are returned exactly as
--- the client supplied them, and may therefore may contain overlong sequences,
--- control codes, `NUL` characters, and even numbers which have been banned by
--- the IETF. redbean takes those things into consideration when performing path
--- safety checks. It is the responsibility of the caller to impose further
--- restrictions on validity, if they're desired.
---@nodiscard
function GetPath() end

---@return string path as it was resolved by the routing algorithms, which might contain the virtual host prepended if used.
---@nodiscard
function GetEffectivePath() end

---@return uint16 port
---@nodiscard
function GetPort() end

---@return string? scheme from Request-URL, if any.
---@nodiscard
function GetScheme() end

---@return integer? current status (as set by an earlier `SetStatus` call) or `nil` if the status hasn't been set yet.
---@nodiscard
function GetStatus() end

---@return number seconds current time as a UNIX timestamp with 0.0001s precision.
---@nodiscard
function GetTime() end

---@return string url the effective Request-URL as an ASCII string
--- Illegal characters or UTF-8 is guaranteed to be percent encoded, and has been
--- normalized to include either the Host or `X-Forwarded-Host` headers, if they
--- exist, and possibly a scheme too if redbean is being used as an HTTP proxy
--- server.
---
--- In the future this API might change to return an object instead.
---@nodiscard
function GetUrl() end

---@return string? user
---@nodiscard
function GetUser() end

---@return integer httpversion the request HTTP protocol version, which can be `9` for `HTTP/0.9`, `10` for `HTTP/1.0`, or `11` for `HTTP/1.1`.
---@nodiscard
function GetHttpVersion() end

---@return integer
---@nodiscard
---@deprecated Use `GetHttpVersion` instead.
function GetVersion() end

---@param code integer
---@return string reason string describing the HTTP reason phrase. See `gethttpreason.c`.
---@nodiscard
function GetHttpReason(code) end

---@param length integer?
---@return string # with the specified number of random bytes (1..256). If no length is specified, then a string of length 16 is returned.
---@nodiscard
function GetRandomBytes(length) end

---@return integer redbeanversion the Redbean version in the format 0xMMmmpp, with major (MM), minor (mm), and patch (pp) versions encoded. The version value 1.4 would be represented as 0x010400.
---@nodiscard
function GetRedbeanVersion() end

---@param prefix string? paths of all assets in the zip central directory, prefixed by a slash.
--- If prefix parameter is provided, then only paths that start with the prefix
--- (case sensitive) are returned.
---@return string[]
---@nodiscard
function GetZipPaths(prefix) end

---@param name string
---@return boolean # `true` if parameter with name was supplied in either the Request-URL or an application/x-www-form-urlencoded message body.
---@nodiscard
function HasParam(name) end

--- Programs redbean `/` listing page to not display any paths beginning with prefix.
--- This function should only be called from `/.init.lua`.
---@param prefix string
function HidePath(prefix) end

---@param path string
---@return boolean # `true` if the prefix of the given path is set with `HidePath`.
---@nodiscard
function IsHiddenPath(path) end

---@param uint32 integer
---@return boolean # `true` if IP address is not a private network (`10.0.0.0/8`, `172.16.0.0/12`, `192.168.0.0/16`) and is not localhost (`127.0.0.0/8`).
--- Note: we intentionally regard TEST-NET IPs as public.
---@nodiscard
function IsPublicIp(uint32) end

---@param uint32 integer
---@return boolean # `true` if IP address is part of a private network (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
---@nodiscard
function IsPrivateIp(uint32) end

---@return boolean # `true` if the client IP address (returned by GetRemoteAddr) is part of the localhost network (127.0.0.0/8).
---@nodiscard
function IsLoopbackClient() end

---@param uint32 integer
---@return boolean # true if IP address is part of the localhost network (127.0.0.0/8).
---@nodiscard
function IsLoopbackIp(uint32) end

---@param path string
---@return boolean # `true` if ZIP artifact at path is stored on disk using DEFLATE compression.
---@nodiscard
function IsAssetCompressed(path) end

---@deprecated use IsAssetCompressed instead.
---@param path string
---@return boolean
---@nodiscard
function IsCompressed(path) end

---@return boolean
---@nodiscard
function IsClientUsingSsl() end

--- Adds spaces to beginnings of multiline string. If the int parameter is not
--- supplied then 1 space will be added.
---@param str string
---@param int integer?
---@return string
---@nodiscard
function IndentLines(str, int) end

---@param path string
---@return string asset contents of file as string.
--- The asset may be sourced from either the zip (decompressed) or the local
--- filesystem if the `-D` flag was used. If slurping large file into memory is a
--- concern, then consider using `ServeAsset` which can serve directly off disk.
---@nodiscard
function LoadAsset(path) end

--- Stores asset to executable's ZIP central directory. This currently happens in
---  an append-only fashion and is still largely in the proof-of-concept stages.
--- Currently only supported on Linux, XNU, and FreeBSD. In order to use this
--- feature, the `-*` flag must be passed.
---@param path string
---@param data string
---@param mode? integer
function StoreAsset(path, data, mode) end

--- Emits message string to log, if level is less than or equal to `GetLogLevel`.
--- If redbean is running in interactive mode, then this will log to the console.
--- If redbean is running as a daemon or the `-L LOGFILE` flag is passed, then this
--- will log to the file. Reasonable values for level are `kLogDebug` >
--- `kLogVerbose` > `kLogInfo` > `kLogWarn` > `kLogError` > `kLogFatal`. The
--- logger emits timestamps in the local timezone with microsecond precision. If
--- log entries are emitted more frequently than once per second, then the log
--- entry will display a delta timestamp, showing how much time has elapsed since
--- the previous log entry. This behavior is useful for quickly measuring how long
--- various portions of your code take to execute.
---@param level integer
---@param message string
function Log(level, message) end

--- Converts RFC1123 string that looks like this: Mon, 29 Mar 2021 15:37:13 GMT to
--- a UNIX timestamp. See `parsehttpdatetime.c`.
---@param rfc1123 string
---@return integer seconds
---@nodiscard
function ParseHttpDateTime(rfc1123) end

--- Parses URL.
--- 
---@return Url url An object containing the following fields is returned:
--- 
--- - `scheme` is a string, e.g. `"http"`
--- - `user` is the username string, or nil if absent
--- - `pass` is the password string, or nil if absent
--- - `host` is the hostname string, or nil if `url` was a path
--- - `port` is the port string, or nil if absent
--- - `path` is the path string, or nil if absent
--- - `params` is the URL paramaters, e.g. `/?a=b&c` would be
---   represented as the data structure `{{"a", "b"}, {"c"}, ...}`
--- - `fragment` is the stuff after the `#` character
--- 
---@param url string
---@param flags integer? may have:
--- 
--- - `kUrlPlus` to turn `+` into space
--- - `kUrlLatin1` to transcode ISO-8859-1 input into UTF-8
--- 
--- This parser is charset agnostic. Percent encoded bytes are
--- decoded for all fields. Returned values might contain things
--- like NUL characters, spaces, control codes, and non-canonical
--- encodings. Absent can be discerned from empty by checking if
--- the pointer is set.
--- 
--- There's no failure condition for this routine. This is a
--- permissive parser. This doesn't normalize path segments like
--- `.` or `..` so use IsAcceptablePath() to check for those. No 
--- restrictions are imposed beyond that which is strictly
--- necessary for parsing. All the data that is provided will be
--- consumed to the one of the fields. Strict conformance is
--- enforced on some fields more than others, like scheme, since
--- it's the most non-deterministically defined field of them all.
--- 
--- Please note this is a URL parser, not a URI parser. Which
--- means we support everything the URI spec says we should do
--- except for the things we won't do, like tokenizing path
--- segments into an array and then nesting another array beneath
--- each of those for storing semicolon parameters. So this parser
--- won't make SIP easy. What it can do is parse HTTP URLs and most
--- URIs like data:opaque, better in fact than most things which
--- claim to be URI parsers.
---
---@nodiscard
function ParseUrl(url, flags) end

---@param str string
---@return boolean # `true` if path doesn't contain ".", ".." or "//" segments See `isacceptablepath.c`
---@nodiscard
function IsAcceptablePath(str) end

---@param str string
---@return boolean # `true` if path doesn't contain "." or ".." segments See `isreasonablepath.c`
---@nodiscard
function IsReasonablePath(str) end

--- This function is the inverse of ParseUrl. The output will always be correctly
--- formatted. The exception is if illegal characters are supplied in the scheme
--- field, since there's no way of escaping those. Opaque parts are escaped as
--- though they were paths, since many URI parsers won't understand things like
--- an unescaped question mark in path.
---@param url Url
---@return string url
---@nodiscard
function EncodeUrl(url) end

--- Converts IPv4 address string to integer, e.g. "1.2.3.4" → 0x01020304, or
--- returns -1 for invalid inputs. See also `FormatIp` for the inverse operation.
---@param ip string
---@return integer ip
---@nodiscard
function ParseIp(ip) end

---@param path string
---@return string? comment comment text associated with asset in the ZIP central directory.
---@nodiscard
function GetAssetComment(path) end

---@deprecated Use `GetAssetComment` instead.
---@param path string
---@return string?
---@nodiscard
function GetComment(path) end

---@param path string
---@return number seconds UNIX timestamp for modification time of a ZIP asset (or local file if the -D flag is used). If both a file and a ZIP asset are present, then the file is used.
---@nodiscard
function GetAssetLastModifiedTime(path) end

---@deprecated Use `GetAssetLastModifiedTime` instead.
---@param path string
---@return number
---@nodiscard
function GetLastModifiedTime(path) end

---@param path string
---@return integer mode UNIX-style octal mode for ZIP asset (or local file if the -D flag is used)
---@nodiscard
function GetAssetMode(path) end

---@param path string
---@return integer bytesize byte size of uncompressed contents of ZIP asset (or local file if the `-D` flag is used)
---@nodiscard
function GetAssetSize(path) end

---@return string body the request message body if present or an empty string.
---@nodiscard
function GetBody() end

---@return string
---@nodiscard
---@deprecated Use `GetBody` instead.
function GetPayload() end

---@param name string
---@return string cookie
---@nodiscard
function GetCookie(name) end

--- Computes MD5 checksum, returning 16 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Md5(str) end

--- Computes SHA1 checksum, returning 20 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Sha1(str) end

--- Computes SHA224 checksum, returning 28 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Sha224(str) end

--- Computes SHA256 checksum, returning 32 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Sha256(str) end

--- Computes SHA384 checksum, returning 48 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Sha384(str) end

--- Computes SHA512 checksum, returning 64 bytes of binary.
---@param str string
---@return string checksum
---@nodiscard
function Sha512(str) end

---@param name "MD5"|"SHA1"|"SHA224"|"SHA256"|"SHA384"|"SHA512"|"BLAKE2B256"
---@param payload string
---@param key string? If the key is provided, then HMAC value of the same function is returned.
---@return string # value of the specified cryptographic hash function.
---@nodiscard
function GetCryptoHash(name, payload, key) end

--- Configures the address on which to listen. This can be called multiple times
--- to set more than one address. If an integer is provided then it should be a
--- word-encoded IPv4 address, such as the ones returned by `ResolveIp()`. If a
--- string is provided, it will first be passed to ParseIp() to see if it's an
--- IPv4 address. If it isn't, then a HOSTS.TXT lookup is performed, with fallback
--- to the system-configured DNS resolution service. Please note that in MODE=tiny
--- the HOSTS.TXT and DNS resolution isn't included, and therefore an IP must be
--- provided.
---@param ip integer
---@overload fun(host:string)
function ProgramAddr(ip) end

--- Changes HTTP Server header, as well as the `<h1>` title on the `/` listing page.
--- The brand string needs to be a UTF-8 value that's encodable as ISO-8859-1.
--- If the brand is changed to something other than redbean, then the promotional
--- links will be removed from the listing page too. This function should only be
--- called from `/.init.lua`.
---@param str string
function ProgramBrand(str) end

--- Configures Cache-Control and Expires header generation for static asset serving.
--- A negative value will disable the headers. Zero means don't cache. Greater than
--- zero asks public proxies and browsers to cache for a given number of seconds.
--- The directive value is added to the Cache-Control header when specified (with
--- "must-revalidate" provided by default) and can be set to an empty  string to
--- remove the default value.
--- This should only be called from `/.init.lua`.
---@param seconds integer
---@param directive string?
function ProgramCache(seconds, directive) end

--- This function is the same as the -C flag if called from `.init.lua`, e.g.
--- `ProgramCertificate(LoadAsset("/.sign.crt"))` for zip loading or
--- `ProgramCertificate(Slurp("/etc/letsencrypt.lol/fullchain.pem"))` for local
--- file system only. This function is not available in unsecure mode.
---@param pem string
function ProgramCertificate(pem) end

--- Appends HTTP header to the header buffer for all responses (whereas `SetHeader`
--- only appends a header to the current response buffer). name is case-insensitive
--- and restricted to non-space ASCII. value is a UTF-8 string that must be
--- encodable as ISO-8859-1. Leading and trailing whitespace is trimmed
--- automatically. Overlong characters are canonicalized. C0 and C1 control codes
--- are forbidden, with the exception of tab. The header buffer is independent of
--- the payload buffer. This function disallows the setting of certain headers
--- such as Content-Range and Date, which are abstracted by the transport layer.
---@param name string
---@param value string
function ProgramHeader(name, value) end

--- Sets the heartbeat interval (in milliseconds). 5000ms is the default and 100ms is the minimum.
---@param milliseconds integer Negative values `(<0)` sets the interval in seconds.
function ProgramHeartbeatInterval(milliseconds) end

---@param milliseconds integer Negative values `(<0)` sets the interval in seconds.
--- Default timeout is 60000ms. Minimal value of timeout is 10(ms).
--- This should only be called from `/.init.lua`.
function ProgramTimeout(milliseconds) end

--- Hard-codes the port number on which to listen, which can be any number in the
--- range `1..65535`, or alternatively `0` to ask the operating system to choose a
--- port, which may be revealed later on by `GetServerAddr` or the `-z` flag to stdout.
---@param uint16 integer
function ProgramPort(uint16) end

--- Sets the maximum HTTP message payload size in bytes. The
--- default is very conservatively set to 65536 so this is
--- something many people will want to increase. This limit is
--- enforced at the transport layer, before any Lua code is
--- called, because right now redbean stores and forwards
--- messages. (Use the UNIX API for raw socket streaming.) Setting
--- this to a very high value can be useful if you're less
--- concerned about rogue clients and would rather have your Lua
--- code be granted more control to bounce unreasonable messages.
--- If a value less than 1450 is supplied, it'll automatically be
--- increased to 1450, since that's the size of ethernet frames.
--- This function can only be called from `.init.lua`.
---@param int integer
function ProgramMaxPayloadSize(int) end

--- This function is the same as the -K flag if called from .init.lua, e.g.
--- `ProgramPrivateKey(LoadAsset("/.sign.key"))` for zip loading or
--- `ProgramPrivateKey(Slurp("/etc/letsencrypt/privkey.pem"))` for local file
--- system only. This function is not available in unsecure mode.
---@param pem string
function ProgramPrivateKey(pem) end

--- Configures fallback routing for paths which would otherwise return 404 Not
--- Found. If code is `0` then the path is rewritten internally as an accelerated
--- redirect. If code is `301`, `302`, `307`, or `308` then a redirect response
--- will be sent to the client. This should only be called from `/.init.lua`.
---@param code integer
---@param src string
---@param location string
function ProgramRedirect(code, src, location) end

--- Defaults to `86400` (24 hours). This may be set to `≤0` to disable SSL tickets.
--- It's a good idea to use these since it increases handshake performance 10x and
--- eliminates a network round trip. This function is not available in unsecure mode.
---@param seconds integer
function ProgramSslTicketLifetime(seconds) end

--- This function can be used to enable the PSK ciphersuites which simplify SSL
--- and enhance its performance in controlled environments. key may contain 1..32
--- bytes of random binary data and identity is usually a short plaintext string.
--- The first time this function is called, the preshared key will be added to
--- both the client and the server SSL configs. If it's called multiple times,
--- then the remaining keys will be added to the server, which is useful if you
--- want to assign separate keys to each client, each of which needs a separate
--- identity too. If this function is called multiple times with the same identity
--- string, then the latter call will overwrite the prior. If a preshared key is
--- supplied and no certificates or key-signing-keys are programmed, then redbean
--- won't bother auto-generating any serving certificates and will instead use
--- only PSK ciphersuites. This function is not available in unsecure mode.
---@param key string
---@param identity string
function ProgramSslPresharedKey(key, identity) end

--- May be used to disable the verification of certificates
--- for remote hosts when using the Fetch() API. This function is
--- not available in unsecure mode.
---@param enabled boolean
function ProgramSslFetchVerify(enabled) end

--- Enables the verification of certificates supplied by the HTTP clients that
--- connect to your redbean. This has the same effect as the -j flag. Tuning this
--- option alone does not preclude the possibility of unsecured HTTP clients, which
--- can be disabled using ProgramSslRequired(). This function can only be called
--- from `.init.lua`. This function is not available in unsecure mode.
---@param enabled boolean
function ProgramSslClientVerify(enabled) end

--- Enables the blocking of HTTP so that all inbound clients and must use the TLS
--- transport layer. This has the same effect as the -J flag. `Fetch()` is still
--- allowed to make outbound HTTP requests. This function can only be called from
--- `.init.lua`. This function is not available in unsecure mode.
---@param mandatory string
function ProgramSslRequired(mandatory) end

--- This function may be called multiple times to specify the subset of available
--- ciphersuites you want to use in both the HTTPS server and the `Fetch()` client.
--- The default list, ordered by preference, is as follows:
---
--- - ECDHE-ECDSA-AES256-GCM-SHA384
--- - ECDHE-ECDSA-AES128-GCM-SHA256
--- - ECDHE-ECDSA-CHACHA20-POLY1305-SHA256
--- - ECDHE-PSK-AES256-GCM-SHA384
--- - ECDHE-PSK-AES128-GCM-SHA256
--- - ECDHE-PSK-CHACHA20-POLY1305-SHA256
--- - ECDHE-RSA-AES256-GCM-SHA384
--- - ECDHE-RSA-AES128-GCM-SHA256
--- - ECDHE-RSA-CHACHA20-POLY1305-SHA256
--- - DHE-RSA-AES256-GCM-SHA384
--- - DHE-RSA-AES128-GCM-SHA256
--- - DHE-RSA-CHACHA20-POLY1305-SHA256
--- - ECDHE-ECDSA-AES128-CBC-SHA256
--- - ECDHE-RSA-AES256-CBC-SHA384
--- - ECDHE-RSA-AES128-CBC-SHA256
--- - DHE-RSA-AES256-CBC-SHA256
--- - DHE-RSA-AES128-CBC-SHA256
--- - ECDHE-PSK-AES256-CBC-SHA384
--- - ECDHE-PSK-AES128-CBC-SHA256
--- - ECDHE-ECDSA-AES256-CBC-SHA
--- - ECDHE-ECDSA-AES128-CBC-SHA
--- - ECDHE-RSA-AES256-CBC-SHA
--- - ECDHE-RSA-AES128-CBC-SHA
--- - DHE-RSA-AES256-CBC-SHA
--- - DHE-RSA-AES128-CBC-SHA
--- - ECDHE-PSK-AES256-CBC-SHA
--- - ECDHE-PSK-AES128-CBC-SHA
--- - RSA-AES256-GCM-SHA384
--- - RSA-AES128-GCM-SHA256
--- - RSA-AES256-CBC-SHA256
--- - RSA-AES128-CBC-SHA256
--- - RSA-AES256-CBC-SHA
--- - RSA-AES128-CBC-SHA
--- - PSK-AES256-GCM-SHA384
--- - PSK-AES128-GCM-SHA256
--- - PSK-CHACHA20-POLY1305-SHA256
--- - PSK-AES256-CBC-SHA384
--- - PSK-AES128-CBC-SHA256
--- - PSK-AES256-CBC-SHA
--- - PSK-AES128-CBC-SHA
--- - ECDHE-RSA-3DES-EDE-CBC-SHA
--- - DHE-RSA-3DES-EDE-CBC-SHA
--- - ECDHE-PSK-3DES-EDE-CBC-SHA
--- - RSA-3DES-EDE-CBC-SHA
--- - PSK-3DES-EDE-CBC-SHA
---
--- When redbean is run on an old (or low-power) CPU that doesn't have the AES-NI
--- instruction set (Westmere c. 2010) then the default ciphersuite is tuned
--- automatically to favor the ChaCha20 Poly1305 suites.
---
--- The names above are canonical to redbean. They were programmatically simplified
--- from the official IANA names. This function will accept the IANA names too. In
--- most cases it will accept the OpenSSL and GnuTLS naming convention as well.
---
--- This function is not available in unsecure mode.
---@param name string
function ProgramSslCiphersuite(name) end

--- Returns `true` if `-d` flag was passed to redbean.
---@return boolean
---@nodiscard
function IsDaemon() end

--- Same as the `-U` flag if called from `.init.lua` for `setuid()`
function ProgramUid(int) end

--- Same as the `-G` flag if called from `.init.lua` for `setgid()`
---@param int integer
function ProgramGid(int) end

--- Same as the `-D` flag if called from `.init.lua` for overlaying local file
--- system directories. This may be called multiple times. The first directory
--- programmed is preferred. These currently do not show up in the index page listing.
---@param str string
function ProgramDirectory(str) end

--- Same as the `-m` flag if called from `.init.lua` for logging message headers only.
---@param bool boolean
function ProgramLogMessages(bool) end

--- Same as the `-b` flag if called from `.init.lua` for logging message bodies as
--- part of `POST` / `PUT` / etc. requests.
---@param bool boolean
function ProgramLogBodies(bool) end

--- Same as the `-L` flag if called from `.init.lua` for setting the log file path
--- on the local file system. It's created if it doesn't exist. This is called
--- before de-escalating the user / group id. The file is opened in append only
--- mode. If the disk runs out of space then redbean will truncate the log file if
--- has access to change the log file after daemonizing.
---@param str string
function ProgramLogPath(str) end

--- Same as the `-P` flag if called from `.init.lua` for setting the pid file path
--- on the local file system. It's useful for reloading daemonized redbean using
--- `kill -HUP $(cat /var/run/redbean.pid)` or terminating redbean with
--- `kill $(cat /var/run/redbean.pid)` which will gracefully terminate all clients.
--- Sending the `TERM` signal twice will cause a forceful shutdown, which might
--- make someone with a slow internet connection who's downloading big files unhappy.
---@param str string
function ProgramPidPath(str) end

--- Same as the `-u` flag if called from `.init.lua`. Can be used to configure the
--- uniprocess mode. The current value is returned.
---@param bool boolean?
---@return boolean
function ProgramUniprocess(bool) end

--- Reads all data from file the easy way.
---
--- This function reads file data from local file system. Zip file assets can be
--- accessed using the `/zip/...` prefix.
---
--- `i` and `j` may be used to slice a substring in filename. These parameters are
--- 1-indexed and behave consistently with Lua's `string.sub()` API. For example:
---
---     assert(Barf('x.txt', 'abc123'))
---     assert(assert(Slurp('x.txt', 2, 3)) == 'bc')
---
--- This function is uninterruptible so `unix.EINTR` errors will be ignored. This
--- should only be a concern if you've installed signal handlers. Use the UNIX API
--- if you need to react to it.
---
---@param filename string
---@param i integer?
---@param j integer?
---@return string data
---@nodiscard
---@overload fun(filename: string, i?: integer, j?: integer): nil, unix.Errno
function Slurp(filename, i, j) end

--- Writes all data to file the easy way.
---
--- This function writes to the local file system.
---
---@param filename string
---@param data string
---@param mode integer? defaults to 0644. This parameter is ignored when flags doesn't have `unix.O_CREAT`.
---
---@param flags integer? defaults to `unix.O_TRUNC | unix.O_CREAT`.
---
---@param offset integer? is 1-indexed and may be used to overwrite arbitrary slices within a file when used in conjunction with `flags=0`.
--- For example:
---
---     assert(Barf('x.txt', 'abc123'))
---     assert(Barf('x.txt', 'XX', 0, 0, 3))
---     assert(assert(Slurp('x.txt', 1, 6)) == 'abXX23')
---
---@return true
---@overload fun(filename: string, data: string, mode?: integer, flags?: integer, offset?: integer): nil, error: unix.Errno
function Barf(filename, data, mode, flags, offset) end

--- Sleeps the specified number of seconds (can be fractional).
--- The smallest interval is a microsecond.
---@param seconds number
function Sleep(seconds) end

--- Instructs redbean to follow the normal HTTP serving path. This function is
--- useful when writing an OnHttpRequest handler, since that overrides the
--- serving path entirely. So if the handler decides it doesn't want to do
--- anything, it can simply call this function, to hand over control back to the
--- redbean core. By default, the host and path arguments are supplied from the
--- resolved `GetUrl` value. This handler always resolves, since it will generate
--- a 404 Not Found response if redbean couldn't find an appropriate endpoint.
---@param host string?
---@param path string?
function Route(host, path) end

--- This is the same as `Route`, except it only implements the subset of request
--- routing needed for serving virtual-hosted assets, where redbean tries to prefix
--- the path with the hostname when looking up a file. This function returns `true`
--- if the request was resolved. If it was resolved, then your `OnHttpRequest`
--- request handler can still set additional headers.
---@param host string?
---@param path string?
---@return boolean
function RouteHost(host, path) end

--- This is the same as `Route`, except it only implements the subset of request
--- routing needed for serving assets. This function returns `true` if the
--- request was resolved. If it was resolved, then your `OnHttpRequest` request
--- handler can still set additional headers.
---@param path string?
---@return boolean
function RoutePath(path) end

--- Instructs redbean to serve static asset at path. This function causes what
---  would normally happen outside a dynamic handler to happen. The asset can be
--- sourced from either the zip or local filesystem if `-D` is used. This function
--- is mutually exclusive with `SetStatus` and `ServeError`.
---@param path string
function ServeAsset(path) end

--- Instructs redbean to serve a boilerplate error page. This takes care of logging
--- the error, setting the reason phrase, and adding a payload. This function is
--- mutually exclusive with `SetStatus` and `ServeAsset`.
---@param code integer
---@param reason string?
function ServeError(code, reason) end

--- Instructs redbean to return the specified redirect code along with
--- the Location header set. This function is mutually exclusive with
--- `SetStatus` and other `Serve*` functions.
---
---@param code integer
---@param location string
function ServeRedirect(code, location) end

--- Sets logger verbosity. Reasonable values for level are `kLogDebug` >
--- `kLogVerbose` > `kLogInfo` > `kLogWarn` > `kLogError` > `kLogFatal`.
---@param level integer
function SetLogLevel(level) end

--- Replaces C0 control codes and trojan source characters with descriptive
--- UNICODE pictorial representation. This function also canonicalizes overlong
--- encodings. C1 control codes are replaced with a JavaScript-like escape sequence.
---@param str string
---@return string
---@nodiscard
function VisualizeControlCodes(str) end

--- Canonicalizes overlong encodings.
---@param str string
---@return string
---@nodiscard
function Underlong(str) end

---@param x integer
---@return integer # position of first bit set.
--- Passing `0` will raise an error. Same as the Intel x86 instruction BSF.
---@nodiscard
function Bsf(x) end

---@param x integer
---@return integer # binary logarithm of `x`
--- Passing `0` will raise an error. Same as the Intel x86 instruction BSR.
---@nodiscard
function Bsr(x) end

--- Computes Phil Katz CRC-32 used by zip/zlib/gzip/etc.
---@param initial integer
---@param data string
---@return integer
---@nodiscard
function Crc32(initial, data) end

--- Computes 32-bit Castagnoli Cyclic Redundancy Check.
---@param initial integer
---@param data string
---@return integer
---@nodiscard
function Crc32c(initial, data) end

--- Returns number of bits set in integer.
---@param x integer
---@return integer
---@nodiscard
function Popcnt(x) end

--- Returns CPU timestamp counter.
---@return integer
---@nodiscard
function Rdtsc() end

---@return integer # fastest pseudorandom non-cryptographic random number.
--- This linear congruential generator passes practrand and bigcrush.
---@nodiscard
function Lemur64() end

---@return integer # nondeterministic pseudorandom non-cryptographic number.
--- This linear congruential generator passes practrand and bigcrush. This
--- generator is safe across `fork()`, threads, and signal handlers.
---@nodiscard
function Rand64() end

---@return integer # 64-bit hardware random integer from RDRND instruction, with automatic fallback to `getrandom()` if not available.
---@nodiscard
function Rdrand() end

---@return integer # 64-bit hardware random integer from `RDSEED` instruction, with automatic fallback to `RDRND` and `getrandom()` if not available.
---@nodiscard
function Rdseed() end

---@return integer cpucount CPU core count or `0` if it couldn't be determined.
---@nodiscard
function GetCpuCount() end

---@return integer # 0-indexed CPU core on which process is currently scheduled.
---@nodiscard
function GetCpuCore() end

---@return integer # 0-indexed NUMA node on which process is currently scheduled.
---@nodiscard
function GetCpuNode() end

--- Shrinks byte buffer in half using John Costella's magic kernel. This downscales
--- data 2x using an eight-tap convolution, e.g.
---
---     >: Decimate('\xff\xff\x00\x00\xff\xff\x00\x00\xff\xff\x00\x00')
---     "\xff\x00\xff\x00\xff\x00"
---
--- This is very fast if SSSE3 is available (Intel 2004+ / AMD 2011+).
---@param data string
---@return string
---@nodiscard
function Decimate(data) end

---@param data string
---@return number # Shannon entropy of `data`.
--- This gives you an idea of the density of information. Cryptographic random
--- should be in the ballpark of `7.9` whereas plaintext will be more like `4.5`.
---@nodiscard
function MeasureEntropy(data) end

--- Compresses data.
---
---     >: Deflate("hello")
---     "\xcbH\xcd\xc9\xc9\x07\x00"
---     >: Inflate("\xcbH\xcd\xc9\xc9\x07\x00", 5)
---     "hello"
---
--- The output format is raw DEFLATE that's suitable for embedding into formats
--- like a ZIP file. It's recommended that, like ZIP, you also store separately a
--- `Crc32()` checksum in addition to the original uncompressed size.
---
---@param uncompressed string
---@param level integer? the compression level, which defaults to `7`. The max is `9`.
--- Lower numbers go faster (4 for instance is a sweet spot) and higher numbers go
--- slower but have better compression.
---@return string compressed
---@nodiscard
---@overload fun(uncompressed: string, level?: integer): nil, error: string
function Deflate(uncompressed, level) end

--- Decompresses data.
---
--- This function performs the inverse of Deflate(). It's recommended that you
--- perform a `Crc32()` check on the output string after this function succeeds.
---
---@param compressed string
---@param maxoutsize integer the uncompressed size, which should be known.
--- However, it is permissable (although not advised) to specify some large number
--- in which case (on success) the byte length of the output string may be less
--- than `maxoutsize`.
---@return string uncompressed
---@nodiscard
---@overload fun(compressed: string, maxoutsize: integer): nil, error: string
function Inflate(compressed, maxoutsize) end

--- Performs microbenchmark. Nanoseconds are computed from RDTSC tick counts,
--- using an approximation that's measured beforehand with the
--- unix.`clock_gettime()` function. The `ticks` result is the canonical average
--- number of clock ticks. This subroutine will subtract whatever the overhead
--- happens to be for benchmarking a function that does nothing. This overhead
--- value will be reported in the result. `tries` indicates if your microbenchmark
--- needed to be repeated, possibly because your system is under load and the
--- benchmark was preempted by the operating system, or moved to a different core.
---@param func function
---@param count integer?
---@param maxattempts integer?
---@return number nanoseconds the average number of nanoseconds that `func` needed to execute
---@return integer ticks
---@return integer overhead_ticks
---@return integer tries
---@nodiscard
function Benchmark(func, count, maxattempts) end

--- Formats string as octal integer literal string. If the provided value is zero,
--- the result will be `"0"`. Otherwise the resulting value will be the
--- zero-prefixed octal string. The result is currently modulo 2^64. Negative
--- numbers are converted to unsigned.
---@param int integer
---@return string
---@nodiscard
function oct(int) end

--- Formats string as hexadecimal integer literal string. If the provided value is
--- zero, the result will be `"0"`. Otherwise the resulting value will be the
--- "0x"-prefixed hex string. The result is currently modulo 2^64. Negative numbers
--- are converted to unsigned.
---@param int integer
---@return string
---@nodiscard
function hex(int) end

--- Formats string as binary integer literal string. If the provided value is zero,
--- the result will be `"0"`. Otherwise the resulting value will be the
--- "0b"-prefixed binary str. The result is currently modulo 2^64. Negative numbers
--- are converted to unsigned.
---@param int integer
---@return string
---@nodiscard
function bin(int) end

--- Gets IP address associated with hostname.
---
--- This function first checks if hostname is already an IP address, in which case
--- it returns the result of `ParseIp`. Otherwise, it checks HOSTS.TXT on the local
--- system and returns the first IPv4 address associated with hostname. If no such
--- entry is found, a DNS lookup is performed using the system configured (e.g.
--- `/etc/resolv.conf`) DNS resolution service. If the service returns multiple IN
--- A records then only the first one is returned.
---
--- The returned address is word-encoded in host endian order. For example,
--- 1.2.3.4 is encoded as 0x01020304. The `FormatIp` function may be used to turn
--- this value back into a string.
---
--- If no IP address could be found, then `nil` is returned alongside a string of
--- unspecified format describing the error. Calls to this function may be wrapped
--- in `assert()` if an exception is desired.
---@param hostname string
---@return uint32 ip uint32
---@nodiscard
---@overload fun(hostname: string): nil, error: string
function ResolveIp(hostname) end

--- Returns `true` if IP address is trustworthy.
--- If the `ProgramTrustedIp()` function has NOT been called then redbean
--- will consider the networks 127.0.0.0/8, 10.0.0.0/8, 172.16.0.0/12,
--- and 192.168.0.0/16 to be trustworthy too. If `ProgramTrustedIp()` HAS
--- been called at some point earlier in your redbean's lifecycle, then
--- it'll trust the IPs and network subnets you specify instead.
--- The network interface addresses used by the host machine are always
--- considered trustworthy, e.g. 127.0.0.1. This may change soon, if we
--- decide to export a `GetHostIps()` API which queries your NIC devices.
---@param ip integer
---@return boolean
function IsTrustedIp(ip) end

--- Trusts an IP address or network
--- This function may be used to configure the `IsTrustedIp()` function
--- which is how redbean determines if a client is allowed to send us
--- headers like X-Forwarded-For (cf `GetRemoteAddr` vs. `GetClientAddr`)
--- without them being ignored. Trusted IPs is also how redbean turns
--- off token bucket rate limiting selectively, so be careful. Here's
--- an example of how you could trust all of Cloudflare's IPs:
---
---     ProgramTrustedIp(ParseIp("103.21.244.0"), 22);
---     ProgramTrustedIp(ParseIp("103.22.200.0"), 22);
---     ProgramTrustedIp(ParseIp("103.31.4.0"), 22);
---     ProgramTrustedIp(ParseIp("104.16.0.0"), 13);
---     ProgramTrustedIp(ParseIp("104.24.0.0"), 14);
---     ProgramTrustedIp(ParseIp("108.162.192.0"), 18);
---     ProgramTrustedIp(ParseIp("131.0.72.0"), 22);
---     ProgramTrustedIp(ParseIp("141.101.64.0"), 18);
---     ProgramTrustedIp(ParseIp("162.158.0.0"), 15);
---     ProgramTrustedIp(ParseIp("172.64.0.0"), 13);
---     ProgramTrustedIp(ParseIp("173.245.48.0"), 20);
---     ProgramTrustedIp(ParseIp("188.114.96.0"), 20);
---     ProgramTrustedIp(ParseIp("190.93.240.0"), 20);
---     ProgramTrustedIp(ParseIp("197.234.240.0"), 22);
---     ProgramTrustedIp(ParseIp("198.41.128.0"), 17);
---
--- Although you might want consider trusting redbean's open source
--- freedom embracing solution to DDOS protection instead!
---@param ip integer
---@param cidr integer?
function ProgramTrustedIp(ip, cidr) end

--- Enables DDOS protection.
---
--- Imagine you have 2**32 buckets, one for each IP address. Each bucket
--- can hold about 127 tokens. Every second a background worker puts one
--- token in each bucket. When a TCP client socket is opened, it takes a
--- token from its bucket, and then proceeds. If the bucket holds only a
--- third of its original tokens, then redbean sends them a 429 warning.
--- If the client ignores this warning and keeps sending requests, until
--- there's no tokens left, then the banhammer finally comes down.
---
---    function OnServerStart()
---        ProgramTokenBucket()
---        ProgramTrustedIp(ParseIp('x.x.x.x'), 32)
---        assert(unix.setrlimit(unix.RLIMIT_NPROC, 1000, 1000))
---    end
---
--- This model of network rate limiting generously lets people "burst" a
--- tiny bit. For example someone might get a strong craving for content
--- and smash the reload button in Chrome 64 times in a fow seconds. But
--- since the client only get 1 new token per second, they'd better cool
--- their heels for a few minutes after doing that. This amount of burst
--- can be altered by choosing the `reject` / `ignore` / `ban` threshold
--- arguments. For example, if the `reject` parameter is set to 126 then
--- no bursting is allowed, which probably isn't a good idea.
---
--- redbean is programmed to acquire a token immediately after accept()
--- is called from the main server process, which is well before fork()
--- or read() or any Lua code happens. redbean then takes action, based
--- on the token count, which can be accept / reject / ignore / ban. If
--- redbean determines a ban is warrented, then 4-byte datagram is sent
--- to the unix domain socket `/var/run/blackhole.sock` which should be
--- operated using the blackholed program we distribute separately.
---
--- The trick redbean uses on Linux for example is insert rules in your
--- raw prerouting table. redbean is very fast at the application layer
--- so the biggest issue we've encountered in production is are kernels
--- themselves, and programming the raw prerouting table dynamically is
--- how we solved that.
---
--- `replenish` is the number of times per second a token should be
--- added to each bucket. The default value is 1 which means one token
--- is granted per second to all buckets. The minimum value is 1/3600
--- which means once per hour. The maximum value for this setting is
--- 1e6, which means once every microsecond.
--- 
--- `cidr` is the specificity of judgement.  Since creating 2^32 buckets
--- would need 4GB of RAM, redbean defaults this value to 24 which means
--- filtering applies to class c network blocks (i.e. x.x.x.*), and your
--- token buckets only take up 2^24 bytes of RAM (16MB). This can be set
--- to any number on the inclusive interval [8,32], where having a lower
--- number means you use less ram/cpu, but splash damage applies more to
--- your clients; whereas higher numbers means more ram/cpu usage, while
--- ensuring rate limiting only applies to specific compromised actors.
--- 
--- `reject` is the token count or treshold at which redbean should send
--- 429 Too Many Request warnings to the client. Permitted values can be
--- anywhere between -1 and 126 inclusively. The default value is 30 and
--- -1 means disable to disable (assuming AcquireToken() will be used).
--- 
--- `ignore` is the token count or treshold, at which redbean should try
--- simply ignoring clients and close the connection without logging any
--- kind of warning, and without sending any response. The default value
--- for this setting is `MIN(reject / 2, 15)`. This must be less than or
--- equal to the `reject` setting. Allowed values are [-1,126] where you
--- can use -1 as a means of disabling `ignore`.
--- 
--- `ban` is the token count at which redbean should report IP addresses
--- to the blackhole daemon via a unix-domain socket datagram so they'll
--- get banned in the kernel routing tables. redbean's default value for
--- this setting is `MIN(ignore / 10, 1)`. Permitted values are [-1,126]
--- where -1 may be used as a means of disabling the `ban` feature.
--- 
--- This function throws an exception if the constraints described above
--- are not the case. Warnings are logged should redbean fail to connect
--- to the blackhole daemon, assuming it hasn't been disabled. It's safe
--- to use load balancing tools when banning is enabled, since you can't
--- accidentally ban your own network interface addresses, loopback ips,
--- or ProgramTrustedIp() addresses where these rate limits don't apply.
--- 
--- It's assumed will be called from the .init.lua global scope although
--- it could be used in interpreter mode, or from a forked child process
--- in which case the only processes that'll have ability to use it will
--- be that same process, and any descendent processes. This function is
--- only able to be called once.
--- 
--- This feature is not available in unsecure mode.
---@param replenish number?
---@param cidr integer?
---@param reject integer?
---@param ignore integer?
---@param ban integer?
function ProgramTokenBucket(replenish, cidr, reject, ignore, ban) end

--- Atomically acquires token.
---
--- This routine atomically acquires a single token for an `ip` address.
--- The return value is the token count before the subtraction happened.
--- No action is taken based on the count, since the caller will decide.
---
--- `ip` should be an IPv4 address and this defaults to `GetClientAddr()`,
--- although other interpretations of its meaning are possible.
---
--- Your token buckets are stored in shared memory so this can be called
--- from multiple forked processes. which operate on the same values.
---@param ip uint32?
---@return int8
function AcquireToken(ip) end

--- Counts number of tokens in bucket.
--- 
--- This function is the same as AcquireToken() except no subtraction is
--- performed, i.e. no token is taken.
--- 
--- `ip` should be an IPv4 address and this defaults to GetClientAddr(),
--- although other interpretations of its meaning are possible.
---@param ip uint32?
---@return int8
function CountTokens(ip) end

--- Sends IP address to blackholed service.
---
--- `ProgramTokenBucket()` needs to be called beforehand. The default
--- settings will blackhole automatically, during the `accept()` loop
--- based on the banned threshold. However if your Lua code calls
--- `AcquireToken()` manually, then you'll need this function to take
--- action on the returned values.
--- 
--- This function returns true if a datagram could be sent sucessfully.
--- Otherwise false is returned, which can happen if blackholed isn't
--- running, or if a lot of processes are sending messages to it and the
--- operation would have blocked.
--- 
--- It's assumed that the blackholed service is running locally in the
--- background.
---@param ip uint32
function Blackhole(ip) end

-- MODULES

---Please refer to the LuaSQLite3 Documentation.
---
--- For example, you could put the following in your `/.init.lua` file:
---
---     lsqlite3 = require "lsqlite3"
---     db = lsqlite3.open_memory()
---     db:exec[[
---       CREATE TABLE test (
---         id INTEGER PRIMARY KEY,
---         content TEXT
---       );
---       INSERT INTO test (content) VALUES ('Hello World');
---       INSERT INTO test (content) VALUES ('Hello Lua');
---       INSERT INTO test (content) VALUES ('Hello Sqlite3');
---     ]]
---
--- Then, your Lua server pages or OnHttpRequest handler may perform SQL
--- queries by accessing the db global. The performance is good too, at about
--- 400k qps.
---
---     for row in db:nrows("SELECT * FROM test") do
---         Write(row.id.." "..row.content.."<br>")
---     end
---
--- redbean supports a subset of what's defined in the upstream LuaSQLite3
--- project. Most of the unsupported APIs relate to pointers and database
--- notification hooks.

lsqlite3 = {
    -- Error Codes

    --- The `lsqlite3.OK` result code means that the operation was successful
    --- and that there were no errors. Most other result codes indicate an
    --- error.
    OK = 0,
    --- The `lsqlite3.ERROR` result code is a generic error code that is used
    --- when no other more specific error code is available.
    ERROR = 1,
    --- The `lsqlite3.INTERNAL` result code indicates an internal malfunction.
    --- In a working version of SQLite, an application should never see this
    --- result code. If application does encounter this result code, it shows
    --- that there is a bug in the database engine.
    ---
    --- SQLite does not currently generate this result code. However,
    --- application-defined SQL functions or virtual tables, or VFSes, or other
    --- extensions might cause this result code to be returned.
    INTERNAL = 2,
    --- The `lsqlite3.PERM` result code indicates that the requested access mode
    --- for a newly created database could not be provided.
    PERM = 3,
    --- The `lsqlite3.ABORT` result code indicates that an operation was aborted
    --- prior to completion, usually be application request. See also:
    --- `lsqlite3.INTERRUPT`.
    ---
    --- If the callback function to `exec()` returns non-zero, then `exec()`
    --- will return `lsqlite3.ABORT`.
    ---
    --- If a ROLLBACK operation occurs on the same database connection as a
    --- pending read or write, then the pending read or write may fail with an
    --- `lsqlite3.ABORT` error.
    ABORT = 4,
    --- The lsqlite3.BUSY result code indicates that the database file could not
    --- be written (or in some cases read) because of concurrent activity by
    --- some other database connection, usually a database connection in a
    --- separate process.
    ---
    --- For example, if process A is in the middle of a large write transaction
    --- and at the same time process B attempts to start a new write
    --- transaction, process B will get back an `lsqlite3.BUSY` result because
    --- SQLite only supports one writer at a time. Process B will need to wait
    --- for process A to finish its transaction before starting a new
    --- transaction. The `db:busy_timeout()` and `db:busy_handler()` interfaces
    --- are available to process B to help it deal with `lsqlite3.BUSY` errors.
    ---
    --- An `lsqlite3.BUSY` error can occur at any point in a transaction: when
    --- the transaction is first started, during any write or update operations,
    --- or when the transaction commits. To avoid encountering `lsqlite3.BUSY`
    --- errors in the middle of a transaction, the application can use 
    --- `BEGIN IMMEDIATE` instead of just `BEGIN` to start a transaction. The
    --- `BEGIN IMMEDIATE` command might itself return `lsqlite3.BUSY`, but if it
    --- succeeds, then SQLite guarantees that no subsequent operations on the same database through the next COMMIT will return `lsqlite3.BUSY`.
    ---
    --- The `lsqlite3.BUSY` result code differs from `lsqlite3.LOCKED` in that
    --- `lsqlite3.BUSY` indicates a conflict with a separate database
    --- connection, probably in a separate process, whereas `lsqlite3.LOCKED`
    --- indicates a conflict within the same database connection (or sometimes
    --- a database connection with a shared cache).
    BUSY = 5,
    --- The `lsqlite3.LOCKED` result code indicates that a write operation could
    --- not continue because of a conflict within the same database connection
    --- or a conflict with a different database connection that uses a shared
    --- cache.
    ---
    --- For example, a DROP TABLE statement cannot be run while another thread
    --- is reading from that table on the same database connection because
    --- dropping the table would delete the table out from under the concurrent
    --- reader.
    ---
    --- The `lsqlite3.LOCKED` result code differs from `lsqlite3.BUSY` in that
    --- `lsqlite3.LOCKED` indicates a conflict on the same database connection
    --- (or on a connection with a shared cache) whereas `lsqlite3.BUSY`
    --- indicates a conflict with a different database connection, probably in
    --- a different process.
    LOCKED = 6,
    --- The `lsqlite3.NOMEM` result code indicates that SQLite was unable to
    --- allocate all the memory it needed to complete the operation. In other
    --- words, an internal call to `sqlite3_malloc()` or `sqlite3_realloc()` has
    --- failed in a case where the memory being allocated was required in order
    --- to continue the operation.
    NOMEM = 7,
    --- The `lsqlite3.READONLY` result code is returned when an attempt is made
    --- to alter some data for which the current database connection does not
    --- have write permission.
    READONLY = 8,
    --- The `lsqlite3.INTERRUPT` result code indicates that an operation was
    --- interrupted by the `sqlite3_interrupt()` interface. See also:
    --- `lsqlite3.ABORT`
    INTERRUPT = 9,
    --- The `lsqlite3.IOERR` result code says that the operation could not
    --- finish because the operating system reported an I/O error.
    ---
    --- A full disk drive will normally give an `lsqlite3.FULL` error rather
    --- than an `lsqlite3.IOERR` error.
    ---
    --- There are many different extended result codes for I/O errors that
    --- identify the specific I/O operation that failed.
    IOERR = 10,
    --- The `lsqlite3.CORRUPT` result code indicates that the database file has
    --- been corrupted. See [How To Corrupt Your Database Files](https://www.sqlite.org/lockingv3.html#how_to_corrupt)
    --- for further discussion on how corruption can occur.
    CORRUPT = 11,
    --- The `lsqlite3.NOTFOUND` result code is exposed in three ways:
    ---
    --- `lsqlite3.NOTFOUND` can be returned by the `sqlite3_file_control()` 
    --- interface to indicate that the file control opcode passed as the third
    --- argument was not recognized by the underlying VFS.
    ---
    --- `lsqlite3.NOTFOUND` can also be returned by the xSetSystemCall() method
    --- of an sqlite3_vfs object.
    ---
    --- `lsqlite3.NOTFOUND` an be returned by sqlite3_vtab_rhs_value() to
    --- indicate that the right-hand operand of a constraint is not available
    --- to the xBestIndex method that made the call.
    ---
    --- The `lsqlite3.NOTFOUND` result code is also used internally by the
    --- SQLite implementation, but those internal uses are not exposed to the
    --- application.
    NOTFOUND = 12,
    --- The `lsqlite3.FULL` result code indicates that a write could not
    --- complete because the disk is full. Note that this error can occur when
    --- trying to write information into the main database file, or it can also
    --- occur when writing into temporary disk files.
    --- 
    --- Sometimes applications encounter this error even though there is an
    --- abundance of primary disk space because the error occurs when writing
    --- into temporary disk files on a system where temporary files are stored
    --- on a separate partition with much less space that the primary disk.
    FULL = 13,
    --- The `lsqlite3.CANTOPEN` result code indicates that SQLite was unable to
    --- open a file. The file in question might be a primary database file or
    --- one of several temporary disk files.
    CANTOPEN = 14,
    --- The `lsqlite3.PROTOCOL` result code indicates a problem with the file
    --- locking protocol used by SQLite. The `lsqlite3.PROTOCOL` error is
    --- currently only returned when using WAL mode and attempting to start a
    --- new transaction. There is a race condition that can occur when two
    --- separate database connections both try to start a transaction at the
    --- same time in WAL mode. The loser of the race backs off and tries again,
    --- after a brief delay. If the same connection loses the locking race
    --- dozens of times over a span of multiple seconds, it will eventually give
    --- up and return `lsqlite3.PROTOCOL`. The `lsqlite3.PROTOCOL` error should
    --- appear in practice very, very rarely, and only when there are many
    --- separate processes all competing intensely to write to the same
    --- database.
    PROTOCOL = 15,
    --- The `lsqlite3.EMPTY` result code is not currently used.
    EMPTY = 16,
    --- The `lsqlite3.SCHEMA` result code indicates that the database schema has
    --- changed. This result code can be returned from `Statement:step()`. If
    --- the database schema was changed by some other process in between the
    --- time that the statement was prepared and the time the statement was run,
    --- this error can result.
    --- 
    --- The statement is automatically re-prepared if the schema changes, up to
    --- `SQLITE_MAX_SCHEMA_RETRY` times (default: 50). The `step()` interface
    --- will only return `lsqlite3.SCHEMA` back to the application if the
    --- failure persists after these many retries.
    SCHEMA = 17,
    --- The `lsqlite3.TOOBIG` error code indicates that a string or BLOB was too
    --- large. The default maximum length of a string or BLOB in SQLite is 
    --- 1,000,000,000 bytes. This maximum length can be changed at compile-time
    --- using the `SQLITE_MAX_LENGTH` compile-time option. The `lsqlite3.TOOBIG`
    --- error results when SQLite encounters a string or BLOB that exceeds the
    --- compile-time limit.
    --- 
    --- The `lsqlite3.TOOBIG` error code can also result when an oversized SQL
    --- statement is passed into one of the `db:prepare()` interface. The
    --- maximum length of an SQL statement defaults to a much smaller value of
    --- 1,000,000,000 bytes.
    TOOBIG = 18,
    --- The `lsqlite3.CONSTRAINT` error code means that an SQL constraint
    --- violation occurred while trying to process an SQL statement. Additional
    --- information about the failed constraint can be found by consulting the
    --- accompanying error message (returned via `errmsg()`) or by looking at
    --- the extended error code.
    --- 
    --- The `lsqlite3.CONSTRAINT` code can also be used as the return value from
    --- the `xBestIndex()` method of a virtual table implementation. When
    --- `xBestIndex()` returns `lsqlite3.CONSTRAINT`, that indicates that the
    --- particular combination of inputs submitted to `xBestIndex()` cannot
    --- result in a usable query plan and should not be given further
    --- consideration.
    CONSTRAINT = 19,
    --- SQLite is normally very forgiving about mismatches between the type of a
    --- value and the declared type of the container in which that value is to
    --- be stored. For example, SQLite allows the application to store a large
    --- BLOB in a column with a declared type of BOOLEAN. But in a few cases,
    --- SQLite is strict about types. The `lsqlite3.MISMATCH` error is returned
    --- in those few cases when the types do not match.
    --- 
    --- The rowid of a table must be an integer. Attempt to set the rowid to
    --- anything other than an integer (or a NULL which will be automatically
    --- converted into the next available integer rowid) results in an
    --- `lsqlite3.MISMATCH` error.
    MISMATCH = 20,
    --- The `lsqlite3.MISUSE` return code might be returned if the application
    --- uses any SQLite interface in a way that is undefined or unsupported. For
    --- example, using a prepared statement after that prepared statement has
    --- been finalized might result in an `lsqlite3.MISUSE` error.
    ---
    --- SQLite tries to detect misuse and report the misuse using this result
    --- code. However, there is no guarantee that the detection of misuse will
    --- be successful. Misuse detection is probabilistic. Applications should
    --- never depend on an `lsqlite3.MISUSE` return value.
    ---
    --- If SQLite ever returns `lsqlite3.MISUSE` from any interface, that means
    --- that the application is incorrectly coded and needs to be fixed. Do not
    --- ship an application that sometimes returns `lsqlite3.MISUSE` from a
    --- standard SQLite interface because that application contains potentially
    --- serious bugs.
    MISUSE = 21,
    --- The `lsqlite3.NOLFS` error can be returned on systems that do not
    --- support large files when the database grows to be larger than what the
    --- filesystem can handle. "NOLFS" stands for "NO Large File Support".
    NOLFS = 22,
    --- The `lsqlite3.FORMAT` error code is not currently used by SQLite.
    FORMAT = 24,
    --- The `lsqlite3.RANGE` error indices that the parameter number argument to
    --- one of the `bind` routines or the column number in one of the `column`
    --- routines is out of range.
    RANGE = 25,
    --- When attempting to open a file, the `lsqlite3.NOTADB` error indicates
    --- that the file being opened does not appear to be an SQLite database
    --- file.
    NOTADB = 26,
    --- The `lsqlite3.ROW` result code returned by sqlite3_step() indicates that
    --- another row of output is available.
    ROW = 100,
    --- The `lsqlite3.DONE` result code indicates that an operation has
    --- completed. The `lsqlite3.DONE` result code is most commonly seen as a
    --- return value from `step()` indicating that the SQL statement has run to
    --- completion, but `lsqlite3.DONE` can also be returned by other multi-step
    --- interfaces.
    DONE = 101,

    -- Authorizer Action Codes

    CREATE_INDEX = 1,
    CREATE_TABLE = 2,
    CREATE_TEMP_INDEX = 3,
    CREATE_TEMP_TABLE = 4,
    CREATE_TEMP_TRIGGER = 5,
    CREATE_TEMP_VIEW = 6,
    CREATE_TRIGGER = 7,
    CREATE_VIEW = 8,
    DELETE = 9,
    DROP_INDEX = 10,
    DROP_TABLE = 11,
    DROP_TEMP_INDEX = 12,
    DROP_TEMP_TABLE = 13,
    DROP_TEMP_TRIGGER = 14,
    DROP_TEMP_VIEW = 15,
    DROP_TRIGGER = 16,
    DROP_VIEW = 17,
    INSERT = 18,
    PRAGMA = 19,
    READ = 20,
    SELECT = 21,
    TRANSACTION = 22,
    UPDATE = 23,
    ATTACH = 24,
    DETACH = 25,
    ALTER_TABLE = 26,
    REINDEX = 27,
    ANALYZE = 28,
    CREATE_VTABLE = 29,
    DROP_VTABLE = 30,
    FUNCTION = 31,
    SAVEPOINT = 32,

    -- Open Flags

    ---@type integer
    --- The database is created if it does not already exist.
    OPEN_CREATE = nil,
    ---@type integer
    --- The database is opened with shared cache disabled, overriding the
    --- default shared cache setting provided by sqlite3_enable_shared_cache().
    OPEN_PRIVATECACHE = nil,
    ---@type integer
    --- The new database connection will use the "serialized" threading mode.
    --- This means the multiple threads can safely attempt to use the same
    --- database connection at the same time. (Mutexes will block any actual
    --- concurrency, but in this mode there is no harm in trying.)
    OPEN_FULLMUTEX = nil,
    ---@type integer
    --- The new database connection will use the "multi-thread" threading mode.
    --- This means that separate threads are allowed to use SQLite at the same
    --- time, as long as each thread is using a different database connection.
    OPEN_NOMUTEX = nil,
    ---@type integer
    --- The database will be opened as an in-memory database. The database is
    --- named by the "filename" argument for the purposes of cache-sharing, if
    --- shared cache mode is enabled, but the "filename" is otherwise ignored.
    OPEN_MEMORY = nil,
    ---@type integer
    --- The filename can be interpreted as a URI if this flag is set. See
    --- https://www.sqlite.org/c3ref/open.html
    OPEN_URI = nil,
    ---@type integer
    --- The database is opened for reading and writing if possible, or reading
    --- only if the file is write protected by the operating system. In either
    --- case the database must already exist, otherwise an error is returned.
    OPEN_READWRITE = nil,
    ---@type integer
    --- The database is opened in read-only mode. If the database does not
    --- already exist, an error is returned.
    OPEN_READONLY = nil,
    ---@type integer
    --- The database is opened shared cache enabled, overriding the default
    --- shared cache setting provided by sqlite3_enable_shared_cache(). The use
    --- of shared cache mode is discouraged and hence shared cache capabilities
    --- may be omitted from many builds of SQLite. In such cases, this option is
    --- a no-op.
    OPEN_SHAREDCACHE = nil,

    ---@type integer
    TEXT = nil,
    ---@type integer
    BLOB = nil,
    ---@type integer
    NULL = nil,
    ---@type integer
    FLOAT = nil,
}

--- Opens (or creates if it does not exist) an SQLite database with name filename
--- and returns its handle as userdata (the returned object should be used for all
--- further method calls in connection with this specific database, see Database
--- methods). Example:
---
---     myDB = lsqlite3.open('MyDatabase.sqlite3')  -- open
---     -- do some database calls...
---     myDB:close()  -- close
---
--- In case of an error, the function returns `nil`, an error code and an error message.
---
--- Since `0.9.4`, there is a second optional `flags` argument to `lsqlite3.open`.
--- See https://www.sqlite.org/c3ref/open.html for an explanation of these flags and options.
---
---     local db = lsqlite3.open('foo.db', lsqlite3.OPEN_READWRITE + lsqlite3.OPEN_CREATE + lsqlite3.OPEN_SHAREDCACHE)
---
---@param filename string
---@param flags? integer defaults to `lsqlite3.OPEN_READWRITE + lsqlite3.OPEN_CREATE`
---@return lsqlite3.Database db
---@nodiscard
---@overload fun(filename: string, flags?: integer): nil, errorcode: integer, errormsg: string
function lsqlite3.open(filename, flags) end

--- Opens an SQLite database in memory and returns its handle as userdata. In case
--- of an error, the function returns `nil`, an error code and an error message.
--- (In-memory databases are volatile as they are never stored on disk.)
---@return lsqlite3.Database db
---@nodiscard
---@overload fun(): nil, errorcode: integer, errormsg: string
function lsqlite3.open_memory() end

---@return string version lsqlite3 library version information, in the form 'x.y[.z]'.
---@nodiscard
function lsqlite3.lversion() end

---@return string version SQLite version information, in the form 'x.y[.z[.p]]'.
---@nodiscard
function lsqlite3.version() end

---@class lsqlite3.Context: userdata
--- A callback context is available as a parameter inside the callback functions
--- `db:create_aggregate()` and `db:create_function()`. It can be used to get
--- further information about the state of a query.
local Context = nil

---@return any udata the user-definable data field for callback funtions.
---@nodiscard
function Context:get_aggregate_data() end

--- Set the user-definable data field for callback funtions to `udata`.
function Context:set_aggregate_data(udata) end

--- Sets the result of a callback function to `res`. The type of the result
--- depends on the type of `res` and is either a number or a string or `nil`.
--- All other values will raise an error message.
---@param res string|number?
function Context:result(res) end

--- Sets the result of a callback function to the binary string in blob.
---@param blob string
function Context:result_blob(blob) end

--- Sets the result of a callback function to the value number.
---@param double number
function Context:result_double(double) end
Context.result_number = Context.result_double

--- Sets the result of a callback function to the error value in `err`.
function Context:result_error(err) end

--- Sets the result of a callback function to the integer `number`
---@param number integer
function Context:result_int(number) end

--- Sets the result of a callback function to `nil`.
function Context:result_null() end

--- Sets the result of a callback function to the string in `str`.
---@param str string
function Context:result_text(str) end

--- Returns the userdata parameter given in the call to install the callback
--- function (see db:create_aggregate() and db:create_function() for details).
---@return any
function Context:user_data() end

---@class lsqlite3.Database: userdata
--- After opening a database with `lsqlite3.open()` or `lsqlite3.open_memory()`
--- the returned database object should be used for all further method calls in
--- connection with that database.
local Database = {}

---@param changeset string
---@param filter_cb function
---@param conflict_cb function
---@param udata? any
---@param rebaser? lsqlite3.Rebaser
---@param flags? integer
---@return true
---@overload fun(db: lsqlite3.Database, changeset: string, filter_cb: function, conflict_cb: function, udata?, rebaser?: lsqlite3.Rebaser, flags?: integer): nil, errno: integer
---@overload fun(db: lsqlite3.Database, changeset: string, conflict_cb?: function, udata?, rebaser?: lsqlite3.Rebaser, flags?: integer): true
---@overload fun(db: lsqlite3.Database, changeset: string, conflict_cb?: function, udata?, rebaser?: lsqlite3.Rebaser, flags?: integer): nil, errno: integer
---@overload fun(db: lsqlite3.Database, changeset: string, filter_cb: function, conflict_cb: function, udata?): true
---@overload fun(db: lsqlite3.Database, changeset: string, filter_cb: function, conflict_cb: function, udata?): nil, errno: integer
---@overload fun(db: lsqlite3.Database, changeset: string, conflict_cb?: function, udata?): true
---@overload fun(db: lsqlite3.Database, changeset: string, conflict_cb?: function, udata?): nil, errno: integer
function Database:apply_changeset(changeset, conflict_cb, filter_cb, udata, rebaser, flags) end

--- Sets or removes a busy handler for a database.
---@generic Udata
---@param func fun(udata: Udata, tries: integer)? is either a Lua function that implements the busy handler or `nil` to remove a previously set handler. This function returns nothing.
---@param udata? Udata
--- The handler function is called with two parameters: `udata` and the number
--- of (re-)tries for a pending transaction. It should return `nil`, `false` or
--- `0` if the transaction is to be aborted. All other values will result in
--- another attempt to perform the transaction. (See the SQLite documentation
--- for important hints about writing busy handlers.)
function Database:busy_handler(func, udata) end

--- Sets a busy handler that waits for `milliseconds` if a transaction cannot proceed.
--- Calling this function will remove any busy handler set by `db:busy_handler()`;
--- calling it with an argument less than or equal to `0` will turn off all busy handlers.
---@param milliseconds integer
function Database:busy_timeout(milliseconds) end

---@return integer # the number of database rows that were changed (or inserted or deleted) by the most recent SQL statement.
---@nodiscard
--- Only changes that are directly specified by INSERT, UPDATE, or DELETE
--- statements are counted. Auxiliary changes caused by triggers are not
--- counted. Use `db:total_changes()` to find the total number of changes.
function Database:changes() end

--- Closes a database. All SQL statements prepared using `db:prepare()` should
--- have been finalized before this function is called. The function returns
--- `lsqlite3.OK` on success or else a numerical error code.
---@return integer
function Database:close() end

--- Finalizes all statements that have not been explicitly finalized. If
--- `temponly` is `true`, only internal, temporary statements are finalized.
---@param temponly? boolean
function Database:close_vm(temponly) end

--- This function installs a `commit_hook` callback handler.
---@generic Udata
---@param func fun(udata: Udata) a Lua function that is invoked by SQLite3 whenever a transaction is committed. This callback receives one argument:
---@param udata Udata argument used when the callback was installed.
---
--- If `func` returns `false` or `nil` the COMMIT is allowed to proceed, 
--- otherwise the COMMIT is converted to a ROLLBACK.
---
--- See: `db:rollback_hook` and `db:update_hook`
function Database:commit_hook(func, udata) end

--- Concatenate a list of changesets.
---@param changesets string[]
---@return string changeset
function Database:concat_changeset(changesets) end

--- This function creates an aggregate callback function. Aggregates perform an
--- operation over all rows in a query.

---@param name string the name of the aggregate function as given in an SQL statement.
---@param nargs integer the number of arguments this call will provide
---@param step fun(ctx: lsqlite3.Context, ...: string|number|nil) the actual Lua function that gets called once for every row.
--- It should accept a function context (see Methods for callback contexts) plus
--- the same number of parameters as given in `nargs`.
---@param final fun(ctx: lsqlite3.Context) a function that is called once after all rows have been processed.
--- It receives one argument, the function context.
---@param userdata? any If provided, userdata can be any Lua value and would be returned by the `context:user_data()` method.
---
--- The function context can be used inside the two callback functions to
--- communicate with SQLite3. Here is a simple example:
---
---     db:exec[=[
---         CREATE TABLE numbers(num1,num2);
---         INSERT INTO numbers VALUES(1,11);
---         INSERT INTO numbers VALUES(2,22);
---         INSERT INTO numbers VALUES(3,33);
---     ]=]
---     local num_sum=0
---     local function oneRow(context, num)  -- add one column in all rows
---         num_sum = num_sum + num
---     end
---     local function afterLast(context)   -- return sum after last row has been processed
---         context:result_number(num_sum)
---         num_sum = 0
---     end
---     db:create_aggregate("do_the_sums", 1, oneRow, afterLast)
---     for sum in db:urows('SELECT do_the_sums(num1) FROM numbers') do print("Sum of col 1:",sum) end
---     for sum in db:urows('SELECT do_the_sums(num2) FROM numbers') do print("Sum of col 2:",sum) end
---
--- This prints:
---
---     Sum of col 1:   6
---     Sum of col 2:   66
---
---@return boolean success
function Database:create_aggregate(name, nargs, step, final, userdata) end

--- This creates a collation callback. A collation callback is used to establish
--- a collation order, mostly for string comparisons and sorting purposes.
---@param name string the name of the collation to be created
---@param func fun(s1: string, s2: string): -1|0|1 a function that accepts two string arguments, compares them and returns `0` if both strings are identical, `-1` if the first argument is lower in the collation order than the second and `1` if the first argument is higher in the collation order than the second. 
--- A simple example:
---
---    local function collate(s1,s2)
---      s1=s1:lower()
---      s2=s2:lower()
---      if s1==s2 then return 0
---      elseif s1<s2 then return -1
---      else return 1 end
---    end
---    db:exec[=[
---      CREATE TABLE test(id INTEGER PRIMARY KEY,content COLLATE CINSENS);
---      INSERT INTO test VALUES(NULL,'hello world');
---      INSERT INTO test VALUES(NULL,'Buenos dias');
---      INSERT INTO test VALUES(NULL,'HELLO WORLD');
---    ]=]
---    db:create_collation('CINSENS',collate)
---    for row in db:nrows('SELECT * FROM test') do
---      print(row.id, row.content)
---    end
---
function Database:create_collation(name, func) end

--- This function creates a callback function. Callback function are called by
--- SQLite3 once for every row in a query.
---@param name string the name of the aggregate function as given in an SQL statement.
---@param nargs integer the number of arguments this call will provide
---@param func fun(ctx: lsqlite3.Context, ...) the actual Lua function that gets called once for every row.
--- It should accept a function context (see Methods for callback contexts) plus
--- the same number of parameters as given in `nargs`.
---@param userdata? any If provided, userdata can be any Lua value and would be returned by the `context:user_data()` method.
--- Here is an example:
---
---     db:exec'CREATE TABLE test(col1,col2,col3)'
---     db:exec'INSERT INTO test VALUES(1,2,4)'
---     db:exec'INSERT INTO test VALUES(2,4,9)'
---     db:exec'INSERT INTO test VALUES(3,6,16)'
---     db:create_function('sum_cols',3,function(ctx,a,b,c)
---       ctx:result_number(a+b+c)
---     end))
---     for col1,col2,col3,sum in db:urows('SELECT *,sum_cols(col1,col2,col3) FROM test') do
---       util.printf('%2i+%2i+%2i=%2i\n',col1,col2,col3,sum)
---     end
---
---@return boolean success
function Database:create_function(name, nargs, func, userdata) end

---@return lsqlite3.Rebaser
---@overload fun(self: lsqlite3.Database): nil, errno: integer
---@nodiscard
function Database:create_rebaser() end

---@param name string? defaults to `"main"`
---@return lsqlite3.Session
---@overload fun(self: lsqlite3.Database, name?: string): nil, errno: integer
---@nodiscard
function Database:create_session(name) end

---@return string? filename associated with database `name` of connection `db`.
---@nodiscard
---@param name string may be `"main"` for the main database file, or the name specified after the AS keyword in an ATTACH statement for an attached database.
--- If there is no attached database name on the database connection, then no value is
--- returned; if database name is a temporary or in-memory database, then an
--- empty string is returned.
function Database:db_filename(name) end

--- Deserializes data from a string which was created by `db:serialize`.
---@param s string
function Database:deserialize(s) end

---@return integer error the numerical result code (or extended result code) for the most recent failed call associated with database db.
--- See http://lua.sqlite.org/index.cgi/doc/tip/doc/lsqlite3.wiki#numerical_error_and_result_codes for details.
---@nodiscard
function Database:error_code() end

Database.errcode = Database.error_code

---@return string message an error message for the most recent failed call associated with database `db`.
---@nodiscard
function Database:error_message() end

Database.errmsg = Database.error_message

--- Compiles and executes the SQL statement(s) given in string sql. The statements
--- are simply executed one after the other and not stored. The function returns
--- `lsqlite3.OK` on success or else a numerical error code.
---
--- If one or more of the SQL statements are queries, then the callback function
--- specified in func is invoked once for each row of the query result (if func is
--- `nil`, no callback is invoked).
---
--- The callback receives four arguments:
--- -  `udata (the third parameter of the db:exec() call),
--- - the number of columns in the row
--- - a table with the column values
--- - another table with the column names.
---
--- The callback function should return `0`. If the callback returns a non-zero
--- value then the query is aborted, all subsequent SQL statements are skipped
--- and `db:exec()` returns `lsqlite3.ABORT`. Here is a simple example:
---
---     sql=[=[
---         CREATE TABLE numbers(num1,num2,str);
---         INSERT INTO numbers VALUES(1,11,"ABC");
---         INSERT INTO numbers VALUES(2,22,"DEF");
---         INSERT INTO numbers VALUES(3,33,"UVW");
---         INSERT INTO numbers VALUES(4,44,"XYZ");
---         SELECT * FROM numbers;
---     ]=]
---     function showrow(udata,cols,values,names)
---         assert(udata=='test_udata')
---         print('exec:')
---         for i=1,cols do print('',names[i],values[i]) end
---         return 0
---     end
---     db:exec(sql,showrow,'test_udata')
---
---@generic Udata
---@param sql string
---@param func? fun(udata: Udata, cols: integer, values: string[], names: string[]): integer
---@param udata? Udata
function Database:execute(sql, func, udata) end

Database.exec = Database.execute

--- This function causes any pending database operation to abort and return at
--- the next opportunity.
function Database:interrupt() end

---@param changeset string
---@return string
---@nodiscard
function Database:invert_changeset(changeset) end

---@return boolean
---@nodiscard
function Database:isopen() end

---@param name string
---@param flags integer? defaults to `0`
---@return lsqlite3.Iterator
---@overload fun(self: lsqlite3.Database, name?: string, flags?: integer): nil, errno: integer
---@nodiscard
function Database:iterate_changeset(name, flags) end

---@return integer rowid the most recent INSERT into the database. If no inserts have ever occurred, `0` is returned.
--- Each row in an SQLite table has a unique 64-bit signed integer key called
--- the rowid. This id is always available as an undeclared column named ROWID,
--- OID, or _ROWID_. If the table has a column of type INTEGER PRIMARY KEY then
--- that column is another alias for the rowid.
---@nodiscard
---
--- If an INSERT occurs within a trigger, then the rowid of the inserted row is
--- returned as long as the trigger is running. Once the trigger terminates, the
--- value returned reverts to the last value inserted before the trigger fired.
function Database:last_insert_rowid() end

--- Creates an iterator that returns the successive rows selected by the
--- SQL statement given in string `sql`. Each call to the iterator
--- returns a table in which the named fields correspond to the columns
--- in the database. Here is an example:
---
---     db:exec[=[
---         CREATE TABLE numbers(num1,num2);
---         INSERT INTO numbers VALUES(1,11);
---         INSERT INTO numbers VALUES(2,22);
---         INSERT INTO numbers VALUES(3,33);
---     ]=]
---     for a in db:nrows('SELECT * FROM numbers') do table.print(a) end
---
--- This script prints:
---
---     num2: 11
---     num1: 1
---     num2: 22
---     num1: 2
---     num2: 33
---     num1: 3
---
---@param sql string
---@return fun(vm: lsqlite3.VM) iterator, lsqlite3.VM vm
---@nodiscard
function Database:nrows(sql) end

--- This function compiles the SQL statement in string sql into an internal
--- representation and returns this as userdata. The returned object should be
--- used for all further method calls in connection with this specific SQL
--- statement.
--- See http://lua.sqlite.org/index.cgi/doc/tip/doc/lsqlite3.wiki#methods_for_prepared_statements.
---@param sql string
---@return lsqlite3.Statement
---@nodiscard
function Database:prepare(sql) end

--- This function installs a rollback_hook callback handler.
--- See: `db:commit_hook` and `db:update_hook`
---@generic Udata
---@param func fun(udata: Udata) a Lua function that is invoked by SQLite3 whenever a transaction is rolled back. This callback receives one argument: the `udata` argument used when the callback was installed.
---@param udata Udata
function Database:rollback_hook(func, udata) end

--- Creates an iterator that returns the successive rows selected by the SQL
--- statement given in string `sql`. Each call to the iterator returns a table in
--- which the numerical indices 1 to n correspond to the selected columns 1 to n in
--- the database. Here is an example:
---
---     db:exec[=[
---         CREATE TABLE numbers(num1,num2);
---         INSERT INTO numbers VALUES(1,11);
---         INSERT INTO numbers VALUES(2,22);
---         INSERT INTO numbers VALUES(3,33);
---     ]=]
---     for a in db:rows('SELECT * FROM numbers') do table.print(a) end
---
--- This script prints:
---
---     1: 1
---     2: 11
---     1: 2
---     2: 22
---     1: 3
---     2: 33
---
---@param sql string
---@return fun(vm: lsqlite3.VM): (string|number|nil)[]? iterator, lsqlite3.VM vm
---@nodiscard
function Database:rows(sql) end

--- Serialize a database to be restored later with `Database:deserialize`.
---@return string? -- `nil` if the database has no tables
---@nodiscard
function Database:serialize() end

---@return integer # the number of database rows that have been modified by INSERT, UPDATE or DELETE statements since the database was opened.
--- This includes UPDATE, INSERT and DELETE statements executed as part of trigger
--- programs. All changes are counted as soon as the statement that produces them
--- is completed by calling either `stmt:reset()` or `stmt:finalize()`.
---@nodiscard
function Database:total_changes() end

--- This function installs an update_hook Data Change Notification
--- Callback handler. See: `db:commit_hook` and `db:rollback_hook`
---
---@generic Udata
---@param func fun(udata: Udata, op: integer, db: lsqlite3.Database, name: string, rowid: integer) a Lua function that is invoked by SQLite3
--- whenever a row is updated, inserted or deleted. This callback
--- receives five arguments: the first is the `udata` argument used
--- when the callback was installed; the second is an integer
--- indicating the operation that caused the callback to be invoked
--- (one of `lsqlite3.UPDATE`, `lsqlite3.INSERT`, or
--- `lsqlite3.DELETE`). The third and fourth arguments are the
--- database and table name containing the affected row. The final
--- callback parameter is the rowid of the row. In the case of an
--- update, this is the rowid after the update takes place.
---@param udata Udata
function Database:update_hook(func, udata) end

--- Creates an iterator that returns the successive rows selected by the SQL
--- statement given in string sql. Each call to the iterator returns the values
--- that correspond to the columns in the currently selected row.
--- Here is an example:
---
---     db:exec[=[
---         CREATE TABLE numbers(num1,num2);
---         INSERT INTO numbers VALUES(1,11);
---         INSERT INTO numbers VALUES(2,22);
---         INSERT INTO numbers VALUES(3,33);
---     ]=]
---     for num1,num2 in db:urows('SELECT * FROM numbers') do print(num1,num2) end
---
--- This script prints:
---
---     1       11
---     2       22
---     3       33
---
---@param sql string
---@return fun(vm: lsqlite3.VM): ...: string|number|nil iterator, lsqlite3.VM vm
---@nodiscard
function Database:urows(sql) end

---@param mode integer?
---@param name string?
---@return integer nlog, integer nckpt
---@overload fun(self, mode?: integer, name?: integer): nil, errno: integer
function Database:wal_checkpoint(mode, name) end

---@generic Udata
---@param func (fun(udata: Udata, db: lsqlite3.Database, name: string, page_count: integer): integer)?
---@param udata Udata?
function Database:wal_hook(func, udata) end

---@class lsqlite3.Iterator: userdata
--- Returned by `db:iterate_changeset`
local Iterator = nil

---@return (string|number|false|nil)[]
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:conflict() end

---@return integer nout
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:fk_conflicts() end

---@return true
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:finalize() end

---@return integer
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:next() end

---@return (string|number|false?)[]
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:new() end

---@return (string|number|false?)[]
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:old() end

---@return string, integer, boolean indirect
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:op() end

---@return boolean[]
---@overload fun(self: lsqlite3.Iterator): nil, errno: integer
function Iterator:pk() end

---@class lsqlite3.Rebaser: userdata
--- Returned by `db:create_rebaser`.
local Rebaser = nil

function Rebaser:delete() end

---@param changeset string
---@return string
---@overload fun(self: lsqlite3.Rebaser, changeset: string): nil, errno: integer
function Rebaser:rebase(changeset) end

---@class lsqlite3.Session: userdata
--- Returned by `db:create_session`.
local Session = nil

---@generic Udata
---@param filter_cb fun(udata: Udata)?
---@param udata Udata
---@overload fun(self: lsqlite3.Session, filter_cb?: fun(udata), udata?): nil, errno: integer
---@overload fun(self: lsqlite3.Session, s: string): true
---@overload fun(self: lsqlite3.Session, s: string): nil, errno: integer
---@return true
function Session:attach(filter_cb, udata) end

---@return string changeset
---@nodiscard
function Session:changeset() end

--- Closes the session. Further method calls on the session will throw errors.
function Session:delete() end

---@param s1 string
---@param s2 string
---@return boolean
---@nodiscard
function Session:diff(s1, s2) end

---@return boolean
function Session:enable() end

---@return boolean
function Session:indirect() end

---@return boolean
---@nodiscard
function Session:isempty() end

---@return string
---@nodiscard
function Session:patchset() end

---@class lsqlite3.Statement: userdata
--- After creating a prepared statement with `db:prepare()` the returned statement
--- object should be used for all further calls in connection with that statement.
local Statement = nil

--- Binds `value` to statement parameter `n`. If the type of `value` is
--- string it is bound as text. If the type of value is number, it is
--- bound as an integer or double depending on its subtype using
--- `lua_isinteger`. If `value` is a boolean then it is bound as `0` for
--- `false` or `1` for `true`. If `value` is `nil` or missing, any
--- previous binding is removed.
---
---@return integer `lsqlite3.OK` on success or else a numerical error code,
---@param n integer
---@param value string|number|boolean|nil
function Statement:bind(n, value) end

--- Binds string `blob` (which can be a binary string) as a blob to
--- statement parameter `n`.
---
---@param n integer
---@param blob string
---@return integer `lsqlite3.OK` on success or else a numerical error code,
function Statement:bind_blob(n, blob) end

--- Binds the values in `nametable` to statement parameters. If the
--- statement parameters are named (i.e., of the form `":AAA"` or
--- `"$AAA"`) then this function looks for appropriately named fields in
--- nametable; if the statement parameters are not named, it looks for
--- numerical fields 1 to the number of statement parameters.
---
---@return integer `lsqlite3.OK` on success or else a numerical error code,
---@param nametable table
---@return integer
function Statement:bind_names(nametable) end

---@return any # the largest statement parameter index in prepared the statement.
---@nodiscard
--- When the statement parameters are of the forms `":AAA"` or `"?"`, then they are
--- assigned sequentially increasing numbers beginning with one, so the value
--- returned is the number of parameters. However if the same statement parameter
--- name is used multiple times, each occurrence is given the same number, so the
--- value returned is the number of unique statement parameter names.
---
--- If statement parameters of the form `"?NNN"` are used (where `NNN` is an
--- integer) then there might be gaps in the numbering and the value returned by
--- this interface is the index of the statement parameter with the largest index
--- value.
function Statement:bind_parameter_count() end

---@param n integer
---@return string? -- the name of the n-th parameter in prepared statement.
--- Statement parameters of the form `":AAA"` or `"@AAA"` or `"$VVV"` have a name
--- which is the string `":AAA"` or `"@AAA"` or `"$VVV"`. In other words, the
--- initial `":"` or `"$"` or `"@"` is included as part of the name. Parameters of
--- the form `"?"` or `"?NNN"` have no name. The first bound parameter has an index
--- of `1`. If the value `n` is out of range or if the `n`-th parameter is nameless,
--- then `nil` is returned.
function Statement:bind_parameter_name(n) end

--- Binds the given values to statement parameters.
---@param ... string|number|nil
---@return integer `lsqlite3.OK` on success or else a numerical error code,
function Statement:bind_values(...) end

---@return integer cols the number of columns in the result set returned by the statement or `0` if the statement does not return data (for example an `UPDATE`).
---@nodiscard
function Statement:columns() end

--- This function frees the prepared statement.
---@return integer # If the statement was executed successfully, or not executed at all, then `lsqlite3.OK` is returned. If execution of the statement failed then an error code is returned.
function Statement:finalize() end

---@return any # the name of column n in the result set of statement. (The left-most column is number 0.)
---@nodiscard
function Statement:get_name(n) end

---@return table # the names and types of all columns in the result set of the statement.
---@nodiscard
function Statement:get_named_types() end

---@return table # the names and values of all columns in the current result row of a query.
---@nodiscard
function Statement:get_named_values() end

---@return string[] # the names of all columns in the result set returned by the statement.
---@nodiscard
function Statement:get_names() end

---@return any # the type of column n in the result set of statement. (The left-most column is number 0.)
---@nodiscard
---@nodiscard
function Statement:get_type(n) end

---@return any[] # the types of all columns in the result set returned by the statement.
---@nodiscard
function Statement:get_types() end

---@return any[] unames the names of all columns in the result set returned by the statement.
---@nodiscard
function Statement:get_unames() end

---@return any[] utypes the types of all columns in the result set returned by the statement.
---@nodiscard
function Statement:get_utypes() end

---@return any[] uvalues the values of all columns in the current result row of a query.
---@nodiscard
function Statement:get_uvalues() end

---@param n any
---@return any value the value of column n in the result set of the statement. (The left-most column is number 0.)
---@nodiscard
function Statement:get_value(n) end

---@return any[] values the values of all columns in the result set returned by the statement.
---@nodiscard
function Statement:get_values() end

---@return boolean isopen `true` if `stmt` has not yet been finalized, `false` otherwise.
---@nodiscard
function Statement:isopen() end

---@return function iterator iterates over the names and values of the result set of the statement. Each iteration returns a table with the names and values for the current row. This is the prepared statement equivalent of `db:nrows()`.
---@nodiscard
function Statement:nrows() end

--- This function resets the SQL statement, so that it is ready to be re-executed. Any statement variables that had values bound to them using the `stmt:bind*()` functions retain their values.
function Statement:reset() end

---@return function iterator iterates over the values of the result set of statement `stmt`. Each iteration returns an array with the values for the current row. This is the prepared statement equivalent of `db:rows()`.
---@nodiscard
function Statement:rows() end

--- This function must be called to evaluate the (next iteration of the) prepared statement.
---@return integer # one of the following values:
--- - `lsqlite3.BUSY`: the engine was unable to acquire the locks needed.
---   If the statement is a COMMIT or occurs outside of an explicit transaction,
---   then you can retry the statement. If the statement is not a COMMIT and occurs
---   within a explicit transaction then you should rollback the transaction before
---   continuing.
--- - `lsqlite3.DONE`: the statement has finished executing successfully.
---   `stmt:step()` should not be called again on this statement without first
---   calling `stmt:reset()` to reset the virtual machine back to the initial state.
--- - `lsqlite3.ROW`: this is returned each time a new row of data is ready for
---    processing by the caller. The values may be accessed using the column access
---    functions. `stmt:step()` can be called again to retrieve the next
---    row of data.
--- - `lsqlite3.ERROR`: a run-time error (such as a constraint violation) has
---    occurred. `stmt:step()` should not be called again. More
---    information may be found by calling `db:errmsg()`. A more specific error
---    code (can be obtained by calling `stmt:reset()`.
--- - `lsqlite3.MISUSE`: the function was called inappropriately, perhaps because
---    the statement has already been finalized or a previous call to `stmt:step()`
---    has returned `lsqlite3.ERROR` or `lsqlite3.DONE`.
---@nodiscard
function Statement:step() end

---@return function iterator iterates over the values of the result set of the statement.
--- Each iteration returns the values for the current row. This is the prepared
--- statement equivalent of `db:urows()`.
---@nodiscard
function Statement:urows() end

---@return any row_id the rowid of the most recent `INSERT` into the database corresponding to this statement. See `db:last_insert_rowid()`.
---@nodiscard
function Statement:last_insert_rowid() end

---@class lsqlite3.VM: userdata
local VM = nil

---@param index integer
---@param value string|number|boolean|nil
---@return integer errno
function VM:bind(index, value) end

---@param index integer
---@param value string
---@return integer errno
function VM:bind_blob(index, value) end

---@param names string[]
---@return integer errno
function VM:bind_names(names) end

---@return integer parameter_count
---@nodiscard
function VM:bind_parameter_count() end

---@param index number
---@return string parameter_name
---@nodiscard
function VM:bind_parameter_name(index) end

---@param ... string|number|nil
---@return integer errno
function VM:bind_values(...) end

---@return integer columns the column count
---@nodiscard
function VM:columns() end

---@return integer errno
function VM:finalize() end

---@param index integer
---@return string name
---@nodiscard
function VM:get_name(index) end

---@return string[]
---@nodiscard
function VM:get_named_types() end
VM.type = VM.get_named_types

---@return (string|number?)[]
---@nodiscard
function VM:get_named_values() end
VM.data = VM.get_named_values

---@return string[]
---@nodiscard
function VM:get_names() end
VM.inames = VM.get_names

---@param index integer
---@return string
---@nodiscard
function VM:get_type(index) end

---@return string[]
---@nodiscard
function VM:get_types() end
VM.itypes = VM.get_types

---@return string ...
---@nodiscard
function VM:get_unames() end

---@return string ...
---@nodiscard
function VM:get_utypes() end

---@return string|number? ...
---@nodiscard
function VM:get_uvalues() end

---@param index integer
---@return string|number?
---@nodiscard
function VM:get_value(index) end

---@return (string|number?)[]
---@nodiscard
function VM:get_values() end
VM.idata = VM.get_values

---@return boolean
---@nodiscard
function VM:isopen() end

---@return integer rowid
---@nodiscard
function VM:last_insert_rowid() end

---@param sql string
---@return fun(self: lsqlite3.VM): { [string]: string|number } iterator, self
---@nodiscard
function VM:nrows(sql) end

---@return integer errno
function VM:reset() end

---@param sql string
---@return fun(self: lsqlite3.VM): (string|number|nil)[] iterator, self
---@nodiscard
function VM:rows(sql) end

---@return integer
function VM:step() end

---@param sql string
---@return fun(self: lsqlite3.VM): ...: string|number|nil iterator, self
---@nodiscard
function VM:urows(sql) end

--- This module exposes an API for POSIX regular expressions which enable you to
--- validate input, search for substrings, extract pieces of strings, etc.
--- Here's a usage example:
---
---     -- Example IPv4 Address Regular Expression (see also ParseIP)
---     p = assert(re.compile([[^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})---  $]]))
---     m,a,b,c,d = assert(p:search(𝑠))
---     if m then
---       print("ok", tonumber(a), tonumber(b), tonumber(c), tonumber(d))
---     else
---       print("not ok")
---     end
re = {
    --- No match
    NOMATCH = 1,
    --- Invalid regex
    BADPAT = 2,
    --- Unknown collating element
    ECOLLATE = 3,
    --- Unknown character class name
    ECTYPE = 4,
    --- Trailing backslash
    EESCAPE = 5,
    --- Invalid back reference
    ESUBREG = 6,
    --- Missing ]
    EBRACK = 7,
    --- Missing )
    EPAREN = 8,
    --- Missing }
    EBRACE = 9,
    --- Invalid contents of {}
    BADBR = 10,
    --- Invalid character range.
    ERANGE = 11,
    --- Out of memory
    ESPACE = 12,
    --- Repetition not preceded by valid expression
    BADRPT = 13,

    --- Use this flag if you prefer the default POSIX regex syntax.
    --- We use extended regex notation by default. For example, an extended regular
    --- expression for matching an IP address might look like
    --- `([0-9]*)\.([0-9]*)\.([0-9]*)\.([0-9]*)` whereas with basic syntax it would
    --- look like `\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)`.
    --- This flag may only be used with `re.compile` and `re.search`.
    BASIC = 1,
    --- Use this flag if you prefer the default POSIX regex syntax. We use extended
    ---  regex notation by default. For example, an extended regular expression for
    --- matching an IP address might look like `([0-9]*)\.([0-9]*)\.([0-9]*)\.([0-9]*)`
    --- whereas with basic syntax it would look like `\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)`.
    --- This flag may only be used with `re.compile` and `re.search`.
    ICASE = 2,
    --- Use this flag to change the handling of NEWLINE (\x0a) characters. When this
    --- flag is set, (1) a NEWLINE shall not be matched by a "." or any form of a
    --- non-matching list, (2) a "^" shall match the zero-length string immediately
    --- after a NEWLINE (regardless of `re.NOTBOL`), and (3) a "$" shall match the
    --- zero-length string immediately before a NEWLINE (regardless of `re.NOTEOL`).
    NEWLINE = 4,
    --- Causes `re.search` to only report success and failure. This is reported via
    --- the API by returning empty string for success. This flag may only be used
    ---` with `re.compile` and `re.search`.
    NOSUB = 8,
    --- The first character of the string pointed to by string is not the beginning
    --- of the line. This flag may only be used with `re.search` and `regex_t*:search`.
    NOTBOL = 0x0100,
    --- The last character of the string pointed to by string is not the end of the
    --- line. This flag may only be used with `re.search` and `regex_t*:search`.
    NOTEOL = 0x0200,
}

---@class re.Errno: userdata
re.Errno = nil

---@return integer # one of the following
---@nodiscard
--- - `unix.NOMATCH` No match
--- - `unix.BADPAT` Invalid regex
--- - `unix.ECOLLATE` Unknown collating element
--- - `unix.ECTYPE` Unknown character class name
--- - `unix.EESCAPE` Trailing backslash
--- - `unix.ESUBREG` Invalid back reference
--- - `unix.EBRACK` Missing `]`
--- - `unix.EPAREN` Missing `)`
--- - `unix.EBRACE` Missing `}`
--- - `unix.BADBR` Invalid contents of `{}`
--- - `unix.ERANGE` Invalid character range.
--- - `unix.ESPACE` Out of memory
--- - `unix.BADRPT` Repetition not preceded by valid expression
function re.Errno:errno() end

---@return string description the English string describing the error code.
---@nodiscard
function re.Errno:doc() end

---Delegates to `re.Errno:doc()`.
---@return string
---@nodiscard
function re.Errno:__tostring() end

---@class re.Regex: userdata
re.Regex = {}

--- Executes precompiled regular expression.
---
--- Returns nothing (`nil`) if the pattern doesn't match anything. Otherwise it
--- pushes the matched substring and any parenthesis-captured values too. Flags may
--- contain `re.NOTBOL` or `re.NOTEOL` to indicate whether or not text should be
--- considered at the start and/or end of a line.
---
---@param str string
---@param flags? integer defaults to zero and may have any of:
---
--- - `re.NOTBOL`
--- - `re.NOTEOL`
---
--- This has an O(𝑛) cost.
---@return string match, string ... the match, followed by any captured groups
---@nodiscard
---@overload fun(self: re.Regex, text: string, flags?: integer): nil, error: re.Errno
function re.Regex:search(str, flags) end

--- Searches for regular expression match in text.
---
--- This is a shorthand notation roughly equivalent to:
---
---     preg = re.compile(regex)
---     patt = preg:search(re, text)
---
---@param regex string
---@param text string
---@param flags integer? defaults to zero and may have any of:
---
--- - `re.BASIC`
--- - `re.ICASE`
--- - `re.NEWLINE`
--- - `re.NOSUB`
--- - `re.NOTBOL`
--- - `re.NOTEOL`
---
--- This has exponential complexity. Please use `re.compile()` to compile your regular expressions once from `/.init.lua`. This API exists for convenience. This isn't recommended for prod.
---
--- This uses POSIX extended syntax by default.
---@return string match, string ... the match, followed by any captured groups
---@nodiscard
---@overload fun(regex: string, text: string, flags?: integer): nil, error: re.Errno
function re.search(regex, text, flags) end

--- Compiles regular expression.
---
---@param regex string
---@param flags integer? defaults to zero and may have any of:
---
--- - `re.BASIC`
--- - `re.ICASE`
--- - `re.NEWLINE`
--- - `re.NOSUB`
---
--- This has an O(2^𝑛) cost. Consider compiling regular expressions once
--- from your `/.init.lua` file.
---
--- If regex is an untrusted user value, then `unix.setrlimit` should be
--- used to impose cpu and memory quotas for security.
---
--- This uses POSIX extended syntax by default.
---@return re.Regex
---@nodiscard
---@overload fun(regex: string, flags?: integer): nil, error: re.Errno
function re.compile(regex, flags) end

--- The path module may be used to manipulate unix paths.
---
--- Note that we use unix paths on Windows. For example, if you have a
--- path like `C:\foo\bar` then it should be `/c/foo/bar` with redbean.
--- It should also be noted the unix module is more permissive when
--- using Windows paths, where translation to win32 is very light.
path = {}

--- Strips final component of path, e.g.
---
---     path      │ dirname
---     ───────────────────
---     .         │ .
---     ..        │ .
---     /         │ /
---     usr       │ .
---     /usr/     │ /
---     /usr/lib  │ /usr
---     /usr/lib/ │ /usr
---@param str string
---@return string
---@nodiscard
function path.dirname(str) end

--- Returns final component of path, e.g.
---
---     path      │ basename
---     ─────────────────────
---     .         │ .
---     ..        │ ..
---     /         │ /
---     usr       │ usr
---     /usr/     │ usr
---     /usr/lib  │ lib
---     /usr/lib/ │ lib
---@param str string
---@return string
---@nodiscard
function path.basename(str) end

---Concatenates path components, e.g.
---
---     x         │ y        │ joined
---     ─────────────────────────────────
---     /         │ /        │ /
---     /usr      │ lib      │ /usr/lib
---     /usr/     │ lib      │ /usr/lib
---     /usr/lib  │ /lib     │ /lib
---
--- You may specify 1+ arguments.
---
--- Specifying no arguments will raise an error. If `nil` arguments are specified,
--- then they're skipped over. If exclusively `nil` arguments are passed, then `nil`
--- is returned. Empty strings behave similarly to `nil`, but unlike `nil` may
--- coerce a trailing slash.
---@param str string?
---@param ... string?
---@return string?
---@nodiscard
function path.join(str, ...) end

---Returns `true` if path exists.
---This function is inclusive of regular files, directories, and special files.
--- Symbolic links are followed are resolved. On error, `false` is returned.
---@param path string
---@return boolean
---@nodiscard
function path.exists(path) end

---Returns `true` if path exists and is regular file.
---Symbolic links are not followed. On error, `false` is returned.
---@param path string
---@return boolean
---@nodiscard
function path.isfile(path) end

---Returns `true` if path exists and is directory.
---Symbolic links are not followed. On error, `false` is returned.
---@param path string
---@return boolean
---@nodiscard
function path.isdir(path) end

---Returns `true` if path exists and is symbolic link.
---Symbolic links are not followed. On error, `false` is returned.
---@param path string
---@return boolean
---@nodiscard
function path.islink(path) end

--- ### MaxMind
---
--- This module may be used to get city/country/asn/etc from IPs, e.g.
---
---     -- .init.lua
---     maxmind = require 'maxmind'
---     asndb = maxmind.open('/usr/local/share/maxmind/GeoLite2-ASN.mmdb')
---
---     -- request handler
---     as = asndb:lookup(GetRemoteAddr())
---     if as then
---         asnum = as:get('autonomous_system_number')
---         asorg = as:get('autonomous_system_organization')
---         Write(EscapeHtml(asnum))
---         Write(' ')
---         Write(EscapeHtml(asorg))
---     end
---
--- The database file is distributed by MaxMind. You need to sign up on their
--- website to get a free copy. The database has a generalized structure. For a
--- concrete example of how this module may be used, please see `maxmind.lua`
--- in `redbean-demo.com`.
maxmind = {}

---@param filepath string the location of the MaxMind database
---@return maxmind.Db db
---@nodiscard
function maxmind.open(filepath) end

---@class maxmind.Db
maxmind.Db = {}

---@param ip uint32 IPv4 address as uint32
---@return maxmind.Result? result
---@nodiscard
function maxmind.Db:lookup(ip) end

---@class maxmind.Result
maxmind.Result = {}

---@return any
---@nodiscard
function maxmind.Result:get() end

---@return integer
---@nodiscard
function maxmind.Result:netmask() end

--- This is an experimental module that, like the maxmind module, gives you insight
--- into what kind of device is connecting to your redbean. This module can help
--- you protect your redbean because it provides tools for identifying clients that
--- misrepresent themselves. For example the User-Agent header might report itself
--- as a Windows computer when the SYN packet says it's a Linux computer.
---
---    function OnServerListen(fd, ip, port)
---        unix.setsockopt(fd, unix.SOL_TCP, unix.TCP_SAVE_SYN, true)
---        return false
---    end
---
---    function OnClientConnection(ip, port, serverip, serverport)
---        fd = GetClientFd()
---        syn = unix.getsockopt(fd, unix.SOL_TCP, unix.TCP_SAVED_SYN)
---    end
---
---    function OnHttpRequest()
---        Log(kLogInfo, "client is running %s and reports %s" % {
---                finger.GetSynFingerOs(finger.FingerSyn(syn)),
---                GetHeader('User-Agent')})
---        Route()
---    end
---
finger = {}

--- Fingerprints IP+TCP SYN packet.
---
--- This returns a hash-like magic number that reflects the SYN packet structure,
--- e.g. ordering of options, maximum segment size, etc. We make no guarantees this
--- hashing algorithm won't change as we learn more about the optimal way to
---- fingerprint, so be sure to save your syn packets too if you're using this
--- feature, in case they need to be rehashed in the future.
---
--- This function is nil/error propagating.
---@param syn_packet_bytes string
---@return integer synfinger uint32
---@nodiscard
---@overload fun(syn_packet_bytes: string): nil, error: string
---@overload fun(nil: nil, error?: string): nil, error: string?
function finger.FingerSyn(syn_packet_bytes) end

--- Fingerprints IP+TCP SYN packet.
---
--- If synfinger is a known hard-coded magic number, then one of the following
--- strings may be returned:
---
--- - `"LINUX"`
--- - `"WINDOWS"`
--- - `"XNU"`
--- - `"NETBSD"`
--- - `"FREEBSD"`
--- - `"OPENBSD"`
---
--- If this function returns `nil`, then one thing you can do to help is file an
--- issue and share with us your SYN packet specimens. The way we prefer to receive
--- them is in `EncodeLua(syn_packet_bytes)` format along with details on the
--- operating system which you must know.
---@param synfinger integer
---@return "LINUX"|"WINDOWS"|"XNU"|"NETBSD"|"FREEBSD"|"OPENBSD" osname
---@nodiscard
---@overload fun(synfinger: integer): nil, error: string
function finger.GetSynFingerOs(synfinger) end

--- Describes IP+TCP SYN packet.
---
--- The layout looks as follows:
---
--- - `TTL:OPTIONS:WSIZE:MSS`
---
--- The `TTL`, `WSIZE`, and `MSS` fields are unsigned decimal fields.
---
--- The `OPTIONS` field communicates the ordering of the commonly used subset of
--- tcp options. The following character mappings are defined. TCP options not on
--- this list will be ignored.
---
--- - `E`: End of Option list
--- - `N`: No-Operation
--- - `M`: Maximum Segment Size
--- - `K`: Window Scale
--- - `O`: SACK Permitted
--- - `A`: SACK
--- - `e`: Echo (obsolete)
--- - `r`: Echo reply (obsolete)
--- - `T`: Timestamps
---
--- This function is nil/error propagating.
---@param syn_packet_bytes string
---@return string description
---@nodiscard
---@overload fun(syn_packet_bytes: string): nil, error: string
---@overload fun(nil: nil, error?: string): nil, error: string?
function finger.DescribeSyn(syn_packet_bytes) end

--- This module implements a password hashing algorithm based on blake2b that won
--- the Password Hashing Competition.
---
--- It can be used to securely store user passwords in your SQLite database, in a
--- way that destroys the password, but can be verified by regenerating the hash
--- again the next time the user logs in. Destroying the password is important,
--- since if your database is compromised, the bad guys won't be able to use
--- rainbow tables to recover the plain text of the passwords.
---
--- Argon2 achieves this security by being expensive to compute. Care should be
--- taken in choosing parameters, since an HTTP endpoint that uses Argon2 can just
--- as easily become a denial of service vector. For example, you may want to
--- consider throttling your login endpoint.
argon2 = {
    variants = {
        ---@type integer blend of other two methods [default]
        argon2_id = nil,
        ---@type integer maximize resistance to side-channel attacks
        argon2_i = nil,
        ---@type integer maximize resistance to gpu cracking attacks
        argon2_d = nil
    }
}

---@class argon2.Config
argon2.Config = {
    --- The memory hardness in kibibytes, which defaults
    --- to 4096 (4 mibibytes). It's recommended that this be tuned upwards.
    m_cost = nil,

    --- The number of iterations, which defaults to `3`.
    t_cost = nil,

    --- The parallelism factor, which defaults to `1`.
    parallelism = nil,

    --- the number of desired bytes in hash output,
    --- which defaults to 32.
    hash_len = nil,

    ---@type integer `config.variant` may be:
    ---
    --- - `argon2.variants.argon2_id` blend of other two methods [default]
    --- - `argon2.variants.argon2_i` maximize resistance to side-channel attacks
    --- - `argon2.variants.argon2_d` maximize resistance to gpu cracking attacks
    variant = nil
}

--- Hashes password.
---
--- This is consistent with the README of the reference implementation:
---
---     >: assert(argon2.hash_encoded("password", "somesalt", {
---         variant = argon2.variants.argon2_i,
---         hash_len = 24,
---         t_cost = 2,
---     }))
---
---
--- `salt` is a nonce value used to hash the string.
---
--- `config.m_cost` is the memory hardness in kibibytes, which defaults
--- to 4096 (4 mibibytes). It's recommended that this be tuned upwards.
---
--- `config.t_cost` is the number of iterations, which defaults to 3.
---
--- `config.parallelism` is the parallelism factor, which defaults to 1.
---
--- `config.hash_len` is the number of desired bytes in hash output,
--- which defaults to 32.
---
--- `config.variant` may be:
---
--- - `argon2.variants.argon2_id` blend of other two methods [default]
--- - `argon2.variants.argon2_i` maximize resistance to side-channel attacks
--- - `argon2.variants.argon2_d` maximize resistance to gpu cracking attacks
---
---@param pass string
---@param salt string
---@param config argon2.Config
---@return string ascii
---@nodiscard
---@overload fun(pass: string, salt: string, config?: argon2.Config): nil, error: string
function argon2.hash_encoded(pass, salt, config) end

--- Verifies password, e.g.
---
---     >: argon2.verify(
---         "p=4$c29tZXNhbHQ$RdescudvJCsgt3ub+b+dWRWJTmaaJObG",
---     true
---
---@param encoded string
---@param pass string
---@return boolean ok
---@nodiscard
---@overload fun(encoded: string, pass: string): nil, error: string
function argon2.verify(encoded, pass) end

--- This module exposes the low-level System Five system call interface.
--- This module works on all supported platforms, including Windows NT.
unix = {
    --- @type integer
    AF_INET = nil,
    --- @type integer
    AF_UNIX = nil,
    --- @type integer
    AF_UNSPEC = nil,

    --- @type integer Returns maximum length of arguments for new processes.
    ---
    --- This is the character limit when calling `execve()`. It's the sum of
    --- the lengths of `argv` and `envp` including any nul terminators and
    --- pointer arrays. For example to see how much your shell `envp` uses
    ---
    ---     $ echo $(($(env | wc -c) + 1 + ($(env | wc -l) + 1) * 8))
    ---     758
    ---
    --- POSIX mandates this be 4096 or higher. On Linux this it's 128*1024.
    --- On Windows NT it's 32767*2 because CreateProcess lpCommandLine and
    --- environment block are separately constrained to 32,767 characters.
    --- Most other systems define this limit much higher.
    ARG_MAX = nil,

    --- @type integer
    AT_EACCES = nil,
    --- @type integer
    AT_FDCWD = nil,
    --- @type integer
    AT_SYMLINK_NOFOLLOW = nil,

    --- @type integer Returns default buffer size.
    ---
    --- The UNIX module does not perform any buffering between calls.
    ---
    --- Each time a read or write is performed via the UNIX API your redbean
    --- will allocate a buffer of this size by default. This current default
    --- would be 4096 across platforms.
    BUFSIZ = nil,

    --- @type integer Returns the scheduler frequency.
    ---
    --- This is granularity at which the kernel does work. For example, the
    --- Linux kernel normally operates at 100hz so its CLK_TCK will be 100.
    ---
    --- This value is useful for making sense out of unix.Rusage data.
    CLK_TCK = nil,

    --- @type integer
    CLOCK_BOOTTIME = nil,
    --- @type integer
    CLOCK_BOOTTIME_ALARM = nil,
    --- @type integer
    CLOCK_MONOTONIC = nil,
    --- @type integer
    CLOCK_MONOTONIC_COARSE = nil,
    --- @type integer
    CLOCK_MONOTONIC_PRECISE = nil,
    --- @type integer
    CLOCK_MONOTONIC_FAST = nil,
    --- @type integer
    CLOCK_MONOTONIC_RAW = nil,
    --- @type integer
    CLOCK_PROCESS_CPUTIME_ID = nil,
    --- @type integer
    CLOCK_PROF = nil,
    --- @type integer
    CLOCK_REALTIME = nil,
    --- @type integer
    CLOCK_REALTIME_PRECISE = nil,
    --- @type integer
    CLOCK_REALTIME_ALARM = nil,
    --- @type integer
    CLOCK_REALTIME_COARSE = nil,
    --- @type integer
    CLOCK_REALTIME_FAST = nil,
    --- @type integer
    CLOCK_TAI = nil,
    ---@type integer
    CLOCK_THREAD_CPUTIME_ID = nil,
    --- @type integer
    DT_BLK = nil,
    --- @type integer
    DT_CHR = nil,
    --- @type integer
    DT_DIR = nil,
    --- @type integer
    DT_FIFO = nil,
    --- @type integer
    DT_LNK = nil,
    --- @type integer
    DT_REG = nil,
    --- @type integer
    DT_SOCK = nil,
    --- @type integer
    DT_UNKNOWN = nil,

    --- @type integer Argument list too long.
    ---
    --- Raised by `execve`, `sched_setattr`.
    E2BIG = nil,
    --- @type integer Permission denied.
    ---
    --- Raised by `access`, `bind`, `chdir`, `chmod`, `chown`, `chroot`,
    --- `clock_getres`, `connect`, `execve`, `fcntl`, `getpriority`,
    --- `link`, `mkdir`, `mknod`, `mmap`, `mprotect`, `msgctl`, `open`,
    --- `prctl`, `ptrace`, `readlink`, `rename`, `rmdir`, `semget`,
    --- `send`, `setpgid`, `socket`, `stat`, `symlink`, `truncate`,
    --- `unlink`, `uselib`, `utime`, `utimensat`.
    EACCES = nil,
    --- @type integer Address already in use. Raised by `bind`, `connect`, `listen`.
    EADDRINUSE = nil,
    --- @type integer Address not available. Raised by `bind`, `connect`.
    EADDRNOTAVAIL = nil,
    --- @type integer Address family not supported. Raised by `connect`, `socket`, `socketpair`.
    EAFNOSUPPORT = nil,
    --- @type integer
    --- Resource temporarily unavailable (e.g. SO_RCVTIMEO expired, too many
    --- processes, too much memory locked, read or write with O_NONBLOCK
    --- needs polling, etc.).
    ---
    --- Raised by `accept`, `connect`, `fcntl`, `fork`, `getrandom`,
    --- `mincore`, `mlock`, `mmap`, `mremap`, `poll`, `read`, `select`,
    --- `send`, `setresuid`, `setreuid`, `setuid`, `sigwaitinfo`,
    --- `splice`, `tee`, `timer_create`, `timerfd_create`, `tkill`,
    --- `write`,
    EAGAIN = nil,
    --- @type integer Connection already in progress. Raised by `connect`, `send`.
    EALREADY = nil,
    --- @type integer Bad file descriptor; cf. EBADFD.
    ---
    --- Raised by `accept`, `access`, `bind`, `chdir`, `chmod`, `chown`,
    --- `close`, `connect`, `copy_file_range`, `dup`, `fcntl`, `flock`,
    --- `fsync`, `futimesat`, `opendir`, `getpeername`, `getsockname`,
    --- `getsockopt`, `ioctl`, `link`, `listen`, `lseek`, `mkdir`,
    --- `mknod`, `mmap`, `open`, `prctl`, `read`, `readahead`,
    --- `readlink`, `recv`, `rename`, `select`, `send`, `shutdown`,
    --- `splice`, `stat`, `symlink`, `sync`, `sync_file_range`,
    --- `timerfd_create`, `truncate`, `unlink`, `utimensat`, `write`.
    EBADF = nil,
    --- @type integer
    EBADFD = nil,
    --- @type integer
    EBADMSG = nil,
    --- @type integer Device or resource busy.
    ---
    --- Raised by dup, fcntl, msync, prctl, ptrace, rename,
    --- rmdir.
    EBUSY = nil,
    --- @type integer
    ECANCELED = nil,
    --- @type integer No child process.
    ---
    --- Raised by `wait`, `waitpid`, `waitid`, `wait3`, `wait4`.
    ECHILD = nil,
    --- @type integer Connection reset before accept. Raised by `accept`.
    ECONNABORTED = nil,
    --- @type integer System-imposed limit on the number of threads was encountered.
    ---
    --- Raised by connect, listen, recv.
    ECONNREFUSED = nil,
    --- @type integer Connection reset by client. Raised by `send`.

    ECONNRESET = nil,
    --- @type integer Resource deadlock avoided.
    ---
    --- Raised by `fcntl`.
    EDEADLK = nil,
    --- @type integer Destination address required. Raised by `send`, `write`.
    EDESTADDRREQ = nil,
    --- @type integer
    EDOM = nil,
    --- @type integer Disk quota exceeded.
    ---
    --- Raised by link, mkdir, mknod, open, rename, symlink,
    --- write.
    EDQUOT = nil,
    --- @type integer File exists.
    ---
    --- Raised by `link`, `mkdir`, `mknod`, `mmap`, `open`, `rename`,
    --- `rmdir`, `symlink`.
    EEXIST = nil,
    --- @type integer
    EFAULT = nil,
    --- @type integer File too large.
    ---
    --- Raised by `copy_file_range`, `open`, `truncate`, `write`.
    EFBIG = nil,
    --- @type integer Host is down. Raised by `accept`.
    EHOSTDOWN = nil,
    --- @type integer Host is unreachable. Raised by `accept`.
    EHOSTUNREACH = nil,
    --- @type integer Identifier removed. Raised by `msgctl`.
    EIDRM = nil,
    --- @type integer
    EILSEQ = nil,
    --- @type integer
    EINPROGRESS = nil,
    --- @type integer The greatest of all errnos; crucial for building real time reliable software.
    ---
    --- Raised by `accept`, `clock_nanosleep`, `close`, `connect`, `dup`, `fcntl`,
    --- `flock`, `getrandom`, `nanosleep`, `open`, `pause`, `poll`, `ptrace`, `read`, `recv`,
    --- `select`, `send`, `sigsuspend`, `sigwaitinfo`, `truncate`, `wait`, `write`.
    EINTR = nil,
    --- @type integer Invalid argument.
    --- Raised by [pretty much everything].
    EINVAL = nil,
    --- @type integer
    --- Raised by `access`, `acct`, `chdir`, `chmod`, `chown`, `chroot`, `close`,
    --- `copy_file_range`, `execve`, `fallocate`, `fsync`, `ioperm`, `link`, `madvise`,
    --- `mbind`, `pciconfig_read`, `ptrace`, `read`, `readlink`, `sendfile`, `statfs`,
    --- `symlink`, `sync_file_range`, `truncate`, `unlink`, `write`.
    EIO = nil,
    --- @type integer Socket is connected. Raised by `connect`, `send`.

    EISCONN = nil,
    --- @type integer Is a directory.
    ---
    --- Raised by `copy_file_range`, `execve`, `open`, `read`, `rename`, `truncate`,
    --- `unlink`.
    EISDIR = nil,
    --- @type integer Too many levels of symbolic links.
    ---
    --- Raised by access, bind, chdir, chmod, chown, chroot, execve, link,
    --- mkdir, mknod, open, readlink, rename, rmdir, stat, symlink,
    --- truncate, unlink, utimensat.
    ELOOP = nil,
    --- @type integer Too many open files.
    ---
    --- Raised by `accept`, `dup`, `execve`, `fanotify_init`, `fcntl`,
    --- `open`, `pipe`, `socket`, `socketpair`, `timerfd_create`.
    EMFILE = nil,
    --- @type integer Too many links;
    ---
    --- Raised by `link`, `mkdir`, `rename`.
    EMLINK = nil,
    --- @type integer Message too long. Raised by `send`.
    EMSGSIZE = nil,
    --- @type integer Filename too long. Cosmopolitan Libc currently defines `PATH_MAX` as
    --- 1024 characters. On UNIX that limit should only apply to system call
    --- wrappers like realpath. On Windows NT it's observed by all system
    --- calls that accept a pathname.
    ---
    --- Raised by `access`, `bind`, `chdir`, `chmod`, `chown`, `chroot`,
    --- `execve`, `gethostname`, `link`, `mkdir`, `mknod`, `open`,
    --- `readlink`, `rename`, `rmdir`, `stat`, `symlink`, `truncate`,
    --- `unlink`, `utimensat`.
    ENAMETOOLONG = nil,
    --- @type integer Network is down. Raised by `accept`.
    ENETDOWN = nil,
    --- @type integer Connection reset by network.
    ENETRESET = nil,
    --- @type integer Host is unreachable. Raised by `accept`, `connect`.
    ENETUNREACH = nil,
    --- @type integer Too many open files in system.
    ---
    --- Raised by `accept`, `execve`, `mmap`, `open`, `pipe`, `socket`,
    --- `socketpair`, `swapon`, `timerfd_create`, `uselib`,
    --- `userfaultfd`.
    ENFILE = nil,
    --- @type integer No buffer space available;
    ---
    --- Raised by `getpeername`, `getsockname`, `send`.
    ENOBUFS = nil,
    --- @type integer No message is available in xsi stream or named pipe is being closed;
    --- no data available; barely in posix; returned by ioctl; very close in
    --- spirit to EPIPE?
    ENODATA = nil,
    --- @type integer No such device.
    ---
    --- Raised by `arch_prctl`, `mmap`, `open`, `prctl`, `timerfd_create`.
    ENODEV = nil,
    --- @type integer No such file or directory.
    ---
    --- Raised by `access`, `bind`, `chdir`, `chmod`, `chown`, `chroot`,
    --- `clock_getres`, `execve`, `opendir`, `link`, `mkdir`, `mknod`,
    --- `open`, `readlink`, `rename`, `rmdir`, `stat`, `swapon`,
    --- `symlink`, `truncate`, `unlink`, `utime`, `utimensat`.
    ENOENT = nil,
    --- @type integer Exec format error. Raised by `execve`, `uselib`.
    ENOEXEC = nil,
    --- @type integer No locks available. Raised by `fcntl`, `flock`.
    ENOLCK = nil,
    --- @type integer
    ENOMEM = nil,
    --- @type integer Raised by `msgop`.
    ENOMSG = nil,
    --- @type integer
    ENONET = nil,
    --- @type integer Protocol not available. Raised by `getsockopt`, `accept`.
    ENOPROTOOPT = nil,
    --- @type integer No space left on device.
    ---
    --- Raised by `copy_file_range`, `fsync`, `link`, `mkdir`, `mknod`,
    --- `open`, `rename`, `symlink`, `sync_file_range`, `write`.
    ENOSPC = nil,
    --- @type integer System call not available on this platform. On
    ---     Windows this is raised by `chroot`, `setuid`, `setgid`,
    ---     `getsid`, `setsid`, and others we're doing our best to
    ---     document.
    ENOSYS = nil,
    --- @type integer Block device required. Raised by `umount`.
    ENOTBLK = nil,
    --- @type integer Socket is not connected.
    ---
    --- Raised by `getpeername`, `recv`, `send`, `shutdown`.
    ENOTCONN = nil,
    --- @type integer Not a directory. This means that a directory
    ---     component in a supplied path *existed* but wasn't a
    ---     directory. For example, if you try to `open("foo/bar")` and
    ---     `foo` is a regular file, then `ENOTDIR` will be returned.
    ---
    --- Raised by `open`, `access`, `chdir`, `chroot`, `execve`, `link`,
    --- `mkdir`, `mknod`, `opendir`, `readlink`, `rename`, `rmdir`,
    --- `stat`, `symlink`, `truncate`, `unlink`, `utimensat`, `bind`,
    --- `chmod`, `chown`, `fcntl`, `futimesat`.
    ENOTDIR = nil,
    --- @type integer Directory not empty. Raised by `rmdir`.
    ENOTEMPTY = nil,
    --- @type integer
    ENOTRECOVERABLE = nil,
    --- @type integer Not a socket.
    ---
    --- Raised by `accept`, `bind`, `connect`, `getpeername`,
    --- `getsockname`, `getsockopt`, `listen`, `recv`, `send`,
    --- `shutdown`.
    ENOTSOCK = nil,
    --- @type integer Operation not supported.
    ---
    --- Raised by `chmod`, `clock_getres`, `clock_nanosleep`,
    --- `timer_create`.
    ENOTSUP = nil,
    --- @type integer Inappropriate i/o control operation. Raised by `ioctl`.
    ENOTTY = nil,
    --- @type integer No such device or address. Raised by `lseek`, `open`, `prctl`.
    ENXIO = nil,
    --- @type integer Socket operation not supported.
    ---
    --- Raised by accept, listen, mmap, prctl, readv, send,
    --- socketpair.
    EOPNOTSUPP = nil,
    --- @type integer Raised by `copy_file_range`, `fanotify_init`, `lseek`, `mmap`,
    --- `open`, `stat`, `statfs`
    EOVERFLOW = nil,
    --- @type integer
    EOWNERDEAD = nil,
    --- @type integer Operation not permitted.
    ---
    --- Raised by `accept`, `chmod`, `chown`, `chroot`,
    --- `copy_file_range`, `execve`, `fallocate`, `fanotify_init`,
    --- `fcntl`, `futex`, `get_robust_list`, `getdomainname`,
    --- `getgroups`, `gethostname`, `getpriority`, `getrlimit`,
    --- `getsid`, `gettimeofday`, `idle`, `init_module`, `io_submit`,
    --- `ioctl_console`, `ioctl_ficlonerange`, `ioctl_fideduperange`,
    --- `ioperm`, `iopl`, `ioprio_set`, `keyctl`, `kill`, `link`,
    --- `lookup_dcookie`, `madvise`, `mbind`, `membarrier`,
    --- `migrate_pages`, `mkdir`, `mknod`, `mlock`, `mmap`, `mount`,
    --- `move_pages`, `msgctl`, `nice`, `open`, `open_by_handle_at`,
    --- `pciconfig_read`, `perf_event_open`, `pidfd_getfd`,
    --- `pidfd_send_signal`, `pivot_root`, `prctl`, `process_vm_readv`,
    --- `ptrace`, `quotactl`, `reboot`, `rename`, `request_key`,
    --- `rmdir`, `rt_sigqueueinfo`, `sched_setaffinity`,
    --- `sched_setattr`, `sched_setparam`, `sched_setscheduler`,
    --- `seteuid`, `setfsgid`, `setfsuid`, `setgid`, `setns`, `setpgid`,
    --- `setresuid`, `setreuid`, `setsid`, `setuid`, `setup`,
    --- `setxattr`, `sigaltstack`, `spu_create`, `stime`, `swapon`,
    --- `symlink`, `syslog`, `truncate`, `unlink`, `utime`, `utimensat`,
    --- `write`.
    EPERM = nil,
    --- @type integer Protocol family not supported.
    EPFNOSUPPORT = nil,
    --- @type integer Broken pipe.
    --- Returned by `write`, `send`. This happens when you try
    --- to write data to a subprocess via a pipe but the reader end has
    --- already closed, possibly because the process died. Normally i/o
    --- routines only return this if `SIGPIPE` doesn't kill the process.
    --- Unlike default UNIX programs, redbean currently ignores `SIGPIPE` by
    --- default, so this error code is a distinct possibility when pipes or
    --- sockets are being used.
    EPIPE = nil,
    --- @type integer Raised by `accept`, `connect`, `socket`, `socketpair`.
    EPROTO = nil,
    --- @type integer Protocol not supported. Raised by `socket`, `socketpair`.
    EPROTONOSUPPORT = nil,
    --- @type integer Protocol wrong type for socket. Raised by `connect`.
    EPROTOTYPE = nil,
    --- @type integer Result too large.
    ---
    --- Raised by `prctl`.
    ERANGE = nil,
    --- @type integer
    EREMOTE = nil,
    --- @type integer
    ERESTART = nil,
    --- @type integer Read-only filesystem.
    ---
    --- Raised by access, bind, chmod, chown, link, mkdir, mknod, open,
    --- rename, rmdir, symlink, truncate, unlink, utime, utimensat.
    EROFS = nil,
    --- @type integer Cannot send after transport endpoint shutdown; note that shutdown write is an `EPIPE`.
    ESHUTDOWN = nil,
    --- @type integer Socket type not supported.
    ESOCKTNOSUPPORT = nil,
    --- @type integer Invalid seek.
    ---
    --- Raised by `lseek`, `splice`, `sync_file_range`.
    ESPIPE = nil,
    --- @type integer No such process.
    ---
    --- Raised by `getpriority`, `getrlimit`, `getsid`, `ioprio_set`, `kill`, `setpgid`, `tkill`, `utimensat`.
    ESRCH = nil,
    --- @type integer
    ESTALE = nil,
    --- @type integer Timer expired. Raised by `connect`.
    ETIME = nil,
    --- @type integer Connection timed out. Raised by `connect`.
    ETIMEDOUT = nil,
    --- @type integer Too many references: cannot splice. Raised by `sendmsg`.
    ETOOMANYREFS = nil,
    --- @type integer Won't open executable that's executing in write mode.
    ---
    --- Raised by access, copy_file_range, execve, mmap, open, truncate.
    ETXTBSY = nil,
    --- @type integer
    EUSERS = nil,

    --- @type integer Improper link.
    ---
    --- Raised by copy_file_range, link, rename.
    EXDEV = nil,

    --- @type integer
    FD_CLOEXEC = nil,

    --- @type integer
    F_GETFD = nil,
    --- @type integer
    F_GETFL = nil,
    --- @type integer
    F_OK = nil,
    --- @type integer
    F_RDLCK = nil,
    --- @type integer
    F_SETFD = nil,
    --- @type integer
    F_SETFL = nil,
    --- @type integer
    F_SETLK = nil,
    --- @type integer
    F_SETLKW = nil,
    --- @type integer
    F_UNLCK = nil,
    --- @type integer
    F_WRLCK = nil,

    --- @type integer
    IPPROTO_ICMP = nil,
    --- @type integer
    IPPROTO_IP = nil,
    --- @type integer
    IPPROTO_RAW = nil,
    --- @type integer
    IPPROTO_TCP = nil,
    --- @type integer
    IPPROTO_UDP = nil,

    --- @type integer
    IP_HDRINCL = nil,
    --- @type integer
    IP_MTU = nil,
    --- @type integer
    IP_TOS = nil,
    --- @type integer
    IP_TTL = nil,

    --- @type integer
    ITIMER_PROF = nil,
    --- @type integer
    ITIMER_REAL = nil,
    --- @type integer
    ITIMER_VIRTUAL = nil,

    --- @type integer
    LOG_ALERT = nil,
    --- @type integer
    LOG_CRIT = nil,
    --- @type integer
    LOG_DEBUG = nil,
    --- @type integer
    LOG_EMERG = nil,
    --- @type integer
    LOG_ERR = nil,
    --- @type integer
    LOG_INFO = nil,
    --- @type integer
    LOG_NOTICE = nil,
    --- @type integer
    LOG_WARNING = nil,

    --- @type integer
    MSG_DONTROUTE = nil,
    --- @type integer
    MSG_MORE = nil,
    --- @type integer
    MSG_NOSIGNAL = nil,
    --- @type integer
    MSG_OOB = nil,
    --- @type integer
    MSG_PEEK = nil,
    --- @type integer
    MSG_WAITALL = nil,

    --- @type integer  Returns maximum length of file path component.
    ---
    --- POSIX requires this be at least 14. Most operating systems define it
    --- as 255. It's a good idea to not exceed 253 since that's the limit on
    --- DNS labels.
    NAME_MAX = nil,

    --- @type integer Returns maximum number of signals supported by underlying system.
    ---
    --- The limit for unix.Sigset is 128 to support FreeBSD, but most
    --- operating systems define this much lower, like 32. This constant
    --- reflects the value chosen by the underlying operating system.
    NSIG = nil,

    --- @type integer open for reading (default)
    O_RDONLY = nil,
    --- @type integer open for writing
    O_WRONLY = nil,
    --- @type integer open for reading and writing
    O_RDWR = nil,

    --- @type integer create file if it doesn't exist
    O_CREAT = nil,
    --- @type integer automatic `ftruncate(fd, 0)` if exists
    O_TRUNC = nil,
    --- @type integer automatic `close()` upon `execve()`
    O_CLOEXEC = nil,
    --- @type integer exclusive access (see below)
    O_EXCL = nil,
    --- @type integer open file for append only
    O_APPEND = nil,
    --- @type integer asks read/write to fail with EAGAIN rather than block
    O_NONBLOCK = nil,
    --- @type integer it's complicated (not supported on Apple and OpenBSD)
    O_DIRECT = nil,
    --- @type integer useful for stat'ing (hint on UNIX but required on NT)
    O_DIRECTORY = nil,
    --- @type integer try to make temp more secure (Linux and Windows only)
    O_TMPFILE = nil,
    --- @type integer fail if it's a symlink (zero on Windows)
    O_NOFOLLOW = nil,
    --- @type integer it's complicated (zero on non-Linux/Apple)
    O_DSYNC = nil,
    --- @type integer it's complicated (zero on non-Linux/Apple)
    O_RSYNC = nil,
    --- @type integer it's complicated (zero on non-Linux)
    O_PATH = nil,
    --- @type integer it's complicated (zero on non-FreeBSD)
    O_VERIFY = nil,
    --- @type integer it's complicated (zero on non-BSD)
    O_SHLOCK = nil,
    --- @type integer it's complicated (zero on non-BSD)
    O_EXLOCK = nil,
    --- @type integer don't record access time (zero on non-Linux)
    O_NOATIME = nil,
    --- @type integer hint random access intent (zero on non-Windows)
    O_RANDOM = nil,
    --- @type integer hint sequential access intent (zero on non-Windows)
    O_SEQUENTIAL = nil,
    --- @type integer ask fs to abstract compression (zero on non-Windows)
    O_COMPRESSED = nil,
    --- @type integer turns on that slow performance (zero on non-Windows)
    O_INDEXED = nil,

    --- @type integer
    O_ACCMODE = nil,
    --- @type integer
    O_ASYNC = nil,
    --- @type integer
    O_EXEC = nil,
    --- @type integer
    O_NOCTTY = nil,
    --- @type integer
    O_SEARCH = nil,
    --- @type integer
    O_SYNC = nil,

    --- @type integer Returns maximum length of file path.
    ---
    --- This applies to a complete path being passed to system calls.
    ---
    --- POSIX.1 XSI requires this be at least 1024 so that's what most
    --- platforms support. On Windows NT, the limit is technically 260
    --- characters. Your redbean works around that by prefixing `//?/`
    --- to your paths as needed. On Linux this limit will be 4096, but
    --- that won't be the case for functions such as realpath that are
    --- implemented at the C library level; however such functions are
    --- the exception rather than the norm, and report `enametoolong()`,
    --- when exceeding the libc limit.
    PATH_MAX = nil,

    ---@type integer Causes the violating thread to be killed. This is
    --- the default on Linux. It's effectively the same as killing the
    --- process, since redbean has no threads. The termination signal
    --- can't be caught and will be either `SIGSYS` or `SIGABRT`.
    --- Consider enabling stderr logging below so you'll know why your
    --- program failed. Otherwise check the system log.
    PLEDGE_PENALTY_KILL_THREAD = nil,

    ---@type integer Causes the process and all its threads to be killed.
    --- This is always the case on OpenBSD.
    PLEDGE_PENALTY_KILL_PROCESS = nil,

    ---@type integer Causes system calls to just return an `EPERM` error
    --- instead of killing. This is a gentler solution that allows code to
    --- display a friendly warning. Please note this may lead to weird
    --- behaviors if the software being sandboxed is lazy about checking
    --- error results.
    PLEDGE_PENALTY_RETURN_EPERM = nil,

    ---@type integer Enables friendly error message logging letting you
    --- know which promises are needed whenever violations occur. Without
    --- this, violations will be logged to `dmesg` on Linux if the penalty
    --- is to kill the process. You would then need to manually look up
    --- the system call number and then cross reference it with the
    --- cosmopolitan libc `pledge()` documentation. You can also use
    --- `strace -ff` which is easier. This is ignored OpenBSD, which
    --- already has a good system log. Turning on stderr logging (which
    --- uses SECCOMP trapping) also means that the `unix.WTERMSIG()` on
    --- your killed processes will always be `unix.SIGABRT` on both Linux
    --- and OpenBSD. Otherwise, Linux prefers to raise `unix.SIGSYS`.
    PLEDGE_STDERR_LOGGING = nil,

    --- @type integer Returns maximum size at which pipe i/o is guaranteed atomic.
    ---
    --- POSIX requires this be at least 512. Linux is more generous and
    --- allows 4096. On Windows NT this is currently 4096, and it's the
    --- parameter redbean passes to `CreateNamedPipe()`.
    PIPE_BUF = nil,

    --- @type integer
    POLLERR = nil,
    --- @type integer
    POLLHUP = nil,
    --- @type integer
    POLLIN = nil,
    --- @type integer
    POLLNVAL = nil,
    --- @type integer
    POLLOUT = nil,
    --- @type integer
    POLLPRI = nil,
    --- @type integer
    POLLRDBAND = nil,
    --- @type integer
    POLLRDHUP = nil,
    --- @type integer
    POLLRDNORM = nil,
    --- @type integer
    POLLWRBAND = nil,
    --- @type integer
    POLLWRNORM = nil,

    --- @type integer
    RLIMIT_AS = nil,
    --- @type integer
    RLIMIT_CPU = nil,
    --- @type integer
    RLIMIT_FSIZE = nil,
    --- @type integer
    RLIMIT_NOFILE = nil,
    --- @type integer
    RLIMIT_NPROC = nil,
    --- @type integer
    RLIMIT_RSS = nil,

    --- @type integer
    RUSAGE_BOTH = nil,
    --- @type integer
    RUSAGE_CHILDREN = nil,
    --- @type integer
    RUSAGE_SELF = nil,
    --- @type integer
    RUSAGE_THREAD = nil,

    --- @type integer
    R_OK = nil,

    --- @type integer
    SA_NOCLDSTOP = nil,
    --- @type integer
    SA_NOCLDWAIT = nil,
    --- @type integer
    SA_NODEFER = nil,
    --- @type integer
    SA_RESETHAND = nil,
    --- @type integer
    SA_RESTART = nil,

    --- @type integer
    SEEK_CUR = nil,
    --- @type integer
    SEEK_END = nil,
    --- @type integer
    SEEK_SET = nil,

    ---@type integer sends a tcp half close for reading
    SHUT_RD = nil,
    ---@type integer sends a tcp half close for writing
    SHUT_WR = nil,
    ---@type integer
    SHUT_RDWR = nil,

    --- @type integer Process aborted.
    SIGABRT = nil,
    --- @type integer Sent by setitimer().
    SIGALRM = nil,
    --- @type integer Valid memory access that went beyond underlying end of file.
    SIGBUS = nil,
    --- @type integer Child process exited or terminated and is now a zombie (unless this
    --- is `SIG_IGN` or `SA_NOCLDWAIT`) or child process stopped due to terminal
    --- i/o or profiling/debugging (unless you used `SA_NOCLDSTOP`)
    SIGCHLD = nil,
    --- @type integer Child process resumed from profiling/debugging.
    SIGCONT = nil,
    --- @type integer
    SIGEMT = nil,
    --- @type integer Illegal math.
    SIGFPE = nil,
    --- @type integer Terminal hangup or daemon reload; auto-broadcasted to process group.
    SIGHUP = nil,
    --- @type integer Illegal instruction.
    SIGILL = nil,
    --- @type integer
    SIGINFO = nil,
    --- @type integer Terminal CTRL-C keystroke.
    SIGINT = nil,
    --- @type integer
    SIGIO = nil,
    --- @type integer Terminate with extreme prejudice.
    SIGKILL = nil,
    --- @type integer Write to closed file descriptor.
    SIGPIPE = nil,
    --- @type integer Profiling timer expired.
    SIGPROF = nil,
    --- @type integer Not implemented in most community editions of system five.
    SIGPWR = nil,
    --- @type integer Terminal CTRL-\ keystroke.
    SIGQUIT = nil,
    --- @type integer
    SIGRTMAX = nil,
    --- @type integer
    SIGRTMIN = nil,
    --- @type integer Invalid memory access.
    SIGSEGV = nil,
    --- @type integer
    SIGSTKFLT = nil,
    --- @type integer Child process stopped due to profiling/debugging.
    SIGSTOP = nil,
    --- @type integer
    SIGSYS = nil,
    --- @type integer Terminate.
    SIGTERM = nil,
    --- @type integer INT3 instruction.
    SIGTRAP = nil,
    --- @type integer Terminal CTRL-Z keystroke.
    SIGTSTP = nil,
    --- @type integer Terminal input for background process.
    SIGTTIN = nil,
    --- @type integer Terminal input for background process.
    SIGTTOU = nil,
    --- @type integer
    SIGURG = nil,
    --- @type integer Do whatever you want.
    SIGUSR1 = nil,
    --- @type integer Do whatever you want.
    SIGUSR2 = nil,
    --- @type integer Virtual alarm clock.
    SIGVTALRM = nil,
    --- @type integer Terminal resized.
    SIGWINCH = nil,
    --- @type integer CPU time limit exceeded.
    SIGXCPU = nil,
    --- @type integer File size limit exceeded.
    SIGXFSZ = nil,

    --- @type integer
    SIG_BLOCK = nil,
    --- @type integer
    SIG_DFL = nil,
    --- @type integer
    SIG_IGN = nil,
    --- @type integer
    SIG_SETMASK = nil,
    --- @type integer
    SIG_UNBLOCK = nil,

    --- @type integer
    SOCK_CLOEXEC = nil,
    --- @type integer
    SOCK_DGRAM = nil,
    --- @type integer
    SOCK_NONBLOCK = nil,
    --- @type integer
    SOCK_RAW = nil,
    --- @type integer
    SOCK_RDM = nil,
    --- @type integer
    SOCK_SEQPACKET = nil,
    --- @type integer
    SOCK_STREAM = nil,

    --- @type integer
    SOL_IP = nil,
    --- @type integer
    SOL_SOCKET = nil,
    --- @type integer
    SOL_TCP = nil,
    --- @type integer
    SOL_UDP = nil,

    --- @type integer
    SO_ACCEPTCONN = nil,
    --- @type integer
    SO_BROADCAST = nil,
    --- @type integer
    SO_DEBUG = nil,
    --- @type integer
    SO_DONTROUTE = nil,
    --- @type integer
    SO_ERROR = nil,
    --- @type integer
    SO_KEEPALIVE = nil,
    --- @type integer
    SO_LINGER = nil,
    --- @type integer
    SO_RCVBUF = nil,
    --- @type integer
    SO_RCVLOWAT = nil,
    --- @type integer
    SO_RCVTIMEO = nil,
    --- @type integer
    SO_REUSEADDR = nil,
    --- @type integer
    SO_REUSEPORT = nil,
    --- @type integer
    SO_SNDBUF = nil,
    --- @type integer
    SO_SNDLOWAT = nil,
    --- @type integer
    SO_SNDTIMEO = nil,
    --- @type integer
    SO_TYPE = nil,

    --- @type integer
    TCP_CORK = nil,
    --- @type integer
    TCP_DEFER_ACCEPT = nil,
    --- @type integer
    TCP_FASTOPEN = nil,
    --- @type integer
    TCP_FASTOPEN_CONNECT = nil,
    --- @type integer
    TCP_KEEPCNT = nil,
    --- @type integer
    TCP_KEEPIDLE = nil,
    --- @type integer
    TCP_KEEPINTVL = nil,
    --- @type integer
    TCP_MAXSEG = nil,
    --- @type integer
    TCP_NODELAY = nil,
    --- @type integer
    TCP_NOTSENT_LOWAT = nil,
    --- @type integer
    TCP_QUICKACK = nil,
    --- @type integer
    TCP_SAVED_SYN = nil,
    --- @type integer
    TCP_SAVE_SYN = nil,
    --- @type integer
    TCP_SYNCNT = nil,
    --- @type integer
    TCP_WINDOW_CLAMP = nil,

    --- @type integer
    UTIME_NOW = nil,
    --- @type integer
    UTIME_OMIT = nil,

    --- @type integer
    WNOHANG = nil,

    --- @type integer
    W_OK = nil,

    --- @type integer
    X_OK = nil
}

--- Opens file.
---
--- Returns a file descriptor integer that needs to be closed, e.g.
---
---     fd = assert(unix.open("/etc/passwd", unix.O_RDONLY))
---     print(unix.read(fd))
---     unix.close(fd)
---
--- `flags` should have one of:
---
--- - `O_RDONLY`:     open for reading (default)
--- - `O_WRONLY`:     open for writing
--- - `O_RDWR`:       open for reading and writing
---
--- The following values may also be OR'd into `flags`:
---
---  - `O_CREAT`      create file if it doesn't exist
---  - `O_TRUNC`      automatic ftruncate(fd,0) if exists
---  - `O_CLOEXEC`    automatic close() upon execve()
---  - `O_EXCL`       exclusive access (see below)
---  - `O_APPEND`     open file for append only
---  - `O_NONBLOCK`   asks read/write to fail with EAGAIN rather than block
---  - `O_DIRECT`     it's complicated (not supported on Apple and OpenBSD)
---  - `O_DIRECTORY`  useful for stat'ing (hint on UNIX but required on NT)
---  - `O_TMPFILE`    try to make temp more secure (Linux and Windows only)
---  - `O_NOFOLLOW`   fail if it's a symlink (zero on Windows)
---  - `O_DSYNC`      it's complicated (zero on non-Linux/Apple)
---  - `O_RSYNC`      it's complicated (zero on non-Linux/Apple)
---  - `O_PATH`       it's complicated (zero on non-Linux)
---  - `O_VERIFY`     it's complicated (zero on non-FreeBSD)
---  - `O_SHLOCK`     it's complicated (zero on non-BSD)
---  - `O_EXLOCK`     it's complicated (zero on non-BSD)
---  - `O_NOATIME`    don't record access time (zero on non-Linux)
---  - `O_RANDOM`     hint random access intent (zero on non-Windows)
---  - `O_SEQUENTIAL` hint sequential access intent (zero on non-Windows)
---  - `O_COMPRESSED` ask fs to abstract compression (zero on non-Windows)
---  - `O_INDEXED`    turns on that slow performance (zero on non-Windows)
---
---  There are three regular combinations for the above flags:
---
---  - `O_RDONLY`: Opens existing file for reading. If it doesn't exist
---    then nil is returned and errno will be `ENOENT` (or in some other
---    cases `ENOTDIR`).
---
---  - `O_WRONLY|O_CREAT|O_TRUNC`: Creates file. If it already exists,
---    then the existing copy is destroyed and the opened file will
---    start off with a length of zero. This is the behavior of the
---    traditional creat() system call.
---
---  - `O_WRONLY|O_CREAT|O_EXCL`: Create file only if doesn't exist
---    already. If it does exist then `nil` is returned along with
---    `errno` set to `EEXIST`.
---
--- `dirfd` defaults to to `unix.AT_FDCWD` and may optionally be set to
--- a directory file descriptor to which `path` is relative.
---
--- Returns `ENOENT` if `path` doesn't exist.
---
--- Returns `ENOTDIR` if `path` contained a directory component that
--- wasn't a directory
---.
---@param path string
---@param flags integer
---@param mode integer?
---@param dirfd integer?
---@return integer fd
---@nodiscard
---@overload fun(path: string, flags: integer, mode?: integer, dirfd?: integer): nil, error: unix.Errno
function unix.open(path, flags, mode, dirfd) end

--- Closes file descriptor.
---
--- This function should never be called twice for the same file
--- descriptor, regardless of whether or not an error happened. The file
--- descriptor is always gone after close is called. So it technically
--- always succeeds, but that doesn't mean an error should be ignored.
--- For example, on NFS a close failure could indicate data loss.
---
--- Closing does not mean that scheduled i/o operations have been
--- completed. You'd need to use fsync() or fdatasync() beforehand to
--- ensure that. You shouldn't need to do that normally, because our
--- close implementation guarantees a consistent view, since on systems
--- where it isn't guaranteed (like Windows) close will implicitly sync.
---
--- File descriptors are automatically closed on exit().
---
--- Returns `EBADF` if `fd` wasn't valid.
---
--- Returns `EINTR` possibly maybe.
---
--- Returns `EIO` if an i/o error occurred.
---@param fd integer
---@return true
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.close(fd) end

--- Reads from file descriptor.
---
--- This function returns empty string on end of file. The exception is
--- if `bufsiz` is zero, in which case an empty returned string means
--- the file descriptor works.
---@param fd integer
---@param bufsiz string?
---@param offset integer?
---@return string data
---@overload fun(fd: integer, bufsiz?: string, offset?: integer): nil, error: unix.Errno
function unix.read(fd, bufsiz, offset) end

--- Writes to file descriptor.
---@param fd integer
---@param data string
---@param offset integer?
---@return integer wrotebytes
---@overload fun(fd: integer, data: string, offset?: integer): nil, error: unix.Errno
function unix.write(fd, data, offset) end

--- Invokes `_Exit(exitcode)` on the process. This will immediately
--- halt the current process. Memory will be freed. File descriptors
--- will be closed. Any open connections it owns will be reset. This
--- function never returns.
---@param exitcode integer?
function unix.exit(exitcode) end

--- Returns raw environment variables.
---
--- This allocates and constructs the C/C++ `environ` variable as a Lua
--- table consisting of string keys and string values.
---
--- This data structure preserves casing. On Windows NT, by convention,
--- environment variable keys are treated in a case-insensitive way. It
--- is the responsibility of the caller to consider this.
---
--- This data structure preserves valueless variables. It's possible on
--- both UNIX and Windows to have an environment variable without an
--- equals, even though it's unusual.
---
--- This data structure preserves duplicates. For example, on Windows,
--- there's some irregular uses of environment variables such as how the
--- command prompt inserts multiple environment variables with empty
--- string as keys, for its internal bookkeeping.
---
---@return table<string, string?>
---@nodiscard
function unix.environ() end

--- Creates a new process mitosis style.
---
--- This system call returns twice. The parent process gets the nonzero
--- pid. The child gets zero.
---
--- Here's a simple usage example of creating subprocesses, where we
--- fork off a child worker from a main process hook callback to do some
--- independent chores, such as sending an HTTP request back to redbean.
---
---    -- as soon as server starts, make a fetch to the server
---    -- then signal redbean to shutdown when fetch is complete
---    local onServerStart = function()
---       if assert(unix.fork()) == 0 then
---          local ok, headers, body = Fetch('http://127.0.0.1:8080/test')
---          unix.kill(unix.getppid(), unix.SIGTERM)
---          unix.exit(0)
---       end
---    end
---    OnServerStart = onServerStart
---
--- We didn't need to use `wait()` here, because (a) we want redbean to go
--- back to what it was doing before as the `Fetch()` completes, and (b)
--- redbean's main process already has a zombie collector. However it's
--- a moot point, since once the fetch is done, the child process then
--- asks redbean to gracefully shutdown by sending SIGTERM its parent.
---
--- This is actually a situation where we *must* use fork, because the
--- purpose of the main redbean process is to call accept() and create
--- workers. So if we programmed redbean to use the main process to send
--- a blocking request to itself instead, then redbean would deadlock
--- and never be able to accept() the client.
---
--- While deadlocking is an extreme example, the truth is that latency
--- issues can crop up for the same reason that just cause jitter
--- instead, and as such, can easily go unnoticed. For example, if you
--- do soemething that takes longer than a few milliseconds from inside
--- your redbean heartbeat, then that's a few milliseconds in which
--- redbean is no longer concurrent, and tail latency is being added to
--- its ability to accept new connections. fork() does a great job at
--- solving this.
---
--- If you're not sure how long something will take, then when in doubt,
--- fork off a process. You can then report its completion to something
--- like SQLite. Redbean makes having lots of processes cheap. On Linux
--- they're about as lightweight as what heavyweight environments call
--- greenlets. You can easily have 10,000 Redbean workers on one PC.
---
--- Here's some benchmarks for fork() performance across platforms:
---
---    Linux 5.4 fork      l:     97,200𝑐    31,395𝑛𝑠  [metal]
---    FreeBSD 12 fork     l:    236,089𝑐    78,841𝑛𝑠  [vmware]
---    Darwin 20.6 fork    l:    295,325𝑐    81,738𝑛𝑠  [metal]
---    NetBSD 9 fork       l:  5,832,027𝑐 1,947,899𝑛𝑠  [vmware]
---    OpenBSD 6.8 fork    l: 13,241,940𝑐 4,422,103𝑛𝑠  [vmware]
---    Windows10 fork      l: 18,802,239𝑐 6,360,271𝑛𝑠  [metal]
---
--- One of the benefits of using `fork()` is it creates an isolation
--- barrier between the different parts of your app. This can lead to
--- enhanced reliability and security. For example, redbean uses fork so
--- it can wipe your ssl keys from memory before handing over control to
--- request handlers that process untrusted input. It also ensures that
--- if your Lua app crashes, it won't take down the server as a whole.
--- Hence it should come as no surprise that `fork()` would go slower on
--- operating systems that have more security features. So depending on
--- your use case, you can choose the operating system that suits you.
---
---@return integer|0 childpid
---@overload fun(): nil, error: unix.Errno
function unix.fork() end

--- Performs `$PATH` lookup of executable.
---
---     unix = require 'unix'
---     prog = assert(unix.commandv('ls'))
---     unix.execve(prog, {prog, '-hal', '.'}, {'PATH=/bin'})
---     unix.exit(127)
---
--- We automatically suffix `.com` and `.exe` for all platforms when
--- path searching. By default, the current directory is not on the
--- path. If `prog` is an absolute path, then it's returned as-is. If
--- `prog` contains slashes then it's not path searched either and will
--- be returned if it exists.
---@param prog string
---@return string path
---@overload fun(prog: string): nil, error: unix.Errno
function unix.commandv(prog) end

--- Exits current process, replacing it with a new instance of the
--- specified program. `prog` needs to be an absolute path, see
--- commandv(). `env` defaults to to the current `environ`. Here's
--- a basic usage example:
---
---     unix.execve("/bin/ls", {"/bin/ls", "-hal"}, {"PATH=/bin"})
---     unix.exit(127)
---
--- `prog` needs to be the resolved pathname of your executable. You
--- can use commandv() to search your `PATH`.
---
--- `args` is a string list table. The first element in `args`
--- should be `prog`. Values are coerced to strings. This parameter
--- defaults to `{prog}`.
---
--- `env` is a string list table. Values are coerced to strings. No
--- ordering requirement is imposed. By convention, each string has its
--- key and value divided by an equals sign without spaces. If this
--- parameter is not specified, it'll default to the C/C++ `environ`
--- variable which is inherited from the shell that launched redbean.
--- It's the responsibility of the user to supply a sanitized environ
--- when spawning untrusted processes.
---
--- `execve()` is normally called after `fork()` returns `0`. If that isn't
--- the case, then your redbean worker will be destroyed.
---
--- This function never returns on success.
---
--- `EAGAIN` is returned if you've enforced a max number of
--- processes using `setrlimit(RLIMIT_NPROC)`.
---
---@param prog string
---@param args string[]
---@param env string[]
---@return nil, unix.Errno error
---@overload fun(prog: string): nil, error: unix.Errno
function unix.execve(prog, args, env) end

--- Duplicates file descriptor.
---
--- `newfd` may be specified to choose a specific number for the new
--- file descriptor. If it's already open, then the preexisting one will
--- be silently closed. `EINVAL` is returned if `newfd` equals `oldfd`.
---
--- `flags` can have `O_CLOEXEC` which means the returned file
--- descriptors will be automatically closed upon execve().
---
--- `lowest` defaults to zero and defines the lowest numbered file
--- descriptor that's acceptable to use. If `newfd` is specified then
--- `lowest` is ignored. For example, if you wanted to duplicate
--- standard input, then:
---
---     stdin2 = assert(unix.dup(0, nil, unix.O_CLOEXEC, 3))
---
--- Will ensure that, in the rare event standard output or standard
--- error are closed, you won't accidentally duplicate standard input to
--- those numbers.
---
---@param oldfd integer
---@param newfd integer?
---@param flags integer?
---@param lowest integer?
---@return integer newfd
---@overload fun(oldfd: integer, newfd?: integer, flags?: integer, lowest?: integer): nil, error: unix.Errno
function unix.dup(oldfd, newfd, flags, lowest) end

--- Creates fifo which enables communication between processes.
---
---@param flags integer? may have any combination (using bitwise OR) of:
---
--- - `O_CLOEXEC`: Automatically close file descriptor upon execve()
---
--- - `O_NONBLOCK`: Request `EAGAIN` be raised rather than blocking
---
--- - `O_DIRECT`: Enable packet mode w/ atomic reads and writes, so long
---   as they're no larger than `PIPE_BUF` (guaranteed to be 512+ bytes)
---   with support limited to Linux, Windows NT, FreeBSD, and NetBSD.
---
--- Returns two file descriptors: one for reading and one for writing.
---
--- Here's an example of how pipe(), fork(), dup(), etc. may be used
--- to serve an HTTP response containing the output of a subprocess.
---
---     local unix = require "unix"
---     ls = assert(unix.commandv("ls"))
---     reader, writer = assert(unix.pipe())
---     if assert(unix.fork()) == 0 then
---        unix.close(1)
---        unix.dup(writer)
---        unix.close(writer)
---        unix.close(reader)
---        unix.execve(ls, {ls, "-Shal"})
---        unix.exit(127)
---     else
---        unix.close(writer)
---        SetHeader('Content-Type', 'text/plain')
---        while true do
---           data, err = unix.read(reader)
---           if data then
---              if data ~= "" then
---                 Write(data)
---              else
---                 break
---              end
---           elseif err:errno() ~= EINTR then
---              Log(kLogWarn, tostring(err))
---              break
---           end
---        end
---        assert(unix.close(reader))
---        assert(unix.wait())
---     end
---
---@return integer reader, integer writer
---@nodiscard
---@overload fun(flags?: integer): nil, error: unix.Errno
function unix.pipe(flags) end

--- Waits for subprocess to terminate.
---
--- `pid` defaults to `-1` which means any child process. Setting
--- `pid` to `0` is equivalent to `-getpid()`. If `pid < -1` then
--- that means wait for any pid in the process group `-pid`. Then
--- lastly if `pid > 0` then this waits for a specific process id
---
--- Options may have `WNOHANG` which means don't block, check for
--- the existence of processes that are already dead (technically
--- speaking zombies) and if so harvest them immediately.
---
--- Returns the process id of the child that terminated. In other
--- cases, the returned `pid` is nil and `errno` is non-nil.
---
--- The returned `wstatus` contains information about the process
--- exit status. It's a complicated integer and there's functions
--- that can help interpret it. For example:
---
---     -- wait for zombies
---     -- traditional technique for SIGCHLD handlers
---     while true do
---        pid, status = unix.wait(-1, unix.WNOHANG)
---        if pid then
---           if unix.WIFEXITED(status) then
---              print('child', pid, 'exited with',
---                    unix.WEXITSTATUS(status))
---           elseif unix.WIFSIGNALED(status) then
---              print('child', pid, 'crashed with',
---                    unix.strsignal(unix.WTERMSIG(status)))
---           end
---        elseif status:errno() == unix.ECHILD then
---           Log(kLogDebug, 'no more zombies')
---           break
---        else
---           Log(kLogWarn, tostring(err))
---           break
---        end
---     end
---
---@param pid? integer
---@param options? integer
---@return integer pid, integer wstatus, unix.Rusage rusage
---@overload fun(pid?: integer, options?: integer): nil, error: unix.Errno
function unix.wait(pid, options) end

--- Returns `true` if process exited cleanly.
---@param wstatus integer
---@return boolean
---@nodiscard
function unix.WIFEXITED(wstatus) end

--- Returns code passed to exit() assuming `WIFEXITED(wstatus)` is true.
---@param wstatus integer
---@return integer exitcode uint8
---@nodiscard
function unix.WEXITSTATUS(wstatus) end

--- Returns `true` if process terminated due to a signal.
---@param wstatus integer
---@return boolean
---@nodiscard
function unix.WIFSIGNALED(wstatus) end

--- Returns signal that caused process to terminate assuming
--- `WIFSIGNALED(wstatus)` is `true`.
---@param wstatus integer
---@return integer sig uint8
---@nodiscard
function unix.WTERMSIG(wstatus) end

--- Returns process id of current process.
---
--- This function does not fail.
---@return integer pid
---@nodiscard
function unix.getpid() end

--- Returns process id of parent process.
---
--- This function does not fail.
---@return integer pid
---@nodiscard
function unix.getppid() end

--- Sends signal to process(es).
---
--- The impact of this action can be terminating the process, or
--- interrupting it to request something happen.
---
--- `pid` can be:
---
--- - `pid > 0` signals one process by id
--- - `== 0`    signals all processes in current process group
--- - `-1`      signals all processes possible (except init)
--- - `< -1`    signals all processes in -pid process group
---
--- `sig` can be:
---
--- - `0`       checks both if pid exists and we can signal it
--- - `SIGINT`  sends ctrl-c keyboard interrupt
--- - `SIGQUIT` sends backtrace and exit signal
--- - `SIGTERM` sends shutdown signal
--- - etc.
---
--- Windows NT only supports the kill() signals required by the ANSI C89
--- standard, which are `SIGINT` and `SIGQUIT`. All other signals on the
--- Windows platform that are sent to another process via kill() will be
--- treated like `SIGKILL`.
---@param pid integer
---@param sig integer
---@return true
---@overload fun(pid: integer, sid: integer): nil, error: unix.Errno
function unix.kill(pid, sig) end

--- Triggers signal in current process.
---
--- This is pretty much the same as `kill(getpid(), sig)`.
---@param sig integer
---@return integer rc
---@overload fun(sig: integer): nil, error: unix.Errno
function unix.raise(sig) end

--- Checks if effective user of current process has permission to access file.
---@param path string
---@param how integer can be `R_OK`, `W_OK`, `X_OK`, or `F_OK` to check for read, write, execute, and existence respectively.
---@param flags? integer may have any of:
--- - `AT_SYMLINK_NOFOLLOW`: do not follow symbolic links.
---@param dirfd? integer
---@return true
---@overload fun(path: string, how: integer, flags?: integer, dirfd?: integer): nil, error: unix.Errno
function unix.access(path, how, flags, dirfd) end

--- Makes directory.
---
--- `path` is the path of the directory you wish to create.
---
--- `mode` is octal permission bits, e.g. `0755`.
---
--- Fails with `EEXIST` if `path` already exists, whether it be a
--- directory or a file.
---
--- Fails with `ENOENT` if the parent directory of the directory you
--- want to create doesn't exist. For making `a/really/long/path/`
--- consider using makedirs() instead.
---
--- Fails with `ENOTDIR` if a parent directory component existed that
--- wasn't a directory.
---
--- Fails with `EACCES` if the parent directory doesn't grant write
--- permission to the current user.
---
--- Fails with `ENAMETOOLONG` if the path is too long.
---
---@param path string
---@param mode? integer
---@param dirfd? integer
---@return true
---@overload fun(path: string, mode?: integer, dirfd?: integer): nil, error: unix.Errno
function unix.mkdir(path, mode, dirfd) end

--- Unlike mkdir() this convenience wrapper will automatically create
--- parent parent directories as needed. If the directory already exists
--- then, unlike mkdir() which returns EEXIST, the makedirs() function
--- will return success.
---
--- `path` is the path of the directory you wish to create.
---
--- `mode` is octal permission bits, e.g. `0755`.
---
---@param path string
---@param mode? integer
---@return true
---@overload fun(path: string, mode?: integer): nil, error: unix.Errno
function unix.makedirs(path, mode) end

--- Changes current directory to `path`.
---@param path string
---@return true
---@overload fun(path: string): nil, error: unix.Errno
function unix.chdir(path) end

--- Removes file at `path`.
---
--- If `path` refers to a symbolic link, the link is removed.
---
--- Returns `EISDIR` if `path` refers to a directory. See `rmdir()`.
---
---@param path string
---@param dirfd? integer
---@return true
---@overload fun(path: string, dirfd?: integer): nil, error: unix.Errno
function unix.unlink(path, dirfd) end

--- Removes empty directory at `path`.
---
--- Returns `ENOTDIR` if `path` isn't a directory, or a path component
--- in `path` exists yet wasn't a directory.
---
---@param path string
---@param dirfd? integer
---@return true
---@overload fun(path: string, dirfd?: integer): nil, error: unix.Errno
function unix.rmdir(path, dirfd) end

--- Renames file or directory.
---@param oldpath string
---@param newpath string
---@param olddirfd integer
---@param newdirfd integer
---@return true
---@overload fun(oldpath: string, newpath: string): true
---@overload fun(oldpath: string, newpath: string, olddirfd: integer, newdirfd: integer): nil, error: unix.Errno
---@overload fun(oldpath: string, newpath: string): nil, error: unix.Errno
function unix.rename(oldpath, newpath, olddirfd, newdirfd) end

---Creates hard link, so your underlying inode has two names.
---@param existingpath string
---@param newpath string
---@param flags integer
---@param olddirfd integer
---@param newdirfd integer
---@return true
---@overload fun(existingpath: string, newpath: string, flags?: integer): true
---@overload fun(existingpath: string, newpath: string, flags?: integer): nil, error: unix.Errno
---@overload fun(existingpath: string, newpath: string, flags: integer, olddirfd: integer, newdirfd: integer): true
---@overload fun(existingpath: string, newpath: string, flags: integer, olddirfd: integer, newdirfd: integer): nil, error: unix.Errno
function unix.link(existingpath, newpath, flags, olddirfd, newdirfd) end

--- Creates symbolic link.
---
--- On Windows NT a symbolic link is called a "reparse point" and can
--- only be created from an administrator account. Your redbean will
--- automatically request the appropriate permissions.
---@param target string
---@param linkpath string
---@param newdirfd? integer
---@return true
---@overload fun(target: string, linkpath: string, newdirfd?: integer): nil, error: unix.Errno
function unix.symlink(target, linkpath, newdirfd) end

--- Reads contents of symbolic link.
---
--- Note that broken links are supported on all platforms. A symbolic
--- link can contain just about anything. It's important to not assume
--- that `content` will be a valid filename.
---
--- On Windows NT, this function transliterates `\` to `/` and
--- furthermore prefixes `//?/` to WIN32 DOS-style absolute paths,
--- thereby assisting with simple absolute filename checks in addition
--- to enabling one to exceed the traditional 260 character limit.
---@param path string
---@param dirfd? integer
---@return string content
---@nodiscard
---@overload fun(path: string, dirfd?: integer): nil, error: unix.Errno
function unix.readlink(path, dirfd) end

--- Returns absolute path of filename, with `.` and `..` components
--- removed, and symlinks will be resolved.
---@param path string
---@return string path
---@nodiscard
---@overload fun(path: string): nil, error: unix.Errno
function unix.realpath(path) end

--- Changes access and/or modified timestamps on file.
---
--- `path` is a string with the name of the file.
---
--- The `asecs` and `ananos` parameters set the access time. If they're
--- none or nil, the current time will be used.
---
--- The `msecs` and `mnanos` parameters set the modified time. If
--- they're none or nil, the current time will be used.
---
--- The nanosecond parameters (`ananos` and `mnanos`) must be on the
--- interval [0,1000000000) or `unix.EINVAL` is raised. On XNU this is
--- truncated to microsecond precision. On Windows NT, it's truncated to
--- hectonanosecond precision. These nanosecond parameters may also be
--- set to one of the following special values:
---
--- - `unix.UTIME_NOW`: Fill this timestamp with current time. This
--- feature is not available on old versions of Linux, e.g. RHEL5.
---
--- - `unix.UTIME_OMIT`: Do not alter this timestamp. This feature is
--- not available on old versions of Linux, e.g. RHEL5.
---
--- `dirfd` is a file descriptor integer opened with `O_DIRECTORY`
--- that's used for relative path names. It defaults to `unix.AT_FDCWD`.
---
--- `flags` may have have any of the following flags bitwise or'd
---
--- - `AT_SYMLINK_NOFOLLOW`: Do not follow symbolic links. This makes it
--- possible to edit the timestamps on the symbolic link itself,
--- rather than the file it points to.
---
---@param path string
---@param asecs integer
---@param ananos integer
---@param msecs integer
---@param mnanos integer
---@param dirfd? integer
---@param flags? integer
---@return 0
---@overload fun(path: string): 0
---@overload fun(path: string, asecs: integer, ananos: integer, msecs: integer, mnanos: integer, dirfd?: integer, flags?: integer): nil, error: unix.Errno
---@overload fun(path: string): nil, error: unix.Errno
function unix.utimensat(path, asecs, ananos, msecs, mnanos, dirfd, flags) end

--- Changes access and/or modified timestamps on file descriptor.
---
--- `fd` is the file descriptor of a file opened with `unix.open`.
---
--- The `asecs` and `ananos` parameters set the access time. If they're
--- none or nil, the current time will be used.
---
--- The `msecs` and `mnanos` parameters set the modified time. If
--- they're none or nil, the current time will be used.
---
--- The nanosecond parameters (`ananos` and `mnanos`) must be on the
--- interval [0,1000000000) or `unix.EINVAL` is raised. On XNU this is
--- truncated to microsecond precision. On Windows NT, it's truncated to
--- hectonanosecond precision. These nanosecond parameters may also be
--- set to one of the following special values:
---
--- - `unix.UTIME_NOW`: Fill this timestamp with current time.
---
--- - `unix.UTIME_OMIT`: Do not alter this timestamp.
---
--- This system call is currently not available on very old versions of
--- Linux, e.g. RHEL5.
---
---@param fd integer
---@param asecs integer
---@param ananos integer
---@param msecs integer
---@param mnanos integer
---@return 0
---@overload fun(fd: integer): 0
---@overload fun(fd: integer, asecs: integer, ananos: integer, msecs: integer, mnanos: integer): nil, error: unix.Errno
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.futimens(fd, asecs, ananos, msecs, mnanos) end

--- Changes user and group on file.
---
--- Returns `ENOSYS` on Windows NT.
---@param path string
---@param uid integer
---@param gid integer
---@param flags? integer
---@param dirfd? integer
---@return true
---@overload fun(path: string, uid: integer, gid: integer, flags?: integer, dirfd?: integer): nil, error: unix.Errno
function unix.chown(path, uid, gid, flags, dirfd) end

--- Changes mode bits on file.
---
--- On Windows NT the chmod system call only changes the read-only
--- status of a file.
---@param path string
---@param mode integer
---@param flags? integer
---@param dirfd? integer
---@return true
---@overload fun(path: string, mode: integer, flags?: integer, dirfd?: integer): nil, error: unix.Errno
function unix.chmod(path, mode, flags, dirfd) end

--- Returns current working directory.
---
--- On Windows NT, this function transliterates `\` to `/` and
--- furthermore prefixes `//?/` to WIN32 DOS-style absolute paths,
--- thereby assisting with simple absolute filename checks in addition
--- to enabling one to exceed the traditional 260 character limit.
---@return string path
---@nodiscard
---@overload fun(): nil, error: unix.Errno
function unix.getcwd() end

--- Recursively removes filesystem path.
---
--- Like `unix.makedirs()` this function isn't actually a system call but
--- rather is a Libc convenience wrapper. It's intended to be equivalent
--- to using the UNIX shell's `rm -rf path` command.
---
---@param path string the file or directory path you wish to destroy.
---@return true
---@overload fun(path: string): nil, error: unix.Errno
function unix.rmrf(path) end

--- Manipulates file descriptor.
---
--- `cmd` may be one of:
---
--- - `unix.F_GETFD` Returns file descriptor flags.
--- - `unix.F_SETFD` Sets file descriptor flags.
--- - `unix.F_GETFL` Returns file descriptor status flags.
--- - `unix.F_SETFL` Sets file descriptor status flags.
--- - `unix.F_SETLK` Acquires lock on file interval.
--- - `unix.F_SETLKW` Waits for lock on file interval.
--- - `unix.F_GETLK` Acquires information about lock.
---
--- unix.fcntl(fd:int, unix.F_GETFD)
---     ├─→ flags:int
---     └─→ nil, unix.Errno
---
---   Returns file descriptor flags.
---
---   The returned `flags` may include any of:
---
---   - `unix.FD_CLOEXEC` if `fd` was opened with `unix.O_CLOEXEC`.
---
---   Returns `EBADF` if `fd` isn't open.
---
--- unix.fcntl(fd:int, unix.F_SETFD, flags:int)
---     ├─→ true
---     └─→ nil, unix.Errno
---
---   Sets file descriptor flags.
---
---   `flags` may include any of:
---
---   - `unix.FD_CLOEXEC` to re-open `fd` with `unix.O_CLOEXEC`.
---
---   Returns `EBADF` if `fd` isn't open.
---
--- unix.fcntl(fd:int, unix.F_GETFL)
---     ├─→ flags:int
---     └─→ nil, unix.Errno
---
---   Returns file descriptor status flags.
---
---   `flags & unix.O_ACCMODE` includes one of:
---
---   - `O_RDONLY`
---   - `O_WRONLY`
---   - `O_RDWR`
---
---   Examples of values `flags & ~unix.O_ACCMODE` may include:
---
---   - `O_NONBLOCK`
---   - `O_APPEND`
---   - `O_SYNC`
---   - `O_ASYNC`
---   - `O_NOATIME` on Linux
---   - `O_RANDOM` on Windows
---   - `O_SEQUENTIAL` on Windows
---   - `O_DIRECT` on Linux/FreeBSD/NetBSD/Windows
---
---   Examples of values `flags & ~unix.O_ACCMODE` won't include:
---
---   - `O_CREAT`
---   - `O_TRUNC`
---   - `O_EXCL`
---   - `O_NOCTTY`
---
---   Returns `EBADF` if `fd` isn't open.
---
--- unix.fcntl(fd:int, unix.F_SETFL, flags:int)
---     ├─→ true
---     └─→ nil, unix.Errno
---
---   Changes file descriptor status flags.
---
---   Examples of values `flags` may include:
---
---   - `O_NONBLOCK`
---   - `O_APPEND`
---   - `O_SYNC`
---   - `O_ASYNC`
---   - `O_NOATIME` on Linux
---   - `O_RANDOM` on Windows
---   - `O_SEQUENTIAL` on Windows
---   - `O_DIRECT` on Linux/FreeBSD/NetBSD/Windows
---
---   These values should be ignored:
---
---   - `O_RDONLY`, `O_WRONLY`, `O_RDWR`
---   - `O_CREAT`, `O_TRUNC`, `O_EXCL`
---   - `O_NOCTTY`
---
---   Returns `EBADF` if `fd` isn't open.
---
--- unix.fcntl(fd:int, unix.F_SETLK[, type[, start[, len[, whence]]]])
--- unix.fcntl(fd:int, unix.F_SETLKW[, type[, start[, len[, whence]]]])
---     ├─→ true
---     └─→ nil, unix.Errno
---
---   Acquires lock on file interval.
---
---   POSIX Advisory Locks allow multiple processes to leave voluntary
---   hints to each other about which portions of a file they're using.
---
---   The command may be:
---
---   - `F_SETLK` to acquire lock if possible
---   - `F_SETLKW` to wait for lock if necessary
---
---   `fd` is file descriptor of open() file.
---
---   `type` may be one of:
---
---   - `F_RDLCK` for read lock (default)
---   - `F_WRLCK` for read/write lock
---   - `F_UNLCK` to unlock
---
---   `start` is 0-indexed byte offset into file. The default is zero.
---
---   `len` is byte length of interval. Zero is the default and it means
---   until the end of the file.
---
---   `whence` may be one of:
---
---   - `SEEK_SET` start from beginning (default)
---   - `SEEK_CUR` start from current position
---   - `SEEK_END` start from end
---
---   Returns `EAGAIN` if lock couldn't be acquired. POSIX says this
---   theoretically could also be `EACCES` but we haven't seen this
---   behavior on any of our supported platforms.
---
---   Returns `EBADF` if `fd` wasn't open.
---
--- unix.fcntl(fd:int, unix.F_GETLK[, type[, start[, len[, whence]]]])
---     ├─→ unix.F_UNLCK
---     ├─→ type, start, len, whence, pid
---     └─→ nil, unix.Errno
---
---   Acquires information about POSIX advisory lock on file.
---
---   This function accepts the same parameters as fcntl(F_SETLK) and
---   tells you if the lock acquisition would be successful for a given
---   range of bytes. If locking would have succeeded, then F_UNLCK is
---   returned. If the lock would not have succeeded, then information
---   about a conflicting lock is returned.
---
---   Returned `type` may be `F_RDLCK` or `F_WRLCK`.
---
---   Returned `pid` is the process id of the current lock owner.
---
---   This function is currently not supported on Windows.
---
---   Returns `EBADF` if `fd` wasn't open.
---
---@param fd integer
---@param cmd integer
---@param ... any
---@return any ...
---@overload fun(fd: integer, unix.F_GETFD: integer): flags: integer
---@overload fun(fd: integer, unix.F_GETFD: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_SETFD: integer, flags: integer): true
---@overload fun(fd: integer, unix.F_SETFD: integer, flags: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_GETFL: integer): flags: integer
---@overload fun(fd: integer, unix.F_GETFL: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_SETFL: integer, flags: integer): true
---@overload fun(fd: integer, unix.F_SETFL: integer, flags: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_SETLK: integer, type?: integer, start?: integer, len?: integer, whence?: integer): true
---@overload fun(fd: integer, unix.F_SETLK: integer, type?: integer, start?: integer, len?: integer, whence?: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_SETLKW: integer, type?: integer, start?: integer, len?: integer, whence?: integer): true
---@overload fun(fd: integer, unix.F_SETLKW: integer, type?: integer, start?: integer, len?: integer, whence?: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unix.F_GETLK: integer, type?: integer, start?: integer, len?: integer, whence?: integer): unix.F_UNLCK: integer
---@overload fun(fd: integer, unix.F_GETLK: integer, type?: integer, start?: integer, len?: integer, whence?: integer): type: integer, start: integer, len: integer, whence: integer, pid: integer
---@overload fun(fd: integer, unix.F_GETLK: integer, type?: integer, start?: integer, len?: integer, whence?: integer): nil, error: unix.Errno
function unix.fcntl(fd, cmd, ...) end

---Gets session id.
---@param pid integer
---@return integer sid
---@nodiscard
---@overload fun(pid: integer): nil, error: unix.Errno
function unix.getsid(pid) end

--- Gets process group id.
---@return integer pgid
---@nodiscard
---@overload fun(): nil, error: unix.Errno
function unix.getpgrp() end

--- Sets process group id. This is the same as `setpgid(0,0)`.
---@return integer pgid
---@overload fun(): nil, error: unix.Errno
function unix.setpgrp() end

--- Sets process group id the modern way.
---@param pid integer
---@param pgid integer
---@return true
---@overload fun(pid: integer, pgid: integer): nil, error: unix.Errno
function unix.setpgid(pid, pgid) end

--- Gets process group id the modern way.
---@param pid integer
---@overload fun(pid: integer): nil, error: unix.Errno
function unix.getpgid(pid) end

--- Sets session id.
---
--- This function can be used to create daemons.
---
--- Fails with `ENOSYS` on Windows NT.
---@return integer sid
---@overload fun(): nil, error: unix.Errno
function unix.setsid() end

--- Gets real user id.
---
--- On Windows this system call is polyfilled by running `GetUserNameW()`
--- through Knuth's multiplicative hash.
---
--- This function does not fail.
---@return integer uid
---@nodiscard
function unix.getuid() end

--- Sets real group id.
---
--- On Windows this system call is polyfilled as getuid().
---
--- This function does not fail.
---@return integer gid
---@nodiscard
function unix.getgid() end

--- Gets effective user id.
---
--- For example, if your redbean is a setuid binary, then getuid() will
--- return the uid of the user running the program, and geteuid() shall
--- return zero which means root, assuming that's the file owning user.
---
--- On Windows this system call is polyfilled as getuid().
---
--- This function does not fail.
---@return integer uid
---@nodiscard
function unix.geteuid() end

--- Gets effective group id.
---
--- On Windows this system call is polyfilled as getuid().
---
--- This function does not fail.
---@return integer gid
---@nodiscard
function unix.getegid() end

--- Changes root directory.
---
--- Returns `ENOSYS` on Windows NT.
---@param path string
---@return true
---@overload fun(path: string): nil, error: unix.Errno
function unix.chroot(path) end

--- Sets user id.
---
--- One use case for this function is dropping root privileges. Should
--- you ever choose to run redbean as root and decide not to use the
--- `-G` and `-U` flags, you can replicate that behavior in the Lua
--- processes you spawn as follows:
---
---    ok, err = unix.setgid(1000)  -- check your /etc/groups
---    if not ok then Log(kLogFatal, tostring(err)) end
---    ok, err = unix.setuid(1000)  -- check your /etc/passwd
---    if not ok then Log(kLogFatal, tostring(err)) end
---
--- If your goal is to relinquish privileges because redbean is a setuid
--- binary, then things are more straightforward:
---
---    ok, err = unix.setgid(unix.getgid())
---    if not ok then Log(kLogFatal, tostring(err)) end
---    ok, err = unix.setuid(unix.getuid())
---    if not ok then Log(kLogFatal, tostring(err)) end
---
--- See also the setresuid() function and be sure to refer to your local
--- system manual about the subtleties of changing user id in a way that
--- isn't restorable.
---
--- Returns `ENOSYS` on Windows NT if `uid` isn't `getuid()`.
---@param uid integer
---@return true
---@overload fun(uid: integer): nil, error: unix.Errno
function unix.setuid(uid) end

---Sets user id for file system ops.
---@param uid integer
---@return true
---@overload fun(uid: integer): nil, error: unix.Errno
function unix.setfsuid(uid) end

---Sets group id.
---
---Returns `ENOSYS` on Windows NT if `gid` isn't `getgid()`.
---@param gid integer
---@return true
---@overload fun(gid: integer): nil, error: unix.Errno
function unix.setgid(gid) end

---Sets real, effective, and saved user ids.
---
---If any of the above parameters are -1, then it's a no-op.
---
---Returns `ENOSYS` on Windows NT.
---Returns `ENOSYS` on Macintosh and NetBSD if `saved` isn't -1.
---@param real integer
---@param effective integer
---@param saved integer
---@return true
---@overload fun(real: integer, effective: integer, saved: integer): nil, error: unix.Errno
function unix.setresuid(real, effective, saved) end

--- Sets real, effective, and saved group ids.
---
--- If any of the above parameters are -1, then it's a no-op.
---
--- Returns `ENOSYS` on Windows NT.
--- Returns `ENOSYS` on Macintosh and NetBSD if `saved` isn't -1.
---@param real integer
---@param effective integer
---@param saved integer
---@return true
---@overload fun(real: integer, effective: integer, saved: integer): nil, error: unix.Errno
function unix.setresgid(real, effective, saved) end

--- Sets file permission mask and returns the old one.
---
--- This is used to remove bits from the `mode` parameter of functions
--- like open() and mkdir(). The masks typically used are 027 and 022.
--- Those masks ensure that, even if a file is created with 0666 bits,
--- it'll be turned into 0640 or 0644 so that users other than the owner
--- can't modify it.
---
--- To read the mask without changing it, try doing this:
---
---     mask = unix.umask(027)
---     unix.umask(mask)
---
--- On Windows NT this is a no-op and `mask` is returned.
---
--- This function does not fail.
---@param newmask integer
---@return integer oldmask
function unix.umask(newmask) end

--- Generates a log message, which will be distributed by syslogd.
---
--- `priority` is a bitmask containing the facility value and the level
--- value. If no facility value is ORed into priority, then the default
--- value set by openlog() is used. If set to NULL, the program name is
--- used. Level is one of `LOG_EMERG`, `LOG_ALERT`, `LOG_CRIT`,
--- `LOG_ERR`, `LOG_WARNING`, `LOG_NOTICE`, `LOG_INFO`, `LOG_DEBUG`.
---
--- This function currently works on Linux, Windows, and NetBSD. On
--- WIN32 it uses the ReportEvent() facility.
---@param priority integer
---@param msg string
function unix.syslog(priority, msg) end

--- Returns nanosecond precision timestamp from system, e.g.
---
---    >: unix.clock_gettime()
---    1651137352      774458779
---    >: Benchmark(unix.clock_gettime)
---    126     393     571     1
---
--- `clock` can be any one of of:
---
--- - `CLOCK_REALTIME`: universally supported
--- - `CLOCK_REALTIME_FAST`: ditto but faster on freebsd
--- - `CLOCK_REALTIME_PRECISE`: ditto but better on freebsd
--- - `CLOCK_REALTIME_COARSE`: : like `CLOCK_REALTIME_FAST` but needs Linux 2.6.32+
--- - `CLOCK_MONOTONIC`: universally supported
--- - `CLOCK_MONOTONIC_FAST`: ditto but faster on freebsd
--- - `CLOCK_MONOTONIC_PRECISE`: ditto but better on freebsd
--- - `CLOCK_MONOTONIC_COARSE`: : like `CLOCK_MONOTONIC_FAST` but needs Linux 2.6.32+
--- - `CLOCK_MONOTONIC_RAW`: is actually monotonic but needs Linux 2.6.28+
--- - `CLOCK_PROCESS_CPUTIME_ID`: linux and bsd
--- - `CLOCK_THREAD_CPUTIME_ID`: linux and bsd
--- - `CLOCK_MONOTONIC_COARSE`: linux, freebsd
--- - `CLOCK_PROF`: linux and netbsd
--- - `CLOCK_BOOTTIME`: linux and openbsd
--- - `CLOCK_REALTIME_ALARM`: linux-only
--- - `CLOCK_BOOTTIME_ALARM`: linux-only
--- - `CLOCK_TAI`: linux-only
---
--- Returns `EINVAL` if clock isn't supported on platform.
---
--- This function only fails if `clock` is invalid.
---
--- This function goes fastest on Linux and Windows.
---@param clock? integer
---@return integer seconds, integer nanos
---@nodiscard
---@overload fun(clock?: integer): nil, error: unix.Errno
function unix.clock_gettime(clock) end

--- Sleeps with nanosecond precision.
---
--- Returns `EINTR` if a signal was received while waiting.
---@param seconds integer
---@param nanos integer?
---@return integer remseconds, integer remnanos
---@overload fun(seconds: integer, nanos?: integer): nil, error: unix.Errno
function unix.nanosleep(seconds, nanos) end

--- These functions are used to make programs slower by asking the
--- operating system to flush data to the physical medium.
function unix.sync() end

--- These functions are used to make programs slower by asking the
--- operating system to flush data to the physical medium.
---@param fd integer
---@return true
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.fsync(fd) end

--- These functions are used to make programs slower by asking the
--- operating system to flush data to the physical medium.
---@param fd integer
---@return true
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.fdatasync(fd) end

--- Seeks to file position.
---
--- `whence` can be one of:
---
--- - `SEEK_SET`: Sets the file position to `offset` [default]
--- - `SEEK_CUR`: Sets the file position to `position + offset`
--- - `SEEK_END`: Sets the file position to `filesize + offset`
---
--- Returns the new position relative to the start of the file.
---@param fd integer
---@param offset integer
---@param whence? integer
---@return integer newposbytes
---@overload fun(fd: integer, offset: integer, whence?: integer): nil, error: unix.Errno
function unix.lseek(fd, offset, whence) end

--- Reduces or extends underlying physical medium of file.
--- If file was originally larger, content >length is lost.
---@param path string
---@param length? integer defaults to zero (`0`)
---@return true
---@overload fun(path: string, length?: integer): nil, error: unix.Errno
function unix.truncate(path, length) end

--- Reduces or extends underlying physical medium of open file.
--- If file was originally larger, content >length is lost.
---@param fd integer
---@param length? integer defaults to zero (`0`)
---@return true
---@overload fun(fd: integer, length?: integer): nil, error: unix.Errno
function unix.ftruncate(fd, length) end

---@param family? integer defaults to `AF_INET` and can be:
---
--- - `AF_INET`: Creates Internet Protocol Version 4 (IPv4) socket.
---
--- - `AF_UNIX`: Creates local UNIX domain socket. On the New Technology
--- this requires Windows 10 and only works with `SOCK_STREAM`.
---
---@param type? integer defaults to `SOCK_STREAM` and can be:
---
--- - `SOCK_STREAM`
--- - `SOCK_DGRAM`
--- - `SOCK_RAW`
--- - `SOCK_RDM`
--- - `SOCK_SEQPACKET`
---
--- You may bitwise OR any of the following into `type`:
---
--- - `SOCK_CLOEXEC`
--- - `SOCK_NONBLOCK`
---
---@param protocol? integer may be any of:
---
--- - `0` to let kernel choose [default]
--- - `IPPROTO_TCP`
--- - `IPPROTO_UDP`
--- - `IPPROTO_RAW`
--- - `IPPROTO_IP`
--- - `IPPROTO_ICMP`
---
---@return integer fd
---@nodiscard
---@overload fun(family?: integer, type?: integer, protocol?: integer): nil, error: unix.Errno
function unix.socket(family, type, protocol) end

--- Creates bidirectional pipe.
---
---@param family? integer defaults to `AF_UNIX`.
---@param type? integer defaults to `SOCK_STREAM` and can be:
---
--- - `SOCK_STREAM`
--- - `SOCK_DGRAM`
--- - `SOCK_SEQPACKET`
---
--- You may bitwise OR any of the following into `type`:
---
--- - `SOCK_CLOEXEC`
--- - `SOCK_NONBLOCK`
---
---@param protocol? integer defaults to `0`.
---@return integer fd1, integer fd2
---@overload fun(family?: integer, type?: integer, protocol?: integer): nil, error: unix.Errno
function unix.socketpair(family, type, protocol) end

---  Binds socket.
---
---  `ip` and `port` are in host endian order. For example, if you
---  wanted to listen on `1.2.3.4:31337` you could do any of these
---
---      unix.bind(sock, 0x01020304, 31337)
---      unix.bind(sock, ParseIp('1.2.3.4'), 31337)
---      unix.bind(sock, 1 << 24 | 0 << 16 | 0 << 8 | 1, 31337)
---
---  `ip` and `port` both default to zero. The meaning of bind(0, 0)
---  is to listen on all interfaces with a kernel-assigned ephemeral
---  port number, that can be retrieved and used as follows:
---
---      sock = assert(unix.socket())  -- create ipv4 tcp socket
---      assert(unix.bind(sock))       -- all interfaces ephemeral port
---      ip, port = assert(unix.getsockname(sock))
---      print("listening on ip", FormatIp(ip), "port", port)
---      assert(unix.listen(sock))
---      while true do
---         client, clientip, clientport = assert(unix.accept(sock))
---         print("got client ip", FormatIp(clientip), "port", clientport)
---         unix.close(client)
---      end
---
---  Further note that calling `unix.bind(sock)` is equivalent to not
---  calling bind() at all, since the above behavior is the default.
---@param fd integer
---@param ip? uint32
---@param port? uint16
---@return true
---@overload fun(fd: integer, unixpath: string): true
---@overload fun(fd: integer, ip?: integer, port?: integer): nil, error: unix.Errno
---@overload fun(fd: integer, unixpath: string): nil, error: unix.Errno
function unix.bind(fd, ip, port) end

---Returns list of network adapter addresses.
---@return { name: string, ip: integer, netmask: integer }[] addresses
---@nodiscard
---@overload fun(): nil, error: unix.Errno
function unix.siocgifconf() end

--- Tunes networking parameters.
---
--- `level` and `optname` may be one of the following pairs. The ellipses
--- type signature above changes depending on which options are used.
---
--- `optname` is the option feature magic number. The constants for
--- these will be set to `0` if the option isn't supported on the host
--- platform.
---
--- Raises `ENOPROTOOPT` if your `level` / `optname` combination isn't
--- valid, recognized, or supported on the host platform.
---
--- Raises `ENOTSOCK` if `fd` is valid but isn't a socket.
---
--- Raises `EBADF` if `fd` isn't valid.
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ value:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, value:bool)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_TYPE`
--- - `SOL_SOCKET`, `SO_DEBUG`
--- - `SOL_SOCKET`, `SO_ACCEPTCONN`
--- - `SOL_SOCKET`, `SO_BROADCAST`
--- - `SOL_SOCKET`, `SO_REUSEADDR`
--- - `SOL_SOCKET`, `SO_REUSEPORT`
--- - `SOL_SOCKET`, `SO_KEEPALIVE`
--- - `SOL_SOCKET`, `SO_DONTROUTE`
--- - `SOL_TCP`, `TCP_NODELAY`
--- - `SOL_TCP`, `TCP_CORK`
--- - `SOL_TCP`, `TCP_QUICKACK`
--- - `SOL_TCP`, `TCP_FASTOPEN_CONNECT`
--- - `SOL_TCP`, `TCP_DEFER_ACCEPT`
--- - `SOL_IP`, `IP_HDRINCL`
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ value:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, value:int)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_SNDBUF`
--- - `SOL_SOCKET`, `SO_RCVBUF`
--- - `SOL_SOCKET`, `SO_RCVLOWAT`
--- - `SOL_SOCKET`, `SO_SNDLOWAT`
--- - `SOL_TCP`, `TCP_KEEPIDLE`
--- - `SOL_TCP`, `TCP_KEEPINTVL`
--- - `SOL_TCP`, `TCP_FASTOPEN`
--- - `SOL_TCP`, `TCP_KEEPCNT`
--- - `SOL_TCP`, `TCP_MAXSEG`
--- - `SOL_TCP`, `TCP_SYNCNT`
--- - `SOL_TCP`, `TCP_NOTSENT_LOWAT`
--- - `SOL_TCP`, `TCP_WINDOW_CLAMP`
--- - `SOL_IP`, `IP_TOS`
--- - `SOL_IP`, `IP_MTU`
--- - `SOL_IP`, `IP_TTL`
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ secs:int, nsecs:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, secs:int[, nanos:int])
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_RCVTIMEO`: If this option is specified then
---   your stream socket will have a read() / recv() timeout. If the
---   specified interval elapses without receiving data, then EAGAIN
---   shall be returned by read. If this option is used on listening
---   sockets, it'll be inherited by accepted sockets. Your redbean
---   already does this for GetClientFd() based on the `-t` flag.
---
--- - `SOL_SOCKET`, `SO_SNDTIMEO`: This is the same as `SO_RCVTIMEO`
---   but it applies to the write() / send() functions.
---
--- unix.getsockopt(fd:int, unix.SOL_SOCKET, unix.SO_LINGER)
---     ├─→ seconds:int, enabled:bool
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, unix.SOL_SOCKET, unix.SO_LINGER, secs:int, enabled:bool)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- This `SO_LINGER` parameter can be used to make close() a blocking
--- call. Normally when the kernel returns immediately when it receives
--- close(). Sometimes it's desirable to have extra assurance on errors
--- happened, even if it comes at the cost of performance.
---
--- unix.setsockopt(serverfd:int, unix.SOL_TCP, unix.TCP_SAVE_SYN, enabled:int)
---     ├─→ true
---     └─→ nil, unix.Errno
--- unix.getsockopt(clientfd:int, unix.SOL_TCP, unix.TCP_SAVED_SYN)
---     ├─→ syn_packet_bytes:str
---     └─→ nil, unix.Errno
---
--- This `TCP_SAVED_SYN` option may be used to retrieve the bytes of the
--- TCP SYN packet that the client sent when the connection for `fd` was
--- opened. In order for this to work, `TCP_SAVE_SYN` must have been set
--- earlier on the listening socket. This is Linux-only. You can use the
--- `OnServerListen` hook to enable SYN saving in your Redbean. When the
--- `TCP_SAVE_SYN` option isn't used, this may return empty string.
---@param fd integer
---@param level integer
---@param optname integer
---@return integer value
---@nodiscard
---@overload fun(fd: integer, level: integer, optname: integer): nil, error: unix.Errno
---@overload fun(fd:integer, unix.SOL_SOCKET: integer, unix.SO_LINGER: integer): seconds: integer, enabled: boolean
---@overload fun(fd:integer, unix.SOL_SOCKET: integer, unix.SO_LINGER: integer): nil, error: unix.Errno
---@overload fun(serverfd:integer, unix.SOL_TCP: integer, unix.TCP_SAVE_SYN: integer): syn_packet_bytes: string
---@overload fun(serverfd:integer, unix.SOL_TCP: integer, unix.TCP_SAVE_SYN: integer): nil, error: unix.Errno
function unix.getsockopt(fd, level, optname) end

--- Tunes networking parameters.
---
--- `level` and `optname` may be one of the following pairs. The ellipses
--- type signature above changes depending on which options are used.
---
--- `optname` is the option feature magic number. The constants for
--- these will be set to `0` if the option isn't supported on the host
--- platform.
---
--- Raises `ENOPROTOOPT` if your `level` / `optname` combination isn't
--- valid, recognized, or supported on the host platform.
---
--- Raises `ENOTSOCK` if `fd` is valid but isn't a socket.
---
--- Raises `EBADF` if `fd` isn't valid.
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ value:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, value:bool)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_TYPE`
--- - `SOL_SOCKET`, `SO_DEBUG`
--- - `SOL_SOCKET`, `SO_ACCEPTCONN`
--- - `SOL_SOCKET`, `SO_BROADCAST`
--- - `SOL_SOCKET`, `SO_REUSEADDR`
--- - `SOL_SOCKET`, `SO_REUSEPORT`
--- - `SOL_SOCKET`, `SO_KEEPALIVE`
--- - `SOL_SOCKET`, `SO_DONTROUTE`
--- - `SOL_TCP`, `TCP_NODELAY`
--- - `SOL_TCP`, `TCP_CORK`
--- - `SOL_TCP`, `TCP_QUICKACK`
--- - `SOL_TCP`, `TCP_FASTOPEN_CONNECT`
--- - `SOL_TCP`, `TCP_DEFER_ACCEPT`
--- - `SOL_IP`, `IP_HDRINCL`
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ value:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, value:int)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_SNDBUF`
--- - `SOL_SOCKET`, `SO_RCVBUF`
--- - `SOL_SOCKET`, `SO_RCVLOWAT`
--- - `SOL_SOCKET`, `SO_SNDLOWAT`
--- - `SOL_TCP`, `TCP_KEEPIDLE`
--- - `SOL_TCP`, `TCP_KEEPINTVL`
--- - `SOL_TCP`, `TCP_FASTOPEN`
--- - `SOL_TCP`, `TCP_KEEPCNT`
--- - `SOL_TCP`, `TCP_MAXSEG`
--- - `SOL_TCP`, `TCP_SYNCNT`
--- - `SOL_TCP`, `TCP_NOTSENT_LOWAT`
--- - `SOL_TCP`, `TCP_WINDOW_CLAMP`
--- - `SOL_IP`, `IP_TOS`
--- - `SOL_IP`, `IP_MTU`
--- - `SOL_IP`, `IP_TTL`
---
--- unix.getsockopt(fd:int, level:int, optname:int)
---     ├─→ secs:int, nsecs:int
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, level:int, optname:int, secs:int[, nanos:int])
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- - `SOL_SOCKET`, `SO_RCVTIMEO`: If this option is specified then
---   your stream socket will have a read() / recv() timeout. If the
---   specified interval elapses without receiving data, then EAGAIN
---   shall be returned by read. If this option is used on listening
---   sockets, it'll be inherited by accepted sockets. Your redbean
---   already does this for GetClientFd() based on the `-t` flag.
---
--- - `SOL_SOCKET`, `SO_SNDTIMEO`: This is the same as `SO_RCVTIMEO`
---   but it applies to the write() / send() functions.
---
--- unix.getsockopt(fd:int, unix.SOL_SOCKET, unix.SO_LINGER)
---     ├─→ seconds:int, enabled:bool
---     └─→ nil, unix.Errno
--- unix.setsockopt(fd:int, unix.SOL_SOCKET, unix.SO_LINGER, secs:int, enabled:bool)
---     ├─→ true
---     └─→ nil, unix.Errno
---
--- This `SO_LINGER` parameter can be used to make close() a blocking
--- call. Normally when the kernel returns immediately when it receives
--- close(). Sometimes it's desirable to have extra assurance on errors
--- happened, even if it comes at the cost of performance.
---
--- unix.setsockopt(serverfd:int, unix.SOL_TCP, unix.TCP_SAVE_SYN, enabled:int)
---     ├─→ true
---     └─→ nil, unix.Errno
--- unix.getsockopt(clientfd:int, unix.SOL_TCP, unix.TCP_SAVED_SYN)
---     ├─→ syn_packet_bytes:str
---     └─→ nil, unix.Errno
---
--- This `TCP_SAVED_SYN` option may be used to retrieve the bytes of the
--- TCP SYN packet that the client sent when the connection for `fd` was
--- opened. In order for this to work, `TCP_SAVE_SYN` must have been set
--- earlier on the listening socket. This is Linux-only. You can use the
--- `OnServerListen` hook to enable SYN saving in your Redbean. When the
--- `TCP_SAVE_SYN` option isn't used, this may return empty string.
---@param fd integer
---@param level integer
---@param optname integer
---@param value boolean|integer
---@return true
---@overload fun(fd: integer, level: integer, optname: integer, value: boolean|integer): nil, error: unix.Errno
---@overload fun(fd:integer, unix.SOL_SOCKET: integer, unix.SO_LINGER: integer, secs:integer, enabled:boolean): true
---@overload fun(fd:integer, unix.SOL_SOCKET: integer, unix.SO_LINGER: integer, secs:integer, enabled:boolean): nil, error: unix.Errno
---@overload fun(serverfd:integer, unix.SOL_TCP: integer, unix.TCP_SAVE_SYN: integer, enabled:integer): true
---@overload fun(serverfd:integer, unix.SOL_TCP: integer, unix.TCP_SAVE_SYN: integer, enabled:integer): nil, error: unix.Errno
function unix.setsockopt(fd, level, optname, value) end

--- Checks for events on a set of file descriptors.
---
--- The table of file descriptors to poll uses sparse integer keys. Any
--- pairs with non-integer keys will be ignored. Pairs with negative
--- keys are ignored by poll(). The returned table will be a subset of
--- the supplied file descriptors.
---
--- `events` and `revents` may be any combination (using bitwise OR) of:
---
--- - `POLLIN` (events, revents): There is data to read.
--- - `POLLOUT` (events, revents): Writing is now possible, although may
--- still block if available space in a socket or pipe is exceeded
--- (unless `O_NONBLOCK` is set).
--- - `POLLPRI` (events, revents): There is some exceptional condition
--- (for example, out-of-band data on a TCP socket).
--- - `POLLRDHUP` (events, revents): Stream socket peer closed
--- connection, or shut down writing half of connection.
--- - `POLLERR` (revents): Some error condition.
--- - `POLLHUP` (revents): Hang up. When reading from a channel such as
--- a pipe or a stream socket, this event merely indicates that the
--- peer closed its end of the channel.
--- - `POLLNVAL` (revents): Invalid request.
---
---@param fds table<integer,integer> `{[fd:int]=events:int, ...}`
---@param timeoutms integer? the number of milliseconds to block.
--- If this is set to -1 then that means block as long as it takes until there's an
--- event or an interrupt. If the timeout expires, an empty table is returned.
---@return table<integer,integer> `{[fd:int]=revents:int, ...}`
---@nodiscard
---@overload fun(fds: table<integer,integer>, timeoutms:integer): nil, unix.Errno
function unix.poll(fds, timeoutms) end

--- Returns hostname of system.
---@return string host
---@nodiscard
---@overload fun(): nil, unix.Errno
function unix.gethostname() end

--- Begins listening for incoming connections on a socket.
---@param fd integer
---@param backlog integer?
---@return true
---@overload fun(fd:integer, backlog:integer?): nil, unix.Errno
function unix.listen(fd, backlog) end

--- Accepts new client socket descriptor for a listening tcp socket.
---
--- `flags` may have any combination (using bitwise OR) of:
---
--- - `SOCK_CLOEXEC`
--- - `SOCK_NONBLOCK`
---
---@param serverfd integer
---@param flags integer?
---@return integer clientfd, uint32 ip, uint16 port
---@nodiscard
---@overload fun(serverfd:integer, flags:integer?):clientfd:integer, unixpath:string
---@overload fun(serverfd:integer, flags:integer?):nil, unix.Errno
function unix.accept(serverfd, flags) end

---  Connects a TCP socket to a remote host.
---
---  With TCP this is a blocking operation. For a UDP socket it simply
---  remembers the intended address so that `send()` or `write()` may be used
---  rather than `sendto()`.
---@param fd integer
---@param ip uint32
---@param port uint16
---@return true
---@overload fun(fd:integer, ip:integer, port:integer): nil, unix.Errno
---@overload fun(fd:integer, unixpath:string): true
---@overload fun(fd:integer, unixpath:string): nil, unix.Errno
function unix.connect(fd, ip, port) end

--- Retrieves the local address of a socket.
---@param fd integer
---@return uint32 ip, uint16 port
---@nodiscard
---@overload fun(fd: integer): unixpath:string
---@overload fun(fd: integer): nil, unix.Errno
function unix.getsockname(fd) end

--- Retrieves the remote address of a socket.
---
--- This operation will either fail on `AF_UNIX` sockets or return an
--- empty string.
---
---@param fd integer
---@return uint32 ip, uint16 port
---@nodiscard
---@overload fun(fd: integer): unixpath:string
---@overload fun(fd: integer): nil, unix.Errno
function unix.getpeername(fd) end

---@param fd integer
---@param bufsiz integer?
---@param flags integer? may have any combination (using bitwise OR) of:
--- - `MSG_WAITALL`
--- - `MSG_DONTROUTE`
--- - `MSG_PEEK`
--- - `MSG_OOB`
---@return string data
---@nodiscard
---@overload fun(fd: integer, bufsiz?: integer, flags?: integer): nil, unix.Errno
function unix.recv(fd, bufsiz, flags) end

---@param fd integer
---@param bufsiz integer?
---@param flags integer? may have any combination (using bitwise OR) of:
--- - `MSG_WAITALL`
--- - `MSG_DONTROUTE`
--- - `MSG_PEEK`
--- - `MSG_OOB`
---@return string data, integer ip, integer port
---@nodiscard
---@overload fun(fd: integer, bufsiz?: integer, flags?: integer): data: string, unixpath: string
---@overload fun(fd: integer, bufsiz?: integer, flags?: integer): nil, unix.Errno
function unix.recvfrom(fd, bufsiz, flags) end

--- This is the same as `write` except it has a `flags` argument
--- that's intended for sockets.
---@param fd integer
---@param data string
---@param flags integer? may have any combination (using bitwise OR) of:
--- - `MSG_NOSIGNAL`: Don't SIGPIPE on EOF
--- - `MSG_OOB`: Send stream data through out of bound channel
--- - `MSG_DONTROUTE`: Don't go through gateway (for diagnostics)
--- - `MSG_MORE`: Manual corking to belay nodelay (0 on non-Linux)
---@return integer sent
---@overload fun(fd: integer, data: string, flags?: integer): nil, unix.Errno
function unix.send(fd, data, flags) end

--- This is useful for sending messages over UDP sockets to specific
--- addresses.
---
---@param fd integer
---@param data string
---@param ip uint32
---@param port uint16
---@param flags? integer may have any combination (using bitwise OR) of:
--- - `MSG_OOB`
--- - `MSG_DONTROUTE`
--- - `MSG_NOSIGNAL`
---@return integer sent
---@overload fun(fd:integer, data:string, ip:integer, port:integer, flags?:integer): nil, unix.Errno
---@overload fun(fd:integer, data:string, unixpath:string, flags?:integer): sent: integer
---@overload fun(fd:integer, data:string, unixpath:string, flags?:integer): nil, unix.Errno
function unix.sendto(fd, data, ip, port, flags) end

--- Partially closes socket.
---
---@param fd integer
---@param how integer is set to one of:
---
--- - `SHUT_RD`: sends a tcp half close for reading
--- - `SHUT_WR`: sends a tcp half close for writing
--- - `SHUT_RDWR`
---
--- This system call currently has issues on Macintosh, so portable code
--- should log rather than assert failures reported by `shutdown()`.
---
---@return true
---@overload fun(fd: integer, how: integer): nil, error: unix.Errno
function unix.shutdown(fd, how) end

--- Manipulates bitset of signals blocked by process.
---
---@param how integer can be one of:
---
--- - `SIG_BLOCK`: applies `mask` to set of blocked signals using bitwise OR
--- - `SIG_UNBLOCK`: removes bits in `mask` from set of blocked signals
--- - `SIG_SETMASK`: replaces process signal mask with `mask`
---
--- `mask` is a unix.Sigset() object (see section below).
---
--- For example, to temporarily block `SIGTERM` and `SIGINT` so critical
--- work won't be interrupted, sigprocmask() can be used as follows:
---
---   newmask = unix.Sigset(unix.SIGTERM)
---   oldmask = assert(unix.sigprocmask(unix.SIG_BLOCK, newmask))
---   -- do something...
---   assert(unix.sigprocmask(unix.SIG_SETMASK, oldmask))
---
---@param newmask unix.Sigset
---@return unix.Sigset oldmask
---@overload fun(how: integer, newmask: unix.Sigset): nil, error: unix.Errno
function unix.sigprocmask(how, newmask) end

---@param sig integer can be one of:
---
--- - `unix.SIGINT`
--- - `unix.SIGQUIT`
--- - `unix.SIGTERM`
--- - etc.
---
---@param handler? function|integer can be:
---
--- - Lua function
--- - `unix.SIG_IGN`
--- - `unix.SIG_DFL`
---
---@param flags? integer can have:
---
--- - `unix.SA_RESTART`: Enables BSD signal handling semantics. Normally
--- i/o entrypoints check for pending signals to deliver. If one gets
--- delivered during an i/o call, the normal behavior is to cancel the
--- i/o operation and return -1 with `EINTR` in errno. If you use the
--- `SA_RESTART` flag then that behavior changes, so that any function
--- that's been annotated with @restartable will not return `EINTR`
--- and will instead resume the i/o operation. This makes coding
--- easier but it can be an anti-pattern if not used carefully, since
--- poor usage can easily result in latency issues. It also requires
--- one to do more work in signal handlers, so special care needs to
--- be given to which C library functions are @asyncsignalsafe.
---
--- - `unix.SA_RESETHAND`: Causes signal handler to be single-shot. This
--- means that, upon entry of delivery to a signal handler, it's reset
--- to the `SIG_DFL` handler automatically. You may use the alias
--- `SA_ONESHOT` for this flag, which means the same thing.
---
--- - `unix.SA_NODEFER`: Disables the reentrancy safety check on your signal
--- handler. Normally that's a good thing, since for instance if your
--- `SIGSEGV` signal handler happens to segfault, you're going to want
--- your process to just crash rather than looping endlessly. But in
--- some cases it's desirable to use `SA_NODEFER` instead, such as at
--- times when you wish to `longjmp()` out of your signal handler and
--- back into your program. This is only safe to do across platforms
--- for non-crashing signals such as `SIGCHLD` and `SIGINT`. Crash
--- handlers should use Xed instead to recover execution, because on
--- Windows a `SIGSEGV` or `SIGTRAP` crash handler might happen on a
--- separate stack and/or a separate thread. You may use the alias
--- `SA_NOMASK` for this flag, which means the same thing.
---
--- - `unix.SA_NOCLDWAIT`: Changes `SIGCHLD` so the zombie is gone and
--- you can't call wait() anymore; similar but may still deliver the
--- SIGCHLD.
---
--- - `unix.SA_NOCLDSTOP`: Lets you set `SIGCHLD` handler that's only
--- notified on exit/termination and not notified on `SIGSTOP`,
--- `SIGTSTP`, `SIGTTIN`, `SIGTTOU`, or `SIGCONT`.
---
--- Example:
---
---     function OnSigUsr1(sig)
---         gotsigusr1 = true
---     end
---     gotsigusr1 = false
---     oldmask = assert(unix.sigprocmask(unix.SIG_BLOCK, unix.Sigset(unix.SIGUSR1)))
---     assert(unix.sigaction(unix.SIGUSR1, OnSigUsr1))
---     assert(unix.raise(unix.SIGUSR1))
---     assert(not gotsigusr1)
---     ok, err = unix.sigsuspend(oldmask)
---     assert(not ok)
---     assert(err:errno() == unix.EINTR)
---     assert(gotsigusr1)
---     assert(unix.sigprocmask(unix.SIG_SETMASK, oldmask))
---
--- It's a good idea to not do too much work in a signal handler.
---
---@param mask? unix.Sigset
---@return function|integer oldhandler, integer flags, unix.Sigset mask
---@overload fun(sig: integer, handler?: function|integer, flags?: integer, mask?: unix.Sigset): nil, error: unix.Errno
function unix.sigaction(sig, handler, flags, mask) end

--- Waits for signal to be delivered.
---
--- The signal mask is temporarily replaced with `mask` during this system call.
---@param mask? unix.Sigset specifies which signals should be blocked.
---@return nil, unix.Errno error
function unix.sigsuspend(mask) end

--- Causes `SIGALRM` signals to be generated at some point(s) in the
--- future. The `which` parameter should be `ITIMER_REAL`.
---
--- Here's an example of how to create a 400 ms interval timer:
---
---     ticks = 0
---     assert(unix.sigaction(unix.SIGALRM, function(sig)
---        print('tick no. %d' % {ticks})
---        ticks = ticks + 1
---     end))
---     assert(unix.setitimer(unix.ITIMER_REAL, 0, 400e6, 0, 400e6))
---     while true do
---        unix.sigsuspend()
---     end
---
--- Here's how you'd do a single-shot timeout in 1 second:
---
---     unix.sigaction(unix.SIGALRM, MyOnSigAlrm, unix.SA_RESETHAND)
---     unix.setitimer(unix.ITIMER_REAL, 0, 0, 1, 0)
---
---@param which integer
---@param intervalsec integer
---@param intervalns integer needs to be on the interval `[0,1000000000)`
---@param valuesec integer
---@param valuens integer needs to be on the interval `[0,1000000000)`
---@return integer intervalsec, integer intervalns, integer valuesec, integer valuens
---@overload fun(which: integer): intervalsec: integer, intervalns: integer, valuesec: integer, valuens: integer
---@overload fun(which: integer, intervalsec: integer, intervalns: integer, valuesec: integer, valuens: integer): nil, error: unix.Errno
---@overload fun(which: integer): nil, error: unix.Errno
function unix.setitimer(which, intervalsec, intervalns, valuesec, valuens) end

--- Turns platform-specific `sig` code into its symbolic name.
---
--- For example:
---
---     >: unix.strsignal(9)
---     "SIGKILL"
---     >: unix.strsignal(unix.SIGKILL)
---     "SIGKILL"
---
--- Please note that signal numbers are normally different across
--- supported platforms, and the constants should be preferred.
---
---@param sig integer
---@return string signalname
---@nodiscard
function unix.strsignal(sig) end

--- Changes resource limit.
---
---@param resource integer may be one of:
---
--- - `RLIMIT_AS` limits the size of the virtual address space. This
--- will work on all platforms. It's emulated on XNU and Windows which
--- means it won't propagate across execve() currently.
---
--- - `RLIMIT_CPU` causes `SIGXCPU` to be sent to the process when the
--- soft limit on CPU time is exceeded, and the process is destroyed
--- when the hard limit is exceeded. It works everywhere but Windows
--- where it should be possible to poll getrusage() with setitimer().
---
--- - `RLIMIT_FSIZE` causes `SIGXFSZ` to sent to the process when the
--- soft limit on file size is exceeded and the process is destroyed
--- when the hard limit is exceeded. It works everywhere but Windows.
---
--- - `RLIMIT_NPROC` limits the number of simultaneous processes and it
--- should work on all platforms except Windows. Please be advised it
--- limits the process, with respect to the activities of the user id
--- as a whole.
---
--- - `RLIMIT_NOFILE` limits the number of open file descriptors and it
--- should work on all platforms except Windows (TODO).
---
--- If a limit isn't supported by the host platform, it'll be set to
--- 127. On most platforms these limits are enforced by the kernel and
--- as such are inherited by subprocesses.
---
---@param soft integer
---@param hard? integer defaults to whatever was specified in `soft`.
---@return true
---@overload fun(resource: integer, soft: integer, hard?: integer): nil, error: unix.Errno
function unix.setrlimit(resource, soft, hard) end

--- Returns information about resource limits for current process.
---@param resource integer
---@return integer soft, integer hard
---@nodiscard
---@overload fun(resource: integer): nil, error: unix.Errno
function unix.getrlimit(resource) end

--- Returns information about resource usage for current process, e.g.
---
---     >: unix.getrusage()
---     {utime={0, 53644000}, maxrss=44896, minflt=545, oublock=24, nvcsw=9}
---
---@param who? integer defaults to `RUSAGE_SELF` and can be any of:
---
--- - `RUSAGE_SELF`: current process
--- - `RUSAGE_THREAD`: current thread
--- - `RUSAGE_CHILDREN`: not supported on Windows NT
--- - `RUSAGE_BOTH`: not supported on non-Linux
---
---@return unix.Rusage # See `unix.Rusage` for details on returned fields.
---@nodiscard
---@overload fun(who?: integer): nil, error: unix.Errno
function unix.getrusage(who) end

--- Restrict system operations.
---
--- This can be used to sandbox your redbean workers. It allows finer
--- customization compared to the `-S` flag.
---
--- Pledging causes most system calls to become unavailable. On Linux the
--- disabled calls will return EPERM whereas OpenBSD kills the process.
---
--- Using pledge is irreversible. On Linux it causes PR_SET_NO_NEW_PRIVS
--- to be set on your process.
---
--- By default exit and exit_group are always allowed. This is useful
--- for processes that perform pure computation and interface with the
--- parent via shared memory.
---
--- Once pledge is in effect, the chmod functions (if allowed) will not
--- permit the sticky/setuid/setgid bits to change. Linux will EPERM here
--- and OpenBSD should ignore those three bits rather than crashing.
---
--- User and group IDs also can't be changed once pledge is in effect.
--- OpenBSD should ignore the chown functions without crashing. Linux
--- will just EPERM.
---
--- Memory functions won't permit creating executable code after pledge.
--- Restrictions on origin of SYSCALL instructions will become enforced
--- on Linux (cf. msyscall) after pledge too, which means the process
--- gets killed if SYSCALL is used outside the .privileged section. One
--- exception is if the "exec" group is specified, in which case these
--- restrictions need to be loosened.
---
---@param promises? string may include any of the following groups delimited by spaces.
--- This list has been curated to focus on the
--- system calls for which this module provides wrappers. See the
--- Cosmopolitan Libc pledge() documentation for a comprehensive and
--- authoritative list of raw system calls. Having the raw system call
--- list may be useful if you're executing foreign programs.
---
--- ### stdio
---
--- Allows read, write, send, recv, recvfrom, close, clock_getres,
--- clock_gettime, dup, fchdir, fstat, fsync, fdatasync, ftruncate,
--- getdents, getegid, getrandom, geteuid, getgid, getgroups,
--- getitimer, getpgid, getpgrp, getpid, hgetppid, getresgid,
--- getresuid, getrlimit, getsid, gettimeofday, getuid, lseek,
--- madvise, brk, mmap/mprotect (PROT_EXEC isn't allowed), msync,
--- munmap, gethostname, nanosleep, pipe, pipe2, poll, setitimer,
--- shutdown, sigaction, sigsuspend, sigprocmask, socketpair, umask,
--- wait4, getrusage, ioctl(FIONREAD), ioctl(FIONBIO), ioctl(FIOCLEX),
--- ioctl(FIONCLEX), fcntl(F_GETFD), fcntl(F_SETFD), fcntl(F_GETFL),
--- fcntl(F_SETFL).
---
--- ### rpath
---
--- Allows chdir, getcwd, open, stat, fstat, access, readlink, chmod,
--- chmod, fchmod.
---
--- ### wpath
---
--- Allows getcwd, open, stat, fstat, access, readlink, chmod, fchmod.
---
--- ### cpath
---
--- Allows rename, link, symlink, unlink, mkdir, rmdir.
---
--- ### fattr
---
--- Allows chmod, fchmod, utimensat, futimens.
---
--- ### flock
---
--- Allows flock, fcntl(F_GETLK), fcntl(F_SETLK), fcntl(F_SETLKW).
---
--- ### tty
---
--- Allows isatty, tiocgwinsz, tcgets, tcsets, tcsetsw, tcsetsf.
---
--- ### inet
---
--- Allows socket (AF_INET), listen, bind, connect, accept,
--- getpeername, getsockname, setsockopt, getsockopt.
---
--- ### unix
---
--- Allows socket (AF_UNIX), listen, bind, connect, accept,
--- getpeername, getsockname, setsockopt, getsockopt.
---
--- ### dns
---
--- Allows sendto, recvfrom, socket(AF_INET), connect.
---
--- ### recvfd
---
--- Allows recvmsg, recvmmsg.
---
--- ### sendfd
---
--- Allows sendmsg, sendmmsg.
---
--- ### proc
---
--- Allows fork, vfork, clone, kill, tgkill, getpriority, setpriority,
--- setrlimit, setpgid, setsid.
---
--- ### id
---
--- Allows setuid, setreuid, setresuid, setgid, setregid, setresgid,
--- setgroups, setrlimit, getpriority, setpriority.
---
--- ### settime
---
--- Allows settimeofday and clock_adjtime.
---
--- ### unveil
---
--- Allows unveil().
---
--- ### exec
---
--- Allows execve.
---
--- If the executable in question needs a loader, then you will need
--- "rpath prot_exec" too. With APE, security is strongest when you
--- assimilate your binaries beforehand, using the --assimilate flag,
--- or the o//tool/build/assimilate.com program. On OpenBSD this is
--- mandatory.
---
--- ### prot_exec
---
--- Allows mmap(PROT_EXEC) and mprotect(PROT_EXEC).
---
--- This may be needed to launch non-static non-native executables,
--- such as non-assimilated APE binaries, or programs that link
--- dynamic shared objects, i.e. most Linux distro binaries.
---
---@param execpromises? string only matters if "exec" is specified in `promises`.
--- In that case, this specifies the promises that'll apply once `execve()`
--- happens. If this is `NULL` then the default is used, which is
--- unrestricted. OpenBSD allows child processes to escape the sandbox
--- (so a pledged OpenSSH server process can do things like spawn a root
--- shell). Linux however requires monotonically decreasing privileges.
--- This function will will perform some validation on Linux to make
--- sure that `execpromises` is a subset of `promises`. Your libc
--- wrapper for `execve()` will then apply its SECCOMP BPF filter later.
--- Since Linux has to do this before calling `sys_execve()`, the executed
--- process will be weakened to have execute permissions too.
---
---@param mode integer? if specified should specify one penalty:
---
--- - `unix.PLEDGE_PENALTY_KILL_THREAD` causes the violating thread to
---   be killed. This is the default on Linux. It's effectively the
---   same as killing the process, since redbean has no threads. The
---   termination signal can't be caught and will be either `SIGSYS`
---   or `SIGABRT`. Consider enabling stderr logging below so you'll
---   know why your program failed. Otherwise check the system log.
---
--- - `unix.PLEDGE_PENALTY_KILL_PROCESS` causes the process and all
---   its threads to be killed. This is always the case on OpenBSD.
---
--- - `unix.PLEDGE_PENALTY_RETURN_EPERM` causes system calls to just
---   return an `EPERM` error instead of killing. This is a gentler
---   solution that allows code to display a friendly warning. Please
---   note this may lead to weird behaviors if the software being
---   sandboxed is lazy about checking error results.
---
--- `mode` may optionally bitwise or the following flags:
---
--- - `unix.PLEDGE_STDERR_LOGGING` enables friendly error message
---   logging letting you know which promises are needed whenever
---   violations occur. Without this, violations will be logged to
---   `dmesg` on Linux if the penalty is to kill the process. You
---   would then need to manually look up the system call number and
---   then cross reference it with the cosmopolitan libc pledge()
---   documentation. You can also use `strace -ff` which is easier.
---   This is ignored OpenBSD, which already has a good system log.
---   Turning on stderr logging (which uses SECCOMP trapping) also
---   means that the `unix.WTERMSIG()` on your killed processes will
---   always be `unix.SIGABRT` on both Linux and OpenBSD. Otherwise,
---   Linux prefers to raise `unix.SIGSYS`.
---@return true
---@overload fun(promises?: string, execpromises?: string, mode?: integer): nil, error: unix.Errno
function unix.pledge(promises, execpromises, mode) end

--- Restricts filesystem operations, e.g.
---
---    unix.unveil(".", "r");     -- current dir + children visible
---    unix.unveil("/etc", "r");  -- make /etc readable too
---    unix.unveil(nil, nil);     -- commit and lock policy
---
--- Unveiling restricts a thread's view of the filesystem to a set of
--- allowed paths with specific privileges.
---
--- Once you start using unveil(), the entire file system is considered
--- hidden. You then specify, by repeatedly calling unveil(), which paths
--- should become unhidden. When you're finished, you call `unveil(nil,nil)`
--- which commits your policy, after which further use is forbidden, in
--- the current thread, as well as any threads or processes it spawns.
---
--- There are some differences between unveil() on Linux versus OpenBSD.
---
--- 1. Build your policy and lock it in one go. On OpenBSD, policies take
---  effect immediately and may evolve as you continue to call unveil()
---  but only in a more restrictive direction. On Linux, nothing will
---  happen until you call `unveil(nil,nil)` which commits and locks.
---
--- 2. Try not to overlap directory trees. On OpenBSD, if directory trees
---  overlap, then the most restrictive policy will be used for a given
---  file. On Linux overlapping may result in a less restrictive policy
---  and possibly even undefined behavior.
---
--- 3. OpenBSD and Linux disagree on error codes. On OpenBSD, accessing
---  paths outside of the allowed set raises ENOENT, and accessing ones
---  with incorrect permissions raises EACCES. On Linux, both these
---  cases raise EACCES.
---
--- 4. Unlike OpenBSD, Linux does nothing to conceal the existence of
---  paths. Even with an unveil() policy in place, it's still possible
---  to access the metadata of all files using functions like stat()
---  and open(O_PATH), provided you know the path. A sandboxed process
---  can always, for example, determine how many bytes of data are in
---  /etc/passwd, even if the file isn't readable. But it's still not
---  possible to use opendir() and go fishing for paths which weren't
---  previously known.
---
--- This system call is supported natively on OpenBSD and polyfilled on
--- Linux using the Landlock LSM[1].
---
---@param path string is the file or directory to unveil
---
---@param permissions string is a string consisting of zero or more of the following characters:
---
--- - `r` makes `path` available for read-only path operations,
---   corresponding to the pledge promise "rpath".
---
--- - `w` makes `path` available for write operations, corresponding
---   to the pledge promise "wpath".
---
--- - `x` makes `path` available for execute operations,
---   corresponding to the pledge promises "exec" and "execnative".
---
--- - `c` allows `path` to be created and removed, corresponding to
---   the pledge promise "cpath".
---
---@return true
---@overload fun(path: string, permissions: string): nil, error: unix.Errno
---@overload fun(path: nil, permissions: nil): true
function unix.unveil(path, permissions) end

--- Breaks down UNIX timestamp into Zulu Time numbers.
---@param unixts integer
---@return integer year
---@return integer mon 1 ≤ mon ≤ 12
---@return integer mday 1 ≤ mday ≤ 31
---@return integer hour 0 ≤ hour ≤ 23
---@return integer min 0 ≤ min ≤ 59
---@return integer sec 0 ≤ sec ≤ 60
---@return integer gmtoffsec ±93600 seconds
---@return integer wday 0 ≤ wday ≤ 6
---@return integer yday 0 ≤ yday ≤ 365
---@return integer dst 1 if daylight savings, 0 if not, -1 if unknown
---@return string zone
---@nodiscard
---@overload fun(unixts: integer): nil, error: unix.Errno
function unix.gmtime(unixts) end

--- Breaks down UNIX timestamp into local time numbers, e.g.
---
---     >: unix.localtime(unix.clock_gettime())
---     2022    4       28      2       14      22      -25200  4       117     1       "PDT"
---
--- This follows the same API as `gmtime()` which has further details.
---
--- Your redbean ships with a subset of the time zone database.
---
--- - `/zip/usr/share/zoneinfo/Honolulu`   Z-10
--- - `/zip/usr/share/zoneinfo/Anchorage`  Z -9
--- - `/zip/usr/share/zoneinfo/GST`        Z -8
--- - `/zip/usr/share/zoneinfo/Boulder`    Z -6
--- - `/zip/usr/share/zoneinfo/Chicago`    Z -5
--- - `/zip/usr/share/zoneinfo/New_York`   Z -4
--- - `/zip/usr/share/zoneinfo/UTC`        Z +0
--- - `/zip/usr/share/zoneinfo/GMT`        Z +0
--- - `/zip/usr/share/zoneinfo/London`     Z +1
--- - `/zip/usr/share/zoneinfo/Berlin`     Z +2
--- - `/zip/usr/share/zoneinfo/Israel`     Z +3
--- - `/zip/usr/share/zoneinfo/India`      Z +5
--- - `/zip/usr/share/zoneinfo/Beijing`    Z +8
--- - `/zip/usr/share/zoneinfo/Japan`      Z +9
--- - `/zip/usr/share/zoneinfo/Sydney`     Z+10
---
--- You can control which timezone is used using the `TZ` environment
--- variable. If your time zone isn't included in the above list, you
--- can simply copy it inside your redbean. The same is also the case
--- for future updates to the database, which can be swapped out when
--- needed, without having to recompile.
---
---@param unixts integer
---@return integer year
---@return integer mon 1 ≤ mon ≤ 12
---@return integer mday 1 ≤ mday ≤ 31
---@return integer hour 0 ≤ hour ≤ 23
---@return integer min 0 ≤ min ≤ 59
---@return integer sec 0 ≤ sec ≤ 60
---@return integer gmtoffsec ±93600 seconds
---@return integer wday 0 ≤ wday ≤ 6
---@return integer yday 0 ≤ yday ≤ 365
---@return integer dst 1 if daylight savings, 0 if not, -1 if unknown
---@return string zone
---@overload fun(unixts: integer): nil, error: unix.Errno
function unix.localtime(unixts) end

--- Gets information about file or directory.
---@param flags? integer may have any of:
--- - `AT_SYMLINK_NOFOLLOW`: do not follow symbolic links.
---@param dirfd? integer defaults to `unix.AT_FDCWD` and may optionally be set to a directory file descriptor to which `path` is relative.
---@return unix.Stat
---@nodiscard
---@overload fun(path: string, flags?: integer, dirfd?: integer): nil, unix.Errno
function unix.stat(path, flags, dirfd) end

--- Gets information about opened file descriptor.
---
---@param fd integer should be a file descriptor that was opened using `unix.open(path, O_RDONLY|O_DIRECTORY)`.
---
--- `flags` may have any of:
---
--- - `AT_SYMLINK_NOFOLLOW`: do not follow symbolic links.
---
--- `dirfd` defaults to to `unix.AT_FDCWD` and may optionally be set to
--- a directory file descriptor to which `path` is relative.
---
--- A common use for `fstat()` is getting the size of a file. For example:
---
---     fd = assert(unix.open("hello.txt", unix.O_RDONLY))
---     st = assert(unix.fstat(fd))
---     Log(kLogInfo, 'hello.txt is %d bytes in size' % {st:size()})
---     unix.close(fd)
---
---@return unix.Stat
---@nodiscard
---@overload fun(fd: integer): nil, unix.Errno
function unix.fstat(fd) end

--- Opens directory for listing its contents.
---
--- For example, to print a simple directory listing:
---
---     Write('<ul>\r\n')
---     for name, kind, ino, off in assert(unix.opendir(dir)) do
---         if name ~= '.' and name ~= '..' then
---            Write('<li>%s\r\n' % {EscapeHtml(name)})
---         end
---     end
---     Write('</ul>\r\n')
---
---@param path string
---@return unix.Dir state
---@nodiscard
---@overload fun(path: string): nil, error: unix.Errno
function unix.opendir(path) end

--- Opens directory for listing its contents, via an fd.
---
--- @param fd integer should be created by `open(path, O_RDONLY|O_DIRECTORY)`.
--- The returned `unix.Dir` takes ownership of the file descriptor
--- and will close it automatically when garbage collected.
---
---@return function next, unix.Dir state
---@nodiscard
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.fdopendir(fd) end

--- Returns true if file descriptor is a teletypewriter. Otherwise nil
--- with an Errno object holding one of the following values:
---
--- - `ENOTTY` if `fd` is valid but not a teletypewriter
--- - `EBADF` if `fd` isn't a valid file descriptor.
--- - `EPERM` if pledge() is used without `tty` in lenient mode
---
--- No other error numbers are possible.
---
---@param fd integer
---@return true
---@nodiscard
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.isatty(fd) end

---@param fd integer
---@return integer rows, integer cols cellular dimensions of pseudoteletypewriter display.
---@nodiscard
---@overload fun(fd: integer): nil, error: unix.Errno
function unix.tiocgwinsz(fd) end

--- Returns file descriptor of open anonymous file.
--- 
--- This creates a secure temporary file inside `$TMPDIR`. If it isn't
--- defined, then `/tmp` is used on UNIX and GetTempPath() is used on
--- the New Technology. This resolution of `$TMPDIR` happens once.
--- 
--- Once close() is called, the returned file is guaranteed to be
--- deleted automatically. On UNIX the file is unlink()'d before this
--- function returns. On the New Technology it happens upon close().
--- 
--- On the New Technology, temporary files created by this function
--- should have better performance, because `kNtFileAttributeTemporary`
--- asks the kernel to more aggressively cache and reduce i/o ops.
---@return integer fd
---@overload fun(): nil, error: unix.Errno
function unix.tmpfd() end


--- Relinquishes scheduled quantum.
function unix.sched_yield() end

--- Creates interprocess shared memory mapping.
--- 
--- This function allocates special memory that'll be inherited across
--- fork in a shared way. By default all memory in Redbean is "private"
--- memory that's only viewable and editable to the process that owns
--- it. When unix.fork() happens, memory is copied appropriately so
--- that changes to memory made in the child process, don't clobber
--- the memory at those same addresses in the parent process. If you
--- don't want that to happen, and you want the memory to be shared
--- similar to how it would be shared if you were using threads, then
--- you can use this function to achieve just that.
--- 
--- The memory object this function returns may be accessed using its
--- methods, which support atomics and futexes. It's very low-level.
--- For example, you can use it to implement scalable mutexes:
--- 
---     mem = unix.mapshared(8000 * 8)
--- 
---     LOCK = 0 -- pick an arbitrary word index for lock
--- 
---     -- From Futexes Are Tricky Version 1.1 § Mutex, Take 3;
---     -- Ulrich Drepper, Red Hat Incorporated, June 27, 2004.
---     function Lock()
---         local ok, old = mem:cmpxchg(LOCK, 0, 1)
---         if not ok then
---             if old == 1 then
---                 old = mem:xchg(LOCK, 2)
---             end
---             while old > 0 do
---                 mem:wait(LOCK, 2)
---                 old = mem:xchg(LOCK, 2)
---             end
---         end
---     end
---     function Unlock()
---         old = mem:add(LOCK, -1)
---         if old == 2 then
---             mem:store(LOCK, 0)
---             mem:wake(LOCK, 1)
---         end
---     end
--- 
--- It's possible to accomplish the same thing as unix.mapshared()
--- using files and unix.fcntl() advisory locks. However this goes
--- significantly faster. For example, that's what SQLite does and
--- we recommend using SQLite for IPC in redbean. But, if your app
--- has thousands of forked processes fighting for a file lock you
--- might need something lower level than file locks, to implement
--- things like throttling. Shared memory is a good way to do that
--- since there's nothing that's faster.
--- 
---@param size integer
--- The `size` parameter needs to be a multiple of 8. The returned
--- memory is zero initialized. When allocating shared memory, you
--- should try to get as much use out of it as possible, since the
--- overhead of allocating a single shared mapping is 500 words of
--- resident memory and 8000 words of virtual memory. It's because
--- the Cosmopolitan Libc mmap() granularity is 2**16.
--- 
--- This system call does not fail. An exception is instead thrown
--- if sufficient memory isn't available.
---
---@return unix.Memory
function unix.mapshared(size) end

---@class unix.Memory: userdata
--- unix.Memory encapsulates memory that's shared across fork() and
--- this module provides the fundamental synchronization primitives
--- 
--- Redbean memory maps may be used in two ways:
--- 
--- 1. as an array of bytes a.k.a. a string
--- 2. as an array of words a.k.a. integers
--- 
--- They're aliased, union, or overlapped views of the same memory.
--- For example if you write a string to your memory region, you'll
--- be able to read it back as an integer.
--- 
--- Reads, writes, and word operations will throw an exception if a
--- memory boundary error or overflow occurs.
unix.Memory = {}

---@param offset integer?
--- The starting byte index from which memory is copied, which defaults to zero.
---
---@param bytes integer?
--- If `bytes` is none or nil, then the nul-terminated string at
--- `offset` is returned. You may specify `bytes` to safely read
--- binary data.
--- 
--- This operation happens atomically. Each shared mapping has a
--- single lock which is used to synchronize reads and writes to
--- that specific map. To make it scale, create additional maps.
---@return string
---@nodiscard
function unix.Memory:read(offset, bytes) end

--- Writes bytes to memory region.
--- 
---@param data string
---@param offset integer?
--- `offset` is the starting byte index to which memory is copied,
--- which defaults to zero.
---
---@param bytes integer?
--- If `bytes` is none or nil, then an implicit nil-terminator
--- will be included after your `data` so things like json can
--- be easily serialized to shared memory.
--- 
--- This operation happens atomically. Each shared mapping has a
--- single lock which is used to synchronize reads and writes to
--- that specific map. To make it scale, create additional maps.
function unix.Memory:write(data, offset, bytes) end


--- Loads word from memory region.
--- 
--- This operation is atomic and has relaxed barrier semantics.
---@param word_index integer
---@return integer
---@nodiscard
function unix.Memory:load(word_index) end

--- Stores word from memory region.
--- 
--- This operation is atomic and has relaxed barrier semantics.
---@param word_index integer
---@param value integer
function unix.Memory:store(word_index, value) end


--- Exchanges value.
--- 
--- This sets word at `word_index` to `value` and returns the value
--- previously held in by the word.
--- 
--- This operation is atomic and provides the same memory barrier
--- semantics as the aligned x86 LOCK XCHG instruction.
---@param word_index integer
---@param value integer
---@return integer
function unix.Memory:xchg(word_index, value) end

--- Compares and exchanges value.
--- 
--- This inspects the word at `word_index` and if its value is the same
--- as `old` then it'll be replaced by the value `new`, in which case
--- `true, old` shall be returned. If a different value was held at
--- word, then `false` shall be returned along with the word.
--- 
--- This operation happens atomically and provides the same memory
--- barrier semantics as the aligned x86 LOCK CMPXCHG instruction.
---@param word_index integer
---@param old integer
---@param new integer
---@return boolean success, integer old
function unix.Memory:cmpxchg(word_index, old, new) end

--- Fetches then adds value.
--- 
--- This method modifies the word at `word_index` to contain the sum of
--- value and the `value` paremeter. This method then returns the value
--- as it existed before the addition was performed.
--- 
--- This operation is atomic and provides the same memory barrier
--- semantics as the aligned x86 LOCK XADD instruction.
---@param word_index integer
---@param value integer
---@return integer old
function unix.Memory:fetch_add(word_index, value) end

--- Fetches and bitwise ands value.
--- 
--- This operation happens atomically and provides the same memory
--- barrier ordering semantics as its x86 implementation.
---@param word_index integer
---@param value integer
---@return integer
function unix.Memory:fetch_and(word_index, value) end

--- Fetches and bitwise ors value.
--- 
--- This operation happens atomically and provides the same memory
--- barrier ordering semantics as its x86 implementation.
---@param word_index integer
---@param value integer
---@return integer
function unix.Memory:fetch_or(word_index, value) end

--- Fetches and bitwise xors value.
--- 
--- This operation happens atomically and provides the same memory
--- barrier ordering semantics as its x86 implementation.
---@param word_index integer
---@param value integer
---@return integer
function unix.Memory:fetch_xor(word_index, value) end

--- Waits for word to have a different value.
--- 
--- This method asks the kernel to suspend the process until either the
--- absolute deadline expires or we're woken up by another process that
--- calls `unix.Memory:wake()`.
--- 
--- The `expect` parameter is used only upon entry to synchronize the
--- transition to kernelspace. The kernel doesn't actually poll the
--- memory location. It uses `expect` to make sure the process doesn't
--- get added to the wait list unless it's sure that it needs to wait,
--- since the kernel can only control the ordering of wait / wake calls
--- across processes.
--- 
--- The default behavior is to wait until the heat death of the universe
--- if necessary. You may alternatively specify an absolute deadline. If
--- it's less than or equal to the value returned by clock_gettime, then
--- this routine is non-blocking. Otherwise we'll block at most until
--- the current time reaches the absolute deadline.
--- 
--- Futexes are currently supported on Linux, FreeBSD, OpenBSD. On other
--- platforms this method calls sched_yield() and will either (1) return
--- unix.EINTR if a deadline is specified, otherwise (2) 0 is returned.
--- This means futexes will *work* on Windows, Mac, and NetBSD but they
--- won't be scalable in terms of CPU usage when many processes wait on
--- one process that holds a lock for a long time. In the future we may
--- polyfill futexes in userspace for these platforms to improve things
--- for folks who've adopted this api. If lock scalability is something
--- you need on Windows and MacOS today, then consider fcntl() which is
--- well-supported on all supported platforms but requires using files.
--- Please test your use case though, because it's kind of an edge case
--- to have the scenario above, and chances are this op will work fine.
--- 
---@return 0
---@overload fun(self, word_index: integer, expect: integer, abs_deadline?: integer, nanos?: integer): nil, error: unix.Errno
---
--- `EINTR` if a signal is delivered while waiting on deadline. Callers
--- should use futexes inside a loop that is able to cope with spurious
--- wakeups. We don't actually guarantee the value at word has in fact
--- changed when this returns.
--- 
--- `EAGAIN` is raised if, upon entry, the word at `word_index` had a
--- different value than what's specified at `expect`.
--- 
--- `ETIMEDOUT` is raised when the absolute deadline expires.
---
---@param word_index integer
---@param expect integer
---@param abs_deadline integer?
---@param nanos integer?
function unix.Memory:wait(word_index, expect, abs_deadline, nanos) end

--- Wakes other processes waiting on word.
--- 
--- This method may be used to signal or broadcast to waiters. The
--- `count` specifies the number of processes that should be woken,
--- which defaults to `INT_MAX`.
--- 
--- The return value is the number of processes that were actually woken
--- as a result of the system call. No failure conditions are defined.
---@param index integer
---@param count integer?
---@return integer woken
function unix.Memory:wake(index, count) end

---@class unix.Dir: userdata
--- `unix.Dir` objects are created by `opendir()` or `fdopendir()`.
unix.Dir = {}

--- Closes directory stream object and associated its file descriptor.
---
--- This is called automatically by the garbage collector.
---
--- This may be called multiple times.
---@return true
---@overload fun(self: unix.Dir): nil, error: unix.Errno
function unix.Dir:close() end

--- Reads entry from directory stream.
---
--- Returns `nil` if there are no more entries.
--- On error, `nil` will be returned and `errno` will be non-nil.
---
--- `kind` can be any of:
---
--- - `DT_REG`: file is a regular file
--- - `DT_DIR`: file is a directory
--- - `DT_BLK`: file is a block device
--- - `DT_LNK`: file is a symbolic link
--- - `DT_CHR`: file is a character device
--- - `DT_FIFO`: file is a named pipe
--- - `DT_SOCK`: file is a named socket
--- - `DT_UNKNOWN`
---
--- Note: This function also serves as the `__call` metamethod, so that
--- `unix.Dir` objects may be used as a for loop iterator.
---
---@return string name, integer kind, integer ino, integer off
---@nodiscard
---@overload fun(): nil
function unix.Dir:read() end

---@return integer fd file descriptor of open directory object.
---@nodiscard
---@overload fun(): nil, error: unix.Errno
--- Returns `EOPNOTSUPP` if using a `/zip/...` path or if using Windows NT.
function unix.Dir:fd() end

---@return integer off current arbitrary offset into stream.
---@nodiscard
---@overload fun(): nil, error: unix.Errno
function unix.Dir:tell() end

---Resets stream back to beginning.
function unix.Dir:rewind() end

---@class unix.Rusage: userdata
---`unix.Rusage` objects are created by `wait()` or `getrusage()`.
unix.Rusage = {}

---@return integer seconds, integer nanos amount of CPU consumed in userspace.
---@nodiscard
---
--- It's always the case that `0 ≤ nanos < 1e9`.
---
--- On Windows NT this is collected from GetProcessTimes().
function unix.Rusage:utime() end

---@return integer seconds, integer nanos amount of CPU consumed in kernelspace.
---@nodiscard
---
--- It's always the case that `0 ≤ nanos < 1e9`.
---
--- On Windows NT this is collected from GetProcessTimes().
function unix.Rusage:stime() end

---@return integer kilobytes amount of physical memory used at peak consumption.
---@nodiscard
---
--- On Windows NT this is collected from
--- `NtProcessMemoryCountersEx::PeakWorkingSetSize / 1024`.
function unix.Rusage:maxrss() end

---@return integer integralkilobytes integral private memory consumption w.r.t. scheduled ticks.
---@nodiscard
---
--- If you chart memory usage over the lifetime of your process, then
--- this would be the space filled in beneath the chart. The frequency
--- of kernel scheduling is defined as `unix.CLK_TCK`.  Each time a tick
--- happens, the kernel samples your process's memory usage, by adding
--- it to this value. You can derive the average consumption from this
--- value by computing how many ticks are in `utime + stime`.
---
--- Currently only available on FreeBSD and NetBSD.
function unix.Rusage:idrss() end

---@return integer integralkilobytes integral shared memory consumption w.r.t. scheduled ticks.
---@nodiscard
---
--- If you chart memory usage over the lifetime of your process, then
--- this would be the space filled in beneath the chart. The frequency
--- of kernel scheduling is defined as unix.CLK_TCK.  Each time a tick
--- happens, the kernel samples your process's memory usage, by adding
--- it to this value. You can derive the average consumption from this
--- value by computing how many ticks are in `utime + stime`.
---
--- Currently only available on FreeBSD and NetBSD.
function unix.Rusage:ixrss() end

---@return integer integralkilobytes integral stack memory consumption w.r.t. scheduled ticks.
---@nodiscard
---
--- If you chart memory usage over the lifetime of your process, then
--- this would be the space filled in beneath the chart. The frequency
--- of kernel scheduling is defined as `unix.CLK_TCK`. Each time a tick
--- happens, the kernel samples your process's memory usage, by adding
--- it to this value. You can derive the average consumption from this
--- value by computing how many ticks are in `utime + stime`.
---
--- This is only applicable to redbean if its built with MODE=tiny,
--- because redbean likes to allocate its own deterministic stack.
---
--- Currently only available on FreeBSD and NetBSD.
function unix.Rusage:isrss() end

---@return integer count number of minor page faults.
---@nodiscard
---
--- This number indicates how many times redbean was preempted by the
--- kernel to `memcpy()` a 4096-byte page. This is one of the tradeoffs
--- `fork()` entails. This number is usually tinier, when your binaries
--- are tinier.
---
--- Not available on Windows NT.
function unix.Rusage:minflt() end

---Returns number of major page faults.
---
---This number indicates how many times redbean was preempted by the
---kernel to perform i/o. For example, you might have used `mmap()` to
---load a large file into memory lazily.
---
---On Windows NT this is `NtProcessMemoryCountersEx::PageFaultCount`.
---@return integer count
---@nodiscard
function unix.Rusage:majflt() end

---@return integer count number of swap operations.
---@nodiscard
---
--- Operating systems like to reserve hard disk space to back their RAM
--- guarantees, like using a gold standard for fiat currency. When your
--- system is under heavy memory load, swap operations may happen while
--- redbean is working. This number keeps track of them.
---
--- Not available on Linux, Windows NT.
function unix.Rusage:nswap() end

---@return integer count number of times filesystem had to perform input.
---@nodiscard
---On Windows NT this is `NtIoCounters::ReadOperationCount`.
function unix.Rusage:inblock() end

---@return integer count number of times filesystem had to perform output.
---@nodiscard
--- On Windows NT this is `NtIoCounters::WriteOperationCount`.
function unix.Rusage:oublock() end

---@return integer count count of ipc messages sent.
---@nodiscard
--- Not available on Linux, Windows NT.
function unix.Rusage:msgsnd() end

---@return integer count count of ipc messages received.
---@nodiscard
--- Not available on Linux, Windows NT.
function unix.Rusage:msgrcv() end

---@return integer count number of signals received.
---@nodiscard
--- Not available on Linux.
function unix.Rusage:nsignals() end

---@return integer count number of voluntary context switches.
---@nodiscard
---
--- This number is a good thing. It means your redbean finished its work
--- quickly enough within a time slice that it was able to give back the
--- remaining time to the system.
function unix.Rusage:nvcsw() end

--- @return integer count number of non-consensual context switches.
---
--- This number is a bad thing. It means your redbean was preempted by a
--- higher priority process after failing to finish its work, within the
--- allotted time slice.
function unix.Rusage:nivcsw() end

---@class unix.Stat: userdata
---`unix.Stat` objects are created by `stat()` or `fstat()`.
unix.Stat = {}

---@return integer bytes Size of file in bytes.
---@nodiscard
function unix.Stat:size() end

--- Contains file type and permissions.
---
--- For example, `0010644` is what you might see for a file and
--- `0040755` is what you might see for a directory.
---
--- To determine the file type:
---
--- - `unix.S_ISREG(st:mode())` means regular file
--- - `unix.S_ISDIR(st:mode())` means directory
--- - `unix.S_ISLNK(st:mode())` means symbolic link
--- - `unix.S_ISCHR(st:mode())` means character device
--- - `unix.S_ISBLK(st:mode())` means block device
--- - `unix.S_ISFIFO(st:mode())` means fifo or pipe
--- - `unix.S_ISSOCK(st:mode())` means socket
---
---@return integer mode
---@nodiscard
function unix.Stat:mode() end

---@return integer uid User ID of file owner.
---@nodiscard
function unix.Stat:uid() end

---@return integer gid Group ID of file owner.
---@nodiscard
function unix.Stat:gid() end

--- File birth time.
---
--- This field should be accurate on Apple, Windows, and BSDs. On Linux
--- this is the minimum of atim/mtim/ctim. On Windows NT nanos is only
--- accurate to hectonanoseconds.
---
--- Here's an example of how you might print a file timestamp:
---
---   st = assert(unix.stat('/etc/passwd'))
---   unixts, nanos = st:birthtim()
---   year,mon,mday,hour,min,sec,gmtoffsec = unix.localtime(unixts)
---   Write('%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.9d%+.2d%.2d % {
---            year, mon, mday, hour, min, sec, nanos,
---            gmtoffsec / (60 * 60), math.abs(gmtoffsec) % 60})
---@return integer unixts, integer nanos
---@nodiscard
function unix.Stat:birthtim() end

---@return integer unixts, integer nanos Last modified time.
---@nodiscard
function unix.Stat:mtim() end

---@return integer unixts, integer nanos Last access time.
---@nodiscard
---
--- Please note that file systems are sometimes mounted with `noatime`
--- out of concern for i/o performance. Linux also provides `O_NOATIME`
--- as an option for open().
---
--- On Windows NT this is the same as birth time.
function unix.Stat:atim() end

---@return integer unixts, integer nanos  Complicated time.
---@nodiscard
---
--- Means time file status was last changed on UNIX.
---
--- On Windows NT this is the same as birth time.
function unix.Stat:ctim() end

---@return integer count512 Number of 512-byte blocks used by storage medium.
---@nodiscard
--- This provides some indication of how much physical storage a file
--- actually consumes. For example, for small file systems, your system
--- might report this number as being 8, which means 4096 bytes.
---
--- On Windows NT, if `O_COMPRESSED` is used for a file, then this
--- number will reflect the size *after* compression. you can use:
---
---     st = assert(unix.stat("moby.txt"))
---     print('file size is %d bytes' % {st:size()})
---     print('file takes up %d bytes of space' % {st:blocks() * 512})
---     if GetHostOs() == 'WINDOWS' and st:flags() & 0x800 then
---        print('thanks to file system compression')
---     end
---
--- To tell whether or not compression is being used on a file,
function unix.Stat:blocks() end

---@return integer bytes Block size that underlying device uses.
---@nodiscard
---
--- This field might be of assistance in computing optimal i/o sizes.
---
--- Please note this field has no relationship to blocks, as the latter
--- is fixed at a 512 byte size.
function unix.Stat:blksize() end

---@return integer inode Inode number.
---@nodiscard
---
--- This can be used to detect some other process used `rename()` to swap
--- out a file underneath you, so you can do a refresh. redbean does it
--- during each main process heartbeat for its own use cases.
---
--- On Windows NT this is set to `NtByHandleFileInformation::FileIndex`.
function unix.Stat:ino() end

---@return integer dev ID of device containing file.
---@nodiscard
--- On Windows NT this is set to `NtByHandleFileInformation::VolumeSerialNumber`.
function unix.Stat:dev() end

---@return integer rdev Information about device type.
---@nodiscard
--- This value may be set to `0` or `-1` for files that aren't devices,
--- depending on the operating system. `unix.major()` and `unix.minor()`
--- may be used to extract the device numbers.
function unix.Stat:rdev() end

---@return any
---@nodiscard
function unix.Stat:nlink() end

---@return any
---@nodiscard
function unix.Stat:gen() end

---@return any
---@nodiscard
function unix.Stat:flags() end

---@class unix.Sigset: userdata
--- The unix.Sigset class defines a mutable bitset that may currently
--- contain 128 entries. See `unix.NSIG` to find out how many signals
--- your operating system actually supports.
---
--- Constructs new signal bitset object.
---@param sig integer
---@param ... integer
---@return unix.Sigset
---@nodiscard
function unix.Sigset(sig, ...) end

--- Adds signal to bitset.
---@param sig integer
function unix.Sigset:add(sig) end

--- Removes signal from bitset.
---@param sig integer
function unix.Sigset:remove(sig) end

--- Sets all bits in signal bitset to `true`.
function unix.Sigset:fill() end

--- Sets all bits in signal bitset to `false`.
function unix.Sigset:clear() end

---@param sig integer
---@return boolean # `true` if `sig` is member of signal bitset.
---@nodiscard
function unix.Sigset:contains(sig) end

---@return string # Lua code string that recreates object.
---@nodiscard
function unix.Sigset:__repr() end

---@return string # Lua code string that recreates object.
---@nodiscard
function unix.Sigset:__tostring() end

--- This object is returned by system calls that fail. We prefer returning
--- an object because for many system calls, an error is part their normal
--- operation. For example, it's often desirable to use the `errno()` method
--- when performing a `read()` to check for EINTR.
---@class unix.Errno: userdata
unix.Errno = {}

---@return integer error error magic number.
---@nodiscard
---
--- The error number is always different for different platforms. On
--- UNIX systems, error numbers occupy the range [1,127] in practice.
--- The System V ABI reserves numbers as high as 4095. On Windows NT,
--- error numbers can go up to 65535.
function unix.Errno:errno() end

---@return integer error Windows error number.
---@nodiscard
---
--- On UNIX systems this is always 0. On Windows NT this will normally
--- be the same as `errno(). Because Windows defines so many error codes,
--- there's oftentimes a multimapping between its error codes and System
--- Five. In those cases, this value reflects the `GetLastError()` result
--- at the time the error occurred.
function unix.Errno:winerr() end

---@return string symbol string of symbolic name of System Five error code.
---@nodiscard
--- For example, this might return `"EINTR"`.
function unix.Errno:name() end

---@return string symbol name of system call that failed.
---@nodiscard
--- For example, this might return `"read"` if `read()` was what failed.
function unix.Errno:call() end

---@return string # English string describing System Five error code.
---@nodiscard
--- For example, this might return `"Interrupted system call"`.
function unix.Errno:doc() end

---@return string # verbose string describing error.
---@nodiscard
---
--- Different information components are delimited by slash.
---
--- For example, this might return `"EINTR/4/Interrupted system call"`.
---
--- On Windows NT this will include additional information about the
--- Windows error (including FormatMessage() output) if the WIN32 error
--- differs from the System Five error code.
function unix.Errno:__tostring() end

-- CONSTANTS

---@type integer for debug logging level. See `Log`.
kLogDebug = nil

---@type integer for verbose logging level, which is less than `kLogDebug`. See `Log`.
kLogVerbose = nil

---@type integer for info logging level, which is less than `kLogVerbose`. See `Log`.
kLogInfo = nil

---@type integer for warn logging level, which is less than `kLogVerbose`. See `Log`.
kLogWarn = nil

---@type integer for error logging level, which is less than `kLogWarn`. See `Log`.
kLogError = nil

---@type integer for fatal logging level, which is less than `kLogError`. See `Log`.
---Logging anything at this level will result in a backtrace and process exit.
kLogFatal = nil

---@type integer turn `+` into space. See `ParseUrl`.
kUrlPlus = nil

---@type integer to transcode ISO-8859-1 input into UTF-8. See `ParseUrl`.
kUrlLatin1 = nil

--[[
────────────────────────────────────────────────────────────────────────────────
LEGAL

  redbean contains software licensed ISC, MIT, BSD-2, BSD-3, zlib
  which makes it a permissively licensed gift to anyone who might
  find it useful. The transitive closure of legalese can be found
  inside the binary structure, because notice licenses require we
  distribute the license along with any software that uses it. By
  putting them in the binary, compliance in automated and no need
  for further action on the part of the user who is distributing.


────────────────────────────────────────────────────────────────────────────────
SEE ALSO

  https://redbean.dev/
  https://news.ycombinator.com/item?id=26271117

]]
