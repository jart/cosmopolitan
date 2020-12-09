#include "third_party/chibicc/test/test.h"

int main() {
  typeof("str") StringInitParen1 = ("str");
  char StringInitParen2[] = ("str");

  ASSERT(0, ""[0]);
  ASSERT(1, sizeof(""));

  ASSERT(97, "abc"[0]);
  ASSERT(98, "abc"[1]);
  ASSERT(99, "abc"[2]);
  ASSERT(0, "abc"[3]);
  ASSERT(4, sizeof("abc"));

  ASSERT(7, "\a"[0]);
  ASSERT(8, "\b"[0]);
  ASSERT(9, "\t"[0]);
  ASSERT(10, "\n"[0]);
  ASSERT(11, "\v"[0]);
  ASSERT(12, "\f"[0]);
  ASSERT(13, "\r"[0]);
  ASSERT(27, "\e"[0]);

  ASSERT(106, "\j"[0]);
  ASSERT(107, "\k"[0]);
  ASSERT(108, "\l"[0]);

  ASSERT(7, "\ax\ny"[0]);
  ASSERT(120, "\ax\ny"[1]);
  ASSERT(10, "\ax\ny"[2]);
  ASSERT(121, "\ax\ny"[3]);

  ASSERT(0, "\0"[0]);
  ASSERT(16, "\20"[0]);
  ASSERT(65, "\101"[0]);
  ASSERT(104, "\1500"[0]);
  ASSERT(0, "\x00"[0]);
  ASSERT(119, "\x77"[0]);

  ASSERT(7, sizeof("abc"
                   "def"));
  ASSERT(9, sizeof("abc"
                   "d"
                   "efgh"));
  ASSERT(0, strcmp("abc"
                   "d"
                   "\nefgh",
                   "abcd\nefgh"));
  ASSERT(0, !strcmp("abc"
                    "d",
                    "abcd\nefgh"));
  ASSERT(0, strcmp("\x9"
                   "0",
                   "\t0"));

  ASSERT(16, sizeof(L"abc"
                    ""));

  ASSERT(28, sizeof(L"abc"
                    "def"));
  ASSERT(28, sizeof(L"abc"
                    L"def"));
  ASSERT(14, sizeof(u"abc"
                    "def"));
  ASSERT(14, sizeof(u"abc"
                    u"def"));

  ASSERT(L'a', (L"abc"
                "def")[0]);
  ASSERT(L'd', (L"abc"
                "def")[3]);
  ASSERT(L'\0', (L"abc"
                 "def")[6]);

  ASSERT(u'a', (u"abc"
                "def")[0]);
  ASSERT(u'd', (u"abc"
                "def")[3]);
  ASSERT(u'\0', (u"abc"
                 "def")[6]);

  ASSERT(L'あ', ("あ"
                 L"")[0]);
  ASSERT(0343, ("\343\201\202"
                L"")[0]);
  ASSERT(0201, ("\343\201\202"
                L"")[1]);
  ASSERT(0202, ("\343\201\202"
                L"")[2]);
  ASSERT(0, ("\343\201\202"
             L"")[3]);

  ASSERT(L'a', ("a"
                "b"
                L"c")[0]);
  ASSERT(L'b', ("a"
                "b"
                L"c")[1]);
  ASSERT(L'c', ("a"
                "b"
                L"c")[2]);
  ASSERT(0, ("a"
             "b"
             L"c")[3]);

  return 0;
}
