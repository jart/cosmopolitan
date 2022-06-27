-- Call Lua Module Demo
--
-- Your Lua modules may be stored in the /.lua/ folder.
--
-- In our /.init.lua global scope earlier, we ran the code:
--
--    mymodule = require "mymodule"
--
-- Which preloaded the /.lua/mymodule.lua module once into the server
-- global memory template from which all request handlers are forked.
-- Therefore, we can just immediately use that module from our Lua
-- server pages.

Write[[<!doctype html>
  <title>redbean call lua module demo</title>
  <style>
    body { padding: 1em; }
    h1 a { color: inherit; text-decoration: none; }
    h1 img { border: none; vertical-align: middle; }
    pre { margin-left: 2em; }
    p { word-break: break-word; max-width: 650px; }
    dt { font-weight: bold; }
    dd { margin-top: 1em; margin-bottom: 1em; }
    .hdr { text-indent: -1em; padding-left: 1em; }
  </style>
  <h1>
    <a href="/"><img src="/redbean.png"></a>
    <a href="call-lua-module.lua">call lua module demo</a>
  </h1>
  <p>Your Lua modules may be stored in the /.lua/ folder.
  <p>In our <code>/.init.lua</code> global scope earlier, we ran the code:
  <pre>mymodule = require "mymodule"</pre>
  <p>Which preloaded the <code>/.lua/mymodule.lua</code> module once into
     the server global memory template from which all request handlers are
     forked. Therefore, we can just immediately use that module from our
     Lua Server Pages.
  <p>
    Your <code>mymodule.hello()</code> output is as follows:
  <blockquote>
]]

mymodule.hello()

Write[[
  </blockquote>
  <p>
    <a href="/">go back</a>
  </p>
]]
