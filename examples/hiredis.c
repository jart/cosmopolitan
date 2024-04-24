#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/runtime/runtime.h"
#include "libc/fmt/conv.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/hiredis/hiredis.h"

/**
 * @fileoverview Demo of using hiredis to connect to a Redis server
 */

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 5) {
    fprintf(stderr, "Usage: %s HOST PORT [KEY] [VALUE]\n", argv[0]);
    exit(1);
  }

  bool setValue = (argc == 5), getValue = (argc == 4);

  redisContext *c = redisConnect(argv[1], atoi(argv[2]));
  if (c == NULL || c->err) {
    if (c) {
      fprintf(stderr, "Connection error: %s\n", c->errstr);
      redisFree(c);
    } else {
      fputs("Failed to allocate redis context\n", stderr);
    }
    exit(1);
  }

  redisReply *reply;
  if (!setValue && !getValue) {
    reply = redisCommand(c, "PING");
    printf("PING: %s\n", reply->str);
  } else if (setValue) {
    reply = redisCommand(c, "SET %s %s", argv[3], argv[4]);
    printf("SET: %s\n", reply->str);
  } else {
    reply = redisCommand(c, "GET %s", argv[3]);
    printf("GET: %s\n", reply->str);
  }

  freeReplyObject(reply);
  redisFree(c);
  return 0;
}
