-- reverse proxy for turfwar

if IsDaemon() then
    ProgramPort(443)
    ProgramUid(65534)
    ProgramLogPath('/var/log/turfbean.log')
    ProgramPidPath('/var/log/turfbean.pid')
    ProgramTrustedIp(ParseIp(Slurp('/etc/justine-ip.txt')), 32);
    ProgramCertificate(Slurp('/etc/letsencrypt/live/ipv4.games-ecdsa/fullchain.pem'))
    ProgramPrivateKey(Slurp('/etc/letsencrypt/live/ipv4.games-ecdsa/privkey.pem'))
end

RELAY_HEADERS_TO_CLIENT = {
    'Access-Control-Allow-Origin',
    'Cache-Control',
    'Connection',
    'Content-Type',
    'Last-Modified',
    'Referrer-Policy',
}

function OnServerStart()
    ProgramTokenBucket()
    assert(unix.setrlimit(unix.RLIMIT_NPROC, 1000, 1000))
end

function OnWorkerStart()
    assert(unix.setrlimit(unix.RLIMIT_RSS, 2*1024*1024))
    assert(unix.setrlimit(unix.RLIMIT_CPU, 2))
    assert(unix.unveil(nil, nil))
    assert(unix.pledge("stdio inet unix", nil, unix.PLEDGE_PENALTY_RETURN_EPERM))
end

function OnHttpRequest()
    local ip = GetClientAddr()
    if not IsTrustedIp(ip) then
        local tok = AcquireToken(ip)
        if tok < 2 then
            if Blackhole(ip) then
                Log(kLogWarn, "banned %s" % {FormatIp(ip)})
            else
                Log(kLogWarn, "failed to ban %s" % {FormatIp(ip)})
            end
        end
        if tok < 30 then
            ServeError(429)
            SetHeader('Connection', 'close')
            Log(kLogWarn, "warned %s who has %d tokens" % {FormatIp(ip), tok})
            return
        end
    end
    local url = 'http://127.0.0.1' .. EscapePath(GetPath())
    local name = GetParam('name')
    if name then
        url = url .. '?name=' .. EscapeParam(name)
    end
    local status, headers, body =
        Fetch(url,
              {method = GetMethod(),
               headers = {
                   ['Accept'] = GetHeader('Accept'),
                   ['CF-IPCountry'] = GetHeader('CF-IPCountry'),
                   ['If-Modified-Since'] = GetHeader('If-Modified-Since'),
                   ['Referer'] = GetHeader('Referer'),
                   ['Sec-CH-UA-Platform'] = GetHeader('Sec-CH-UA-Platform'),
                   ['User-Agent'] = GetHeader('User-Agent'),
                   ['X-Forwarded-For'] = FormatIp(ip)}})
    if status then
        SetStatus(status)
        for k,v in pairs(RELAY_HEADERS_TO_CLIENT) do
            SetHeader(v, headers[v])
        end
        Write(body)
    else
        local err = headers
        Log(kLogError, "proxy failed %s" % {err})
        ServeError(503)
    end
end
