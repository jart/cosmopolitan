#include <errno.h>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#define INBUF_SIZE  1024
#define OUTBUF_SIZE 2048

int g_count;

int check_conversion(const char* input, size_t input_len,
                     const char16_t* expected_output, size_t expected_len) {
  iconv_t cd;
  char inbuf[INBUF_SIZE];
  char outbuf[OUTBUF_SIZE];
  char* inptr = inbuf;
  char* outptr = outbuf;
  size_t inbytesleft = input_len;
  size_t outbytesleft = OUTBUF_SIZE;
  size_t result;

  ++g_count;

  memcpy(inbuf, input, input_len);

  cd = iconv_open("UTF-16LE", "UTF-8");
  if (cd == (iconv_t)-1) {
    return 10 + g_count;  // iconv_open failed
  }

  result = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
  if (result == (size_t)-1) {
    iconv_close(cd);
    return 20 + g_count;  // iconv failed, return 20 + specific errno
  }

  if (inbytesleft != 0) {
    iconv_close(cd);
    return 40 + g_count;  // Not all input was converted
  }

  size_t output_len = OUTBUF_SIZE - outbytesleft;
  if (output_len != expected_len) {
    iconv_close(cd);
    return 50 + g_count;  // Output length mismatch
  }

  if (memcmp(outbuf, expected_output, output_len) != 0) {
    iconv_close(cd);
    return 60 + g_count;  // Output content mismatch
  }

  if (iconv_close(cd) == -1)
    return 70 + g_count;  // iconv_close failed

  // Reverse direction check: UTF-16LE back to UTF-8
  cd = iconv_open("UTF-8", "UTF-16LE");
  if (cd == (iconv_t)-1) {
    return 80 + g_count;  // iconv_open failed for reverse direction
  }

  char reverse_inbuf[OUTBUF_SIZE];
  char reverse_outbuf[INBUF_SIZE];
  char* reverse_inptr = reverse_inbuf;
  char* reverse_outptr = reverse_outbuf;
  size_t reverse_inbytesleft = output_len;
  size_t reverse_outbytesleft = INBUF_SIZE;

  memcpy(reverse_inbuf, outbuf, output_len);

  result = iconv(cd, &reverse_inptr, &reverse_inbytesleft, &reverse_outptr,
                 &reverse_outbytesleft);
  if (result == (size_t)-1) {
    iconv_close(cd);
    return 90 + g_count;  // iconv failed for reverse direction
  }

  if (reverse_inbytesleft != 0) {
    iconv_close(cd);
    return 100 + g_count;  // Not all input was converted in reverse direction
  }

  size_t reverse_output_len = INBUF_SIZE - reverse_outbytesleft;
  if (reverse_output_len != input_len) {
    iconv_close(cd);
    return 110 + g_count;  // Reverse output length mismatch
  }

  if (memcmp(reverse_outbuf, input, input_len) != 0) {
    iconv_close(cd);
    return 120 + g_count;  // Reverse output content mismatch
  }

  if (iconv_close(cd) == -1)
    return 130 + g_count;  // iconv_close failed for reverse direction

  return 0;  // Success
}

int main() {
  // Test case 1: Basic ASCII
  const char input1[] = "Hello, world!";
  const char16_t expected1[] = u"Hello, world!";
  int result = check_conversion(input1, sizeof(input1) - 1, expected1,
                                sizeof(expected1) - 2);
  if (result != 0)
    return result;

  // Test case 2: Non-ASCII characters and newline
  const char input2[] = "こんにちは\nWorld! ☺";
  const char16_t expected2[] = u"こんにちは\nWorld! ☺";
  result = check_conversion(input2, sizeof(input2) - 1, expected2,
                            sizeof(expected2) - 2);
  if (result != 0)
    return result;

  // Test case 3: Empty string
  const char input3[] = "";
  const char16_t expected3[] = u"";
  result = check_conversion(input3, 0, expected3, 0);
  if (result != 0)
    return result;

  // Test case 4: String with null characters
  const char input4[] = "Hello\0World";
  const char16_t expected4[] = u"Hello\0World";
  result = check_conversion(input4, sizeof(input4) - 1, expected4,
                            sizeof(expected4) - 2);
  if (result != 0)
    return result;

  // Test case 5: Long string to test buffer handling
  char input5[INBUF_SIZE];
  char16_t expected5[INBUF_SIZE];
  memset(input5, 'A', INBUF_SIZE - 1);
  input5[INBUF_SIZE - 1] = '\0';
  for (int i = 0; i < INBUF_SIZE - 1; i++) {
    expected5[i] = u'A';
  }
  result =
      check_conversion(input5, INBUF_SIZE - 1, expected5, (INBUF_SIZE - 1) * 2);
  if (result != 0)
    return result;

  // Test case 6: Invalid UTF-8 sequence
  const char input6[] = {0xC0, 0x80};
  result = check_conversion(input6, sizeof(input6), NULL, 0);
  if (result != 26) {
    if (errno != EILSEQ)
      return 201;
    return 200;
  }

  // Test case 7: Mixing ASCII and non-ASCII
  const char input7[] = "Hello, 世界!";
  const char16_t expected7[] = u"Hello, 世界!";
  result = check_conversion(input7, sizeof(input7) - 1, expected7,
                            sizeof(expected7) - 2);
  if (result != 0)
    return result;

  // Test case 8: Surrogate pairs
  const char input8[] = "𐐷";  // U+10437
  const char16_t expected8[] =
      u"𐐷";  // This will be encoded as a surrogate pair
  result = check_conversion(input8, sizeof(input8) - 1, expected8,
                            sizeof(expected8) - 2);
  if (result != 0)
    return result;

  return 0;  // All tests passed
}
