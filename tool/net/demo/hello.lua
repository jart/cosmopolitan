if not IsPublicIp(GetClientAddr()) then
   StoreAsset('/hi', 'sup')
end
mymodule.hello()
