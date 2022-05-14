-- StoreAsset Demo
--
-- Redbean is able to store files into its own executable structure.
-- This is currently only supported on a Linux, Apple, and FreeBSD.
--
-- By loading this page, your redbean will insert an immediate file into
-- your redbean named `/hi`, which you can click the back button in the
-- browser and view it on the listing page right afterwards!

Write[[<!doctype html>
  <title>redbean store asset demo</title>
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
    <a href="store-asset.lua">store asset demo</a>
  </h1>
]]

if IsPublicIp(GetClientAddr()) then
   Write[[
      <p>
        Bad request.
      </p>
      <p>
        This HTTP endpoint self-modifies the web server. You're
        communicating wtith this redbean over a public network.
        Therefore redbean won't service this request.
      </p>
   ]]

elseif GetHostOs() == "WINDOWS" or
       GetHostOs() == "OPENBSD" or
       GetHostOs() == "NETBSD" then
   Write[[
      <p>
        Unsupported
      </p>
      <p>
        Sorry! Redbean's Lua StoreAsset() function is only
        supported on Linux, Apple, and FreeBSD right now.
      </p>
   ]]

else
   StoreAsset('/hi', [[
StoreAsset() worked!

This file was inserted into your redbean by the Lua StoreAsset() API
which was invoked by you browsing to the /store-asset.lua page.

Enjoy your self-modifying web server!
]])

   Write[[
     <p>
       This Lua script has just stored a new file named <code>/hi</code>
       to your redbean zip executable. This was accomplished while the
       web server is running. It live updates, so if you click the back
       button in your browser, you should see <code>/hi</code> in the
       ZIP central directory listing, and you can send an HTTP message
       requesting it.
     </p>
   ]]

end

Write[[
  <p>
    <a href="/">go back</a>
  </p>
]]
