buffer = db:serialize()
StoreAsset("backup.sqlite3", buffer)

Write("backup created. size: "..GetAssetSize("backup.sqlite3"))