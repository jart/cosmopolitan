-- fingerprinting example

Write[[<!doctype html>

<title>redbean binary trees</title>

<style>
  body { padding: 1em; }
  h1 a { color: inherit; text-decoration: none; }
  h1 img { border: none; vertical-align: middle; }
  input { margin: 1em; padding: .5em; }
  p { word-break: break-word; max-width: 650px; }
  dt { font-family: monospace; }
  dd { margin-top: 1em; margin-bottom: 1em; }
  .hdr { text-indent: -1em; padding-left: 1em; }
</style>

<h1>
  <a href="/"><img src="/redbean.png"></a>
  <a href="finger.lua">redbean finger demo</a>
</h1>

]]

Write[[
  <h2>Your TCP SYN Packet</h2>
]]
-- See .init.lua hooks which set SYN and SYNERR globals.
if syn then
   if syn ~= '' then
      Write('<dl>\r\n')
      Write('<dt>syn = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SAVED_SYN)\r\n')
      Write('<dd>')
      Write(EscapeHtml(EncodeLua(syn)))
      Write('\r\n')
      Write('<dt>finger.FingerSyn(syn)\r\n')
      Write('<dd>')
      Write(EscapeHtml(EncodeLua(finger.FingerSyn(syn))))
      Write('\r\n')
      Write('<dt>finger.DescribeSyn(syn)\r\n')
      Write('<dd>')
      Write(EscapeHtml(EncodeLua(finger.DescribeSyn(syn))))
      Write('\r\n')
      Write('<dt>finger.GetSynFingerOs(finger.FingerSyn(syn))\r\n')
      Write('<dd>')
      Write(EscapeHtml(EncodeLua(finger.GetSynFingerOs(finger.FingerSyn(syn)))))
      Write('\r\n')
      Write('</dl>\r\n')
   else
      Write([[
        <p>
          your operating system returned an empty string as the syn
          packet! did you remember to use setsockopt(TCP_SAVE_SYN)?
          did you call getsockopt(TCP_SAVED_SYN) more than once?
        </p>
      ]])
   end
else
   Write([[
     <p>
       your operating system most likely doesn't support TCP_SAVED_SYN
       because getsockopt() returned %s.
     </p>
   ]] % {EscapeHtml(tostring(synerr))})
end
