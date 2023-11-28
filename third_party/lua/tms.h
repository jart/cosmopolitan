#ifndef COSMOPOLITAN_THIRD_PARTY_LUA_TMS_H_
#define COSMOPOLITAN_THIRD_PARTY_LUA_TMS_H_
COSMOPOLITAN_C_START_

/*
 * WARNING: if you change the order of this enumeration,
 * grep "ORDER TM" and "ORDER OP"
 */
typedef enum {
  TM_INDEX,
  TM_NEWINDEX,
  TM_GC,
  TM_MODE,
  TM_LEN,
  TM_EQ, /* last tag method with fast access */
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_MOD,
  TM_POW,
  TM_DIV,
  TM_IDIV,
  TM_BAND,
  TM_BOR,
  TM_BXOR,
  TM_SHL,
  TM_SHR,
  TM_UNM,
  TM_BNOT,
  TM_LT,
  TM_LE,
  TM_CONCAT,
  TM_CALL,
  TM_CLOSE,
  TM_N /* number of elements in the enum */
} TMS;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LUA_TMS_H_ */
