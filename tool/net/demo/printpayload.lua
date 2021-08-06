SetStatus(200)
SetHeader("Content-Type", GetHeader("Content-Type") or "text/plain")
Write(GetPayload())
