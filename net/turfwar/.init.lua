-- reverse proxy for turfwar

ProgramPort(443)
ProgramTokenBucket()

RELAY_HEADERS_TO_CLIENT = {
    'Access-Control-Allow-Origin',
    'Cache-Control',
    'Connection',
    'Content-Encoding',
    'Content-Type',
    'Last-Modified',
    'Referrer-Policy',
    'Vary',
}

function OnWorkerStart()
    assert(unix.unveil(nil, nil))
    assert(unix.pledge("stdio inet", nil, unix.PLEDGE_PENALTY_RETURN_EPERM))
end

function OnHttpRequest()
    local status, headers, body =
        Fetch('http://127.0.0.1' .. EscapePath(GetPath()),
              {method = GetMethod(),
               headers = {
                   ['Accept'] = GetHeader('Accept'),
                   ['Accept-Encoding'] = GetHeader('Accept-Encoding'),
                   ['CF-IPCountry'] = GetHeader('CF-IPCountry'),
                   ['If-Modified-Since'] = GetHeader('If-Modified-Since'),
                   ['Referer'] = GetHeader('Referer'),
                   ['Sec-CH-UA-Platform'] = GetHeader('Sec-CH-UA-Platform'),
                   ['User-Agent'] = GetHeader('User-Agent'),
                   ['X-Forwarded-For'] = FormatIp(GetClientAddr())}})
    if status then
        SetStatus(status)
        for k,v in pairs(RELAY_HEADERS_TO_CLIENT) do
            SetHeader(v, headers[v])
        end
        Write(body)
    else
        err = headers
        Log(kLogError, "proxy failed %s" % {err})
        ServeError(503)
    end
end
