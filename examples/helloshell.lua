-- o/third_party/lua/lua.com helloshell.lua | sort | uniq
-- output:
--   124060
--   130754
--   2924
--   2943
--   2944

function capture(cmd)
  local f = io.popen(cmd,'r')
  local s = f:read("*a")
  f:close()
  return s
end

for i=1, 100 do
  local stdout = capture('ls -laR / 2>&1')
  print(stdout:len())
end
