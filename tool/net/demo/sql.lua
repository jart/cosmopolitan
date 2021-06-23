-- See .init.lua for CREATE TABLE setup.
for row in db:nrows("SELECT * FROM test") do
   Write(row.id.." "..row.content.."<br>")
end
