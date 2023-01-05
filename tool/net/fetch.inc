#define FetchHasHeader(H)    (!!msg.headers[H].a)
#define FetchHeaderData(H)   (inbuf.p + msg.headers[H].a)
#define FetchHeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define FetchHeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), FetchHeaderData(H), FetchHeaderLength(H))

static int LuaFetch(lua_State *L) {
#define ssl nope  // TODO(jart): make this file less huge
  char *p;
  ssize_t rc;
  bool usingssl;
  uint32_t ip;
  struct Url url;
  int t, ret, sock, methodidx, hdridx;
  char *host, *port, *request;
  struct TlsBio *bio;
  struct addrinfo *addr;
  struct Buffer inbuf;     // shadowing intentional
  struct HttpMessage msg;  // shadowing intentional
  struct HttpUnchunker u;
  const char *urlarg, *body, *method;
  char *conlenhdr = "";
  char *headers = 0;
  char *hosthdr = 0;
  char *agenthdr = brand;
  char *key, *val, *hdr;
  size_t keylen, vallen;
  size_t urlarglen, requestlen, paylen, bodylen;
  size_t i, g, n, hdrsize;
  int imethod, numredirects = 0, maxredirects = 5;
  bool followredirect = true;
  struct addrinfo hints = {.ai_family = AF_INET,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};

  /*
   * Get args: url [, body | {method = "PUT", body = "..."}]
   */
  urlarg = luaL_checklstring(L, 1, &urlarglen);
  if (lua_istable(L, 2)) {
    lua_settop(L, 2);  // discard any extra arguments
    lua_getfield(L, 2, "body");
    body = luaL_optlstring(L, -1, "", &bodylen);
    lua_getfield(L, 2, "method");
    // use GET by default if no method is provided
    method = luaL_optstring(L, -1, kHttpMethod[kHttpGet]);
    if ((imethod = GetHttpMethod(method, -1))) {
      method = kHttpMethod[imethod];
    } else {
      return LuaNilError(L, "bad method");
    }
    lua_getfield(L, 2, "followredirect");
    if (lua_isboolean(L, -1)) followredirect = lua_toboolean(L, -1);
    lua_getfield(L, 2, "maxredirects");
    maxredirects = luaL_optinteger(L, -1, maxredirects);
    lua_getfield(L, 2, "numredirects");
    numredirects = luaL_optinteger(L, -1, numredirects);
    lua_getfield(L, 2, "headers");
    if (!lua_isnil(L, -1)) {
      if (!lua_istable(L, -1))
        return luaL_argerror(L, 2, "invalid headers value; table expected");

      lua_pushnil(L);
      while (lua_next(L, -2)) {
        if (lua_type(L, -2) == LUA_TSTRING) {  // skip any non-string keys
          key = lua_tolstring(L, -2, &keylen);
          if (!IsValidHttpToken(key, keylen))
            return LuaNilError(L, "invalid header name: %s", key);

          val = lua_tolstring(L, -1, &vallen);
          if (!(hdr = _gc(EncodeHttpHeaderValue(val, vallen, 0))))
            return LuaNilError(L, "invalid header %s value encoding", key);

          // Content-Length and Connection will be overwritten;
          // skip them to avoid duplicates;
          // also allow unknown headers
          if ((hdridx = GetHttpHeader(key, keylen)) == -1 ||
              hdridx != kHttpContentLength && hdridx != kHttpConnection) {
            if (hdridx == kHttpUserAgent) {
              agenthdr = hdr;
            } else if (hdridx == kHttpHost) {
              hosthdr = hdr;
            } else {
              appendd(&headers, key, keylen);
              appendw(&headers, READ16LE(": "));
              appends(&headers, hdr);
              appendw(&headers, READ16LE("\r\n"));
            }
          }
        }
        lua_pop(L, 1);  // pop the value, keep the key for the next iteration
      }
    }
    lua_settop(L, 2);  // drop all added elements to keep the stack balanced
  } else if (lua_isnoneornil(L, 2)) {
    body = "";
    bodylen = 0;
    method = kHttpMethod[kHttpGet];
  } else {
    body = luaL_checklstring(L, 2, &bodylen);
    method = kHttpMethod[kHttpPost];
  }
  // provide Content-Length header unless it's zero and not expected
  methodidx = GetHttpMethod(method, -1);
  if (bodylen > 0 || !(methodidx == kHttpGet || methodidx == kHttpHead ||
                       methodidx == kHttpTrace || methodidx == kHttpDelete ||
                       methodidx == kHttpConnect)) {
    conlenhdr = _gc(xasprintf("Content-Length: %zu\r\n", bodylen));
  }

  /*
   * Parse URL.
   */
  _gc(ParseUrl(urlarg, urlarglen, &url, true));
  _gc(url.params.p);
  usingssl = false;
  if (url.scheme.n) {
#ifndef UNSECURE
    if (!unsecure && url.scheme.n == 5 &&
        !memcasecmp(url.scheme.p, "https", 5)) {
      usingssl = true;
    } else
#endif
        if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      return LuaNilError(L, "bad scheme");
    }
  }

