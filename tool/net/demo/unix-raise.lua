#!/home/jart/bin/redbean -i
assert(unix.sigaction(unix.SIGUSR1, function(sig)
   gotsigusr1 = true
end))
gotsigusr1 = false
assert(unix.raise(unix.SIGUSR1))
if gotsigusr1 then
   print('hooray the signal was delivered')
else
   print('oh no some other signal was handled')
end
