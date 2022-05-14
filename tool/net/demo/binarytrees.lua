-- binary trees: benchmark game
-- with resource limit tutorial
-- by justine tunney

local outofcpu = false
local oldsigxcpufunc = nil
local oldsigxcpuflags = nil
local oldsigxcpumask = nil

-- This is our signal handler.
function OnSigxcpu(sig)
   -- Please note, it's dangerous to do complicated things from inside
   -- asynchronous signal handlers. Even Log() isn't async signal safe
   -- The best possible practice is to have them simply set a variable
   outofcpu = true
end

-- These two functions are what's being benchmarked. It's a popular
-- torture test for interpreted languages since it generates a lot of
-- garbage. Lua does well on this test, going faster than many other
-- languages, e.g. Racket, Python.

local function MakeTree(depth)
   if outofcpu then
      error({reason='MakeTree() caught SIGXCPU'})
   end
   if depth > 0 then
      depth = depth - 1
      left, right = MakeTree(depth), MakeTree(depth)
      return { left, right }
   else
      return { }
   end
end

local function CheckTree(tree)
   if outofcpu then
      error({reason='CheckTree() caught SIGXCPU'})
   end
   if tree[1] then
      return 1 + CheckTree(tree[1]) + CheckTree(tree[2])
   else
      return 1
   end
end

-- Now for our redbean web server code...

local function WriteForm(depth, suggestion)
   Write([[<!doctype html>

     <title>redbean binary trees</title>

     <style>
       body { padding: 1em; }
       h1 a { color: inherit; text-decoration: none; }
       h1 img { border: none; vertical-align: middle; }
       input { margin: 1em; padding: .5em; }
       p { word-break: break-word; max-width: 650px; }
       dt { font-weight: bold; }
       dd { margin-top: 1em; margin-bottom: 1em; }
       .hdr { text-indent: -1em; padding-left: 1em; }
     </style>

     <h1>
       <a href="/"><img src="/redbean.png"></a>
       <a href="fetch.lua">binary trees benchmark demo</a>
     </h1>

     <p>
       This demo is from the computer language benchmark game. It's a
       torture test for the Lua garbage collector. ProTip: Try loading
       this page while the terminal memory monitor feature is active, so
       you can see memory shuffle around in a dual screen display. We
       use setrlimit to set a 512mb memory limit. So if you specify a
       number higher than 23 (a good meaty benchmark) then the Lua GC
       should panic a bit trying to recover (due to malloc failure) and
       ultimately the worker process should die. However the server and
       your system will survive.
     </p>

     <form action="binarytrees.lua" method="post">
       <input type="text" id="depth" name="depth" size="70"
              value="%s" placeholder="%d" onfocus="this.select()"
              autofocus>
       <input type="submit" value="run">
     </form>

   ]] % {EscapeHtml(depth), suggestion})
end

local function BinaryTreesDemo()
   if GetMethod() == 'GET' or GetMethod() == 'HEAD' then
      WriteForm("18", 18)
   elseif GetMethod() == 'POST' and not HasParam('depth') then
      ServeError(400)
   elseif GetMethod() == 'POST' then

      -- write out the page so user can submit again
      WriteForm(GetParam('depth'), 18)
      Write('<dl>\r\n')
      Write('<dt>Output\r\n')
      Write('<dd>\r\n')

      -- impose quotas to protect the server
      unix.setrlimit(unix.RLIMIT_AS, 1024 * 1024 * 1024)
      unix.setrlimit(unix.RLIMIT_CPU, 2, 4)

      -- when RLIMIT_AS (virtual address space) runs out of memory, then
      -- mmap() and malloc() start to fail, and lua reacts by trying to
      -- run the garbage collector over and over again. so we also place
      -- a limit on the number of "cpu time" seconds too. the behavior
      -- when we hit the soft limit, is the kernel gives us a friendly
      -- warning by sending the signal SIGXCPU which we must catch here
      -- once we catch it, we've got 2 seconds of grace time to clean up
      -- before we hit the "hard limit" and the kernel sends SIGKILL (9)
      oldsigxcpufunc,
      oldsigxcpuflags,
      oldsigxcpumask =
         unix.sigaction(unix.SIGXCPU, OnSigxcpu)

      -- get the user-supplied parameter
      depth = tonumber(GetParam('depth'))

      -- run the benchmark, recording how long it takes
      secs1, nanos1 = unix.clock_gettime()
      res = CheckTree(MakeTree(depth))
      secs2, nanos2 = unix.clock_gettime()

      -- turn 128-bit timestamps into 64-bit nanosecond interval
      if secs2 == secs1 then
         nanos = nanos2 - nanos1
      else
         nanos = (secs2 - secs1) * 1000000000 + (1000000000 - nanos1) + nanos2
      end

      -- write out result of benchmark
      Write('0%o\r\n' % {res})
      Write('<dt>Time Elapsed\r\n')
      Write('<dd>\r\n')
      Write('%g seconds, or<br>\r\n' % {nanos / 1e9})
      Write('%g milliseconds, or<br>\r\n' % {nanos / 1e6})
      Write('%g microseconds, or<br>\r\n' % {nanos / 1e6})
      Write('%d nanoseconds\r\n' % {nanos})
      Write('<dt>unix.clock_gettime() #1\r\n')
      Write('<dd>%d, %d\r\n' % {secs1, nanos1})
      Write('<dt>unix.clock_gettime() #2\r\n')
      Write('<dd>%d, %d\r\n' % {secs2, nanos2})
      Write('</dl>\r\n')

   else
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD, POST')
   end
end

local function main()
   -- catch exceptions
   ok, err = pcall(BinaryTreesDemo)

   -- we don't need to restore the old handler
   -- but it's generally a good practice to clean up
   if oldsigxcpufunc then
      unix.sigaction(unix.SIGXCPU,
                     oldsigxcpufunc,
                     oldsigxcpuflags,
                     oldsigxcpumask)
   end

   -- handle exceptions
   if not ok then

      -- whenever anything, at all, goes wrong, with anything
      -- always with few exceptions close the connection asap
      SetHeader('Connection', 'close')

      if err.reason then
         -- show our error message withoun internal code leaking out
         Write(err.reason)
         Write('\r\n')
      else
         -- just rethrow exception
         error('unexpected failure: ' .. tostring(err))
      end

   end
end

main()