#ifndef UNSECURE
  if (usingssl && !sslinitialized) TlsInit();
#endif

  if (url.host.n) {
    host = _gc(strndup(url.host.p, url.host.n));
    if (url.port.n) {
      port = _gc(strndup(url.port.p, url.port.n));
#ifndef UNSECURE
    } else if (usingssl) {
      port = "443";
#endif
    } else {
      port = "80";
    }
  } else if ((ip = ParseIp(urlarg, -1)) != -1) {
    host = urlarg;
    port = "80";
  } else {
    return LuaNilError(L, "invalid host");
  }
  if (!IsAcceptableHost(host, -1)) {
    return LuaNilError(L, "invalid host");
  }
  if (!hosthdr) hosthdr = _gc(xasprintf("%s:%s", host, port));

  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    p = _gc(xmalloc(1 + url.path.n));
    mempcpy(mempcpy(p, "/", 1), url.path.p, url.path.n);
    url.path.p = p;
    ++url.path.n;
  }

  /*
   * Create HTTP message.
   */
  request = 0;
  appendf(&request,
          "%s %s HTTP/1.1\r\n"
          "Host: %s\r\n"
          "Connection: close\r\n"
          "User-Agent: %s\r\n"
          "%s%s"
          "\r\n",
          method, _gc(EncodeUrl(&url, 0)), hosthdr, agenthdr, conlenhdr,
          headers ? headers : "");
  appendd(&request, body, bodylen);
  requestlen = appendz(request).i;
  _gc(request);

  /*
   * Perform DNS lookup.
   */
  DEBUGF("(ftch) client resolving %s", host);
  if ((rc = getaddrinfo(host, port, &hints, &addr)) != EAI_SUCCESS) {
    return LuaNilError(L, "getaddrinfo(%s:%s) error: EAI_%s %s", host, port,
                       gai_strerror(rc), strerror(errno));
  }

  /*
   * Connect to server.
   */
  ip = ntohl(((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
  DEBUGF("(ftch) client connecting %hhu.%hhu.%hhu.%hhu:%d", ip >> 24, ip >> 16,
         ip >> 8, ip, ntohs(((struct sockaddr_in *)addr->ai_addr)->sin_port));
  CHECK_NE(-1, (sock = GoodSocket(addr->ai_family, addr->ai_socktype,
                                  addr->ai_protocol, false, &timeout)));
  rc = connect(sock, addr->ai_addr, addr->ai_addrlen);
  freeaddrinfo(addr), addr = 0;
  if (rc == -1) {
    close(sock);
    return LuaNilError(L, "connect(%s:%s) error: %s", host, port,
                       strerror(errno));
  }

#ifndef UNSECURE
  if (usingssl) {
    if (sslcliused) {
      mbedtls_ssl_session_reset(&sslcli);
    } else {
      ReseedRng(&rngcli, "child");
    }
    sslcliused = true;
    DEBUGF("(ftch) client handshaking %`'s", host);
    if (!evadedragnetsurveillance) {
      mbedtls_ssl_set_hostname(&sslcli, host);
    }
    bio = _gc(malloc(sizeof(struct TlsBio)));
    bio->fd = sock;
    bio->a = 0;
    bio->b = 0;
    bio->c = -1;
    mbedtls_ssl_set_bio(&sslcli, bio, TlsSend, 0, TlsRecvImpl);
    while ((ret = mbedtls_ssl_handshake(&sslcli))) {
      switch (ret) {
        case MBEDTLS_ERR_SSL_WANT_READ:
          break;
        case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
          goto VerifyFailed;
        default:
          close(sock);
          return LuaNilTlsError(L, "handshake", ret);
      }
    }
    LockInc(&shared->c.sslhandshakes);
    VERBOSEF("(ftch) shaken %s:%s %s %s", host, port,
             mbedtls_ssl_get_ciphersuite(&sslcli),
             mbedtls_ssl_get_version(&sslcli));
  }
#endif /* UNSECURE */

  /*
   * Send HTTP Message.
   */
  DEBUGF("(ftch) client sending %s request", method);
  for (i = 0; i < requestlen; i += rc) {
#ifndef UNSECURE
    if (usingssl) {
      rc = mbedtls_ssl_write(&sslcli, request + i, requestlen - i);
      if (rc <= 0) {
        if (rc == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) goto VerifyFailed;
        close(sock);
        return LuaNilTlsError(L, "write", rc);
      }
    } else
#endif
        if ((rc = WRITE(sock, request + i, requestlen - i)) <= 0) {
      close(sock);
      return LuaNilError(L, "write error: %s", strerror(errno));
    }
  }
  if (logmessages) {
    LogMessage("sent", request, requestlen);
  }

  /*
   * Handle response.
   */
  bzero(&inbuf, sizeof(inbuf));
  InitHttpMessage(&msg, kHttpResponse);
  for (hdrsize = paylen = t = 0;;) {
    if (inbuf.n == inbuf.c) {
      inbuf.c += 1000;
      inbuf.c += inbuf.c >> 1;
      inbuf.p = realloc(inbuf.p, inbuf.c);
    }
    NOISEF("(ftch) client reading");
#ifndef UNSECURE
    if (usingssl) {
      if ((rc = mbedtls_ssl_read(&sslcli, inbuf.p + inbuf.n,
                                 inbuf.c - inbuf.n)) < 0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          rc = 0;
        } else {
          close(sock);
          free(inbuf.p);
          DestroyHttpMessage(&msg);
          return LuaNilTlsError(L, "read", rc);
        }
      }
    } else
#endif
        if ((rc = READ(sock, inbuf.p + inbuf.n, inbuf.c - inbuf.n)) == -1) {
      close(sock);
      free(inbuf.p);
      DestroyHttpMessage(&msg);
      return LuaNilError(L, "read error: %s", strerror(errno));
    }
    g = rc;
    inbuf.n += g;
    switch (t) {
      case kHttpClientStateHeaders:
        if (!g) {
          WARNF("(ftch) HTTP client %s error", "EOF headers");
          goto TransportError;
        }
        rc = ParseHttpMessage(&msg, inbuf.p, inbuf.n);
        if (rc == -1) {
          WARNF("(ftch) HTTP client %s error", "ParseHttpMessage");
          goto TransportError;
        }
        if (rc) {
          DEBUGF("(ftch) content-length is %`'.*s",
                 FetchHeaderLength(kHttpContentLength),
                 FetchHeaderData(kHttpContentLength));
          hdrsize = rc;
          if (logmessages) {
            LogMessage("received", inbuf.p, hdrsize);
          }
          if (100 <= msg.status && msg.status <= 199) {
            if ((FetchHasHeader(kHttpContentLength) &&
                 !FetchHeaderEqualCase(kHttpContentLength, "0")) ||
                (FetchHasHeader(kHttpTransferEncoding) &&
                 !FetchHeaderEqualCase(kHttpTransferEncoding, "identity"))) {
              WARNF("(ftch) HTTP client %s error", "Content-Length #1");
              goto TransportError;
            }
            DestroyHttpMessage(&msg);
            InitHttpMessage(&msg, kHttpResponse);
            memmove(inbuf.p, inbuf.p + hdrsize, inbuf.n - hdrsize);
            inbuf.n -= hdrsize;
            break;
          }
          if (msg.status == 204 || msg.status == 304) {
            goto Finished;
          }
          if (FetchHasHeader(kHttpTransferEncoding) &&
              !FetchHeaderEqualCase(kHttpTransferEncoding, "identity")) {
            if (FetchHeaderEqualCase(kHttpTransferEncoding, "chunked")) {
              t = kHttpClientStateBodyChunked;
              bzero(&u, sizeof(u));
              goto Chunked;
            } else {
              WARNF("(ftch) HTTP client %s error", "Transfer-Encoding");
              goto TransportError;
            }
          } else if (FetchHasHeader(kHttpContentLength)) {
            rc = ParseContentLength(FetchHeaderData(kHttpContentLength),
                                    FetchHeaderLength(kHttpContentLength));
            if (rc == -1) {
              WARNF("(ftch) ParseContentLength(%`'.*s) failed",
                    FetchHeaderLength(kHttpContentLength),
                    FetchHeaderData(kHttpContentLength));
              goto TransportError;
            }
            if ((paylen = rc) <= inbuf.n - hdrsize) {
              goto Finished;
            } else {
              t = kHttpClientStateBodyLengthed;
            }
          } else {
            t = kHttpClientStateBody;
          }
        }
        break;
      case kHttpClientStateBody:
        if (!g) {
          paylen = inbuf.n - hdrsize;
          goto Finished;
        }
        break;
      case kHttpClientStateBodyLengthed:
        if (!g) {
          WARNF("(ftch) HTTP client %s error", "EOF body");
          goto TransportError;
        }
        if (inbuf.n - hdrsize >= paylen) {
          goto Finished;
        }
        break;
      case kHttpClientStateBodyChunked:
      Chunked:
        rc = Unchunk(&u, inbuf.p + hdrsize, inbuf.n - hdrsize, &paylen);
        if (rc == -1) {
          WARNF("(ftch) HTTP client %s error", "Unchunk");
          goto TransportError;
        }
        if (rc) goto Finished;
        break;
      default:
        unreachable;
    }
  }

Finished:
  if (paylen && logbodies) LogBody("received", inbuf.p + hdrsize, paylen);
  VERBOSEF("(ftch) completed %s HTTP%02d %d %s %`'.*s", method, msg.version,
           msg.status, urlarg, FetchHeaderLength(kHttpServer),
           FetchHeaderData(kHttpServer));
  if (followredirect && FetchHasHeader(kHttpLocation) &&
      (msg.status == 301 || msg.status == 308 ||  // permanent redirects
       msg.status == 302 || msg.status == 307 ||  // temporary redirects
       msg.status == 303 /* see other; non-GET changes to GET, body lost */) &&
      numredirects < maxredirects) {
    // if 303, then remove body and set method to GET
    if (msg.status == 303) {
      body = "";
      bodylen = 0;
      method = kHttpMethod[kHttpGet];
    }
    // create table if needed
    if (!lua_istable(L, 2)) {
      lua_settop(L, 1);          // pop body if present
      lua_createtable(L, 0, 3);  // body, method, numredirects
    }
    lua_pushlstring(L, body, bodylen);
    lua_setfield(L, -2, "body");

    lua_pushstring(L, method);
    lua_setfield(L, -2, "method");

    lua_pushinteger(L, numredirects + 1);
    lua_setfield(L, -2, "numredirects");
    // replace URL with Location header
    lua_pushlstring(L, FetchHeaderData(kHttpLocation),
                    FetchHeaderLength(kHttpLocation));
    lua_replace(L, -3);

    DestroyHttpMessage(&msg);
    free(inbuf.p);
    close(sock);
    return LuaFetch(L);
  } else {
    lua_pushinteger(L, msg.status);
    LuaPushHeaders(L, &msg, inbuf.p);
    lua_pushlstring(L, inbuf.p + hdrsize, paylen);
    DestroyHttpMessage(&msg);
    free(inbuf.p);
    close(sock);
    return 3;
  }
TransportError:
  DestroyHttpMessage(&msg);
  free(inbuf.p);
  close(sock);
  return LuaNilError(L, "transport error");
#ifndef UNSECURE
VerifyFailed:
  LockInc(&shared->c.sslverifyfailed);
  close(sock);
  return LuaNilTlsError(
      L, _gc(DescribeSslVerifyFailure(sslcli.session_negotiate->verify_result)),
      ret);
#endif
#undef ssl
}
