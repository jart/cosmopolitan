SetStatus(200)
SetHeader("Content-Type", GetHeader("Content-Type"))
Write(GetPayload())
