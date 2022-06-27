Write('<!doctype html>\r\n')
Write('<title>redbean</title>\r\n')
Write('<style>\r\n')
Write('td,th { padding: 2px 5px; }\r\n')
Write('td { white-space: nowrap; }\r\n')
Write('.l { text-align: left; }\r\n')
Write('.r { text-align: right; }\r\n')
Write('</style>\r\n')

Write('<h3>UNIX Directory Stream Demo</h3>\r\n')
Write('<table>\r\n')
Write('<thead>\r\n')
Write('<tr>\r\n')
Write('<th class=l>name\r\n')
Write('<th>type\r\n')
Write('<th class=r>ino\r\n')
Write('<th class=r>off\r\n')
Write('<th class=r>size\r\n')
Write('<th class=r>blocks\r\n')
Write('<th class=r>mode\r\n')
Write('<th class=r>uid\r\n')
Write('<th class=r>gid\r\n')
Write('<th class=r>dev\r\n')
Write('<th class=r>rdev\r\n')
Write('<th class=r>nlink\r\n')
Write('<th class=r>blksize\r\n')
Write('<th class=r>gen\r\n')
Write('<th class=r>flags\r\n')
Write('<th class=r>birthtim\r\n')
Write('<th class=r>mtim\r\n')
Write('<th class=r>atim\r\n')
Write('<th class=r>ctim\r\n')
Write('<tbody>\r\n')

dir = '.'
for name, kind, ino, off in assert(unix.opendir(dir)) do
   Write('<tr>\r\n')

   Write('<td>')
   Write(EscapeHtml(name))
   if kind == unix.DT_DIR then
      Write('/')
   end
   Write('\r\n')

   Write('<td>')
   if kind == unix.DT_REG then      Write('DT_REG')
   elseif kind == unix.DT_DIR then  Write('DT_DIR')
   elseif kind == unix.DT_FIFO then Write('DT_FIFO')
   elseif kind == unix.DT_CHR then  Write('DT_CHR')
   elseif kind == unix.DT_BLK then  Write('DT_BLK')
   elseif kind == unix.DT_LNK then  Write('DT_LNK')
   elseif kind == unix.DT_SOCK then Write('DT_SOCK')
   else                             Write('DT_UNKNOWN')
   end
   Write('\r\n')

   Write('<td class=r>%d\r\n' % {ino})
   Write('<td class=r>%d\r\n' % {off})

   st,err = unix.stat(dir..'/'..name, unix.AT_SYMLINK_NOFOLLOW)
   if st then

      Write('<td class=r>%d\r\n' % {st:size()})
      Write('<td class=r>%d\r\n' % {st:blocks()})
      Write('<td class=r>%.7o\r\n' % {st:mode()})
      Write('<td class=r>%d\r\n' % {st:uid()})
      Write('<td class=r>%d\r\n' % {st:gid()})
      Write('<td class=r>%d\r\n' % {st:dev()})
      Write('<td class=r>%d,%d\r\n' % {unix.major(st:rdev()), unix.minor(st:rdev())})
      Write('<td class=r>%d\r\n' % {st:nlink()})
      Write('<td class=r>%d\r\n' % {st:blksize()})
      Write('<td class=r>%d\r\n' % {st:gen()})
      Write('<td class=r>%#x\r\n' % {st:flags()})

      function WriteTime(unixsec,nanos)
         year,mon,mday,hour,min,sec,gmtoffsec = unix.localtime(unixsec)
         Write('<td class=r>%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.9d%+.2d%.2d\r\n' % {
                  year, mon, mday, hour, min, sec, nanos,
                  gmtoffsec / (60 * 60), math.abs(gmtoffsec) % 60})
      end

      WriteTime(st:birthtim())
      WriteTime(st:mtim())
      WriteTime(st:atim())
      WriteTime(st:ctim())

   else
      Write('<td class=l colspan=15>%s\r\n' % {err})
   end

end
Write('</table>\r\n')
