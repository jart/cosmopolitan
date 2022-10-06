local function main()
   if GetMethod() ~= 'POST' and GetMethod() ~= 'PUT' then
      ServeError(405)
      SetHeader('Allow', 'POST, PUT')
      return
   end
   SetStatus(200)
   SetHeader("Content-Type", GetHeader("Content-Type") or "text/plain")
   Write(GetBody())
end

main()
