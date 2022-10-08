Write([[<!doctype html>
<title>redbean mapshared demo</title>
<style>
  body { padding: 1em; }
  h1 a { color: inherit; text-decoration: none; }
  h1 img { border: none; vertical-align: middle; }
  input { margin: 1em; padding: .5em; }
  pre { margin-left: 2em; }
  p { word-break: break-word; max-width: 650px; }
  dt { font-weight: bold; }
  dd { margin-top: 1em; margin-bottom: 1em; }
  .hdr { text-indent: -1em; padding-left: 1em; }
</style>
<h1>
  <a href="/"><img src="/redbean.png"></a>
  <a href="fetch.lua">redbean mapshared demo</a>
</h1>
<p>
  This page displays a <code>unix.mapshared()</code> hit counter of
  the <code>GetPath()</code>.
</p>
<dl>
]])

Lock()
s = shm:read(SHM_JSON)
if s == '' then s = '{}' end
t = DecodeJson(s)
Unlock()

for k,v in pairs(t) do
    Write('<dt>%s<dd>%d\n' % {EscapeHtml(k), v})
end

Write('</dl>')
