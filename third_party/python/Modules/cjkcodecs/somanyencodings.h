#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_CJKCODECS_SOMANYENCODINGS_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_CJKCODECS_SOMANYENCODINGS_H_

#define JISX0213_ENCPAIRS 46

COSMOPOLITAN_C_START_

struct CjkIndex {
  uint16_t map;
  unsigned char bottom, top;
};

struct CjkPairEncodeMap {
  uint32_t uniseq;
  uint16_t code;
};

struct dbcs_map {
  const char *charset;
  const struct CjkIndex *encmap;
  const struct CjkIndex *decmap;
};

const struct CjkIndex *big5_decmap(void) pureconst;
const struct CjkIndex *big5_encmap(void) pureconst;
const struct CjkIndex *big5hkscs_bmp_encmap(void) pureconst;
const struct CjkIndex *big5hkscs_decmap(void) pureconst;
const struct CjkIndex *big5hkscs_nonbmp_encmap(void) pureconst;
const struct CjkIndex *cp932ext_decmap(void) pureconst;
const struct CjkIndex *cp932ext_encmap(void) pureconst;
const struct CjkIndex *cp949_encmap(void) pureconst;
const struct CjkIndex *cp949ext_decmap(void) pureconst;
const struct CjkIndex *cp950ext_decmap(void) pureconst;
const struct CjkIndex *cp950ext_encmap(void) pureconst;
const struct CjkIndex *gb18030ext_decmap(void) pureconst;
const struct CjkIndex *gb18030ext_encmap(void) pureconst;
const struct CjkIndex *gb2312_decmap(void) pureconst;
const struct CjkIndex *gbcommon_encmap(void) pureconst;
const struct CjkIndex *gbkext_decmap(void) pureconst;
const struct CjkIndex *jisx0208_decmap(void) pureconst;
const struct CjkIndex *jisx0212_decmap(void) pureconst;
const struct CjkIndex *jisx0213_1_bmp_decmap(void) pureconst;
const struct CjkIndex *jisx0213_1_emp_decmap(void) pureconst;
const struct CjkIndex *jisx0213_2_bmp_decmap(void) pureconst;
const struct CjkIndex *jisx0213_2_emp_decmap(void) pureconst;
const struct CjkIndex *jisx0213_bmp_encmap(void) pureconst;
const struct CjkIndex *jisx0213_emp_encmap(void) pureconst;
const struct CjkIndex *jisx0213_pair_decmap(void) pureconst;
const struct CjkIndex *jisxcommon_encmap(void) pureconst;
const struct CjkIndex *ksx1001_decmap(void) pureconst;
const struct CjkPairEncodeMap *jisx0213_pair_encmap(void) pureconst;
const uint16_t *__big5_decmap(void) pureconst;
const uint16_t *__big5_encmap(void) pureconst;
const uint16_t *__big5hkscs_bmp_encmap(void) pureconst;
const uint16_t *__big5hkscs_decmap(void) pureconst;
const uint16_t *__big5hkscs_nonbmp_encmap(void) pureconst;
const uint16_t *__cp932ext_decmap(void) pureconst;
const uint16_t *__cp932ext_encmap(void) pureconst;
const uint16_t *__cp949_encmap(void) pureconst;
const uint16_t *__cp949ext_decmap(void) pureconst;
const uint16_t *__cp950ext_decmap(void) pureconst;
const uint16_t *__cp950ext_encmap(void) pureconst;
const uint16_t *__gb18030ext_decmap(void) pureconst;
const uint16_t *__gb18030ext_encmap(void) pureconst;
const uint16_t *__gb2312_decmap(void) pureconst;
const uint16_t *__gbcommon_encmap(void) pureconst;
const uint16_t *__gbkext_decmap(void) pureconst;
const uint16_t *__jisx0208_decmap(void) pureconst;
const uint16_t *__jisx0212_decmap(void) pureconst;
const uint16_t *__jisx0213_1_bmp_decmap(void) pureconst;
const uint16_t *__jisx0213_1_emp_decmap(void) pureconst;
const uint16_t *__jisx0213_2_bmp_decmap(void) pureconst;
const uint16_t *__jisx0213_2_emp_decmap(void) pureconst;
const uint16_t *__jisx0213_bmp_encmap(void) pureconst;
const uint16_t *__jisx0213_emp_encmap(void) pureconst;
const uint16_t *__jisxcommon_encmap(void) pureconst;
const uint16_t *__ksx1001_decmap(void) pureconst;
const uint32_t *__jisx0213_pair_decmap(void) pureconst;

COSMOPOLITAN_C_END_
#endif /*COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_CJKCODECS_SOMANYENCODINGS_H_*/
