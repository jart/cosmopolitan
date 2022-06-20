mymodule = require "mymodule"
sqlite3 = require "lsqlite3"

-- /.init.lua is loaded at startup in redbean's main process
HidePath('/usr/share/zoneinfo/')
HidePath('/usr/share/ssl/')

-- open a browser tab using explorer/open/xdg-open
-- LaunchBrowser('/tool/net/demo/index.html')

-- sql database (see sql.lua)
function StoreSqlite(database, path)
   local buffer = database:serialize()
   return StoreAsset(path, buffer)
end

function LoadSqlite(path)
   local database = sqlite3.open_memory()
   local buffer = LoadAsset(path)
   database:deserialize(buffer)
   return database
end

database = "database.sqlite3"
-- Check if there is already a database
if GetAssetSize(database) ~= nil then
   db = LoadSqlite(database)
   db:exec[[
      INSERT INTO test (content) VALUES ('World');
   ]]
else
   db = sqlite3.open_memory()
   db:exec[[
      CREATE TABLE test (
         id INTEGER PRIMARY KEY,
         content TEXT
      );
      INSERT INTO test (content) VALUES ('Hello');
   ]]
end

-- this intercepts all requests if it's defined
function OnHttpRequest()
   if HasParam('magic') then
      Write('<p>\r\n')
      Write('OnHttpRequest() has intercepted your request<br>\r\n')
      Write('because you specified the magic parameter\r\n')
      Write('<pre>\r\n')
      Write(EscapeHtml(LoadAsset('/.init.lua')))
      Write('</pre>\r\n')
   else
      Route() -- this asks redbean to do the default thing
   end
   SetHeader('Server', 'redbean!')
end

function OnServerStop()
   -- make sure we store the database on exit
   StoreSqlite(db, database)
end

function Adder(x, y)
   return x + y
end
