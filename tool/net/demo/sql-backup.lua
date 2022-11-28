if GetAssetSize("backup.sqlite3") == nil then
    Write("no backup exists. call <a href='sql-backupstore.lua'>sql-backupstore.lua</a> first.")
    return
end

backup = sqlite3.open_memory()
buffer = LoadAsset("backup.sqlite3")
backup:deserialize(buffer)

-- insert more values
for i = 1, 250 do
  backup:exec("INSERT INTO test (content) VALUES ('Hello more')")
end

-- See .init.lua for CREATE TABLE setup.
for row in backup:nrows("SELECT * FROM test") do
   Write(row.id.." "..row.content.."<br>")
end
