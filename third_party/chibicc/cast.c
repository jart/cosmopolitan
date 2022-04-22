#include "third_party/chibicc/chibicc.h"

#define REDZONE(X)  \
  "sub\t$16,%rsp\n" \
  "\t" X "\n"       \
  "\tadd\t$16,%rsp"

#define PUSHPOPRAX(X) \
  "push\t%rax\n"      \
  "\t" X "\n"         \
  "\tpop\t%rax"

#ifdef __SSE3__
#define FIST(X) REDZONE("fistt" X)
#else
#define FIST(X)                      \
  REDZONE("fnstcw\t8(%rsp)\n"        \
          "\tmovzwl\t8(%rsp),%eax\n" \
          "\tor\t$12,%ah\n"          \
          "\tmov\t%ax,10(%rsp)\n"    \
          "\tfldcw\t10(%rsp)\n"      \
          "\tfist" X "\n"            \
          "\tfldcw\t8(%rsp)")
#endif

#define u64f64              \
  "test\t%rax,%rax\n"       \
  "\tjs\t1f\n"              \
  "\tpxor\t%xmm0,%xmm0\n"   \
  "\tcvtsi2sd %rax,%xmm0\n" \
  "\tjmp\t2f\n"             \
  "1:\n"                    \
  "\tmov\t%rax,%rdi\n"      \
  "\tand\t$1,%eax\n"        \
  "\tpxor\t%xmm0,%xmm0\n"   \
  "\tshr\t%rdi\n"           \
  "\tor\t%rax,%rdi\n"       \
  "\tcvtsi2sd %rdi,%xmm0\n" \
  "\taddsd\t%xmm0,%xmm0\n"  \
  "2:"

#define u64f80                              \
  PUSHPOPRAX("fildq\t(%rsp)\n"              \
             "\ttest\t%rax,%rax\n"          \
             "\tjns\t1f\n"                  \
             "\tmovq\t$0x5f800000,(%rsp)\n" \
             "\tfadds\t(%rsp)\n"            \
             "1:")

#define i32i8   "movsbl\t%al,%eax"
#define i32u8   "movzbl\t%al,%eax"
#define i32i16  "cwtl"
#define i32u16  "movzwl\t%ax,%eax"
#define i32f32  "cvtsi2ssl %eax,%xmm0"
#define i32i64  "cltq"
#define i32f64  "cvtsi2sdl %eax,%xmm0"
#define u32i64  "mov\t%eax,%eax"
#define i64f32  "cvtsi2ssq %rax,%xmm0"
#define i64f64  "cvtsi2sdq %rax,%xmm0"
#define f32i32  "cvttss2sil %xmm0,%eax"
#define f32u32  "cvttss2siq %xmm0,%rax"
#define f32i64  "cvttss2siq %xmm0,%rax"
#define f32u64  "cvttss2siq %xmm0,%rax"
#define f32f64  "cvtss2sd %xmm0,%xmm0"
#define f64i32  "cvttsd2sil %xmm0,%eax"
#define f64u32  "cvttsd2siq %xmm0,%rax"
#define f64i64  "cvttsd2siq %xmm0,%rax"
#define f64u64  "cvttsd2siq %xmm0,%rax"
#define f64f32  "cvtsd2ss %xmm0,%xmm0"
#define u64f32  "cvtsi2ssq %rax,%xmm0"
#define f32i8   "cvttss2sil %xmm0,%eax\n\tmovsbl\t%al,%eax"
#define f32u8   "cvttss2sil %xmm0,%eax\n\tmovzbl\t%al,%eax"
#define f32i16  "cvttss2sil %xmm0,%eax\n\tmovswl\t%ax,%eax"
#define f32u16  "cvttss2sil %xmm0,%eax\n\tmovzwl\t%ax,%eax"
#define f64i8   "cvttsd2sil %xmm0,%eax\n\tmovsbl\t%al,%eax"
#define f64u8   "cvttsd2sil %xmm0,%eax\n\tmovzbl\t%al,%eax"
#define f64i16  "cvttsd2sil %xmm0,%eax\n\tmovswl\t%ax,%eax"
#define f64u16  "cvttsd2sil %xmm0,%eax\n\tmovzwl\t%ax,%eax"
#define f64f80  REDZONE("movsd\t%xmm0,(%rsp)\n\tfldl\t(%rsp)")
#define f80i8   FIST("ps\t(%rsp)\n\tmovsbl\t(%rsp),%eax")
#define f80u8   FIST("ps\t(%rsp)\n\tmovzbl\t(%rsp),%eax")
#define f80i16  FIST("ps\t(%rsp)\n\tmovzbl\t(%rsp),%eax")
#define f80u16  FIST("pl\t(%rsp)\n\tmovswl\t(%rsp),%eax")
#define f80i32  FIST("pl\t(%rsp)\n\tmov\t(%rsp),%eax")
#define f80u32  FIST("pl\t(%rsp)\n\tmov\t(%rsp),%eax")
#define f80i64  FIST("pq\t(%rsp)\n\tmov\t(%rsp),%rax")
#define f80u64  FIST("pq\t(%rsp)\n\tmov\t(%rsp),%rax")
#define f80f32  REDZONE("fstps\t(%rsp)\n\tmovss\t(%rsp),%xmm0")
#define f80f64  REDZONE("fstpl\t(%rsp)\n\tmovsd\t(%rsp),%xmm0")
#define i32f80  PUSHPOPRAX("fildl\t(%rsp)")
#define u32f32  "mov\t%eax,%eax\n\tcvtsi2ssq %rax,%xmm0"
#define u32f64  "mov\t%eax,%eax\n\tcvtsi2sdq %rax,%xmm0"
#define u32f80  "mov\t%eax,%eax\n\tpush %rax\n\tfildll\t(%rsp)\n\tpop\t%rax"
#define i64f80  PUSHPOPRAX("fildll\t(%rsp)")
#define f32f80  REDZONE("movss\t%xmm0,(%rsp)\n\tflds\t(%rsp)")
#define i8i128  "movsbq\t%al,%rax\n\tcqto"
#define i16i128 "movswq\t%ax,%rax\n\tcqto"
#define i32i128 "cltq\n\tcqto"
#define i64i128 "cqto"
#define u8i128  "movzbq\t%al,%rax\n\txor\t%edx,%edx"
#define u16i128 "movzwq\t%ax,%rax\n\txor\t%edx,%edx"
#define u32i128 "cltq\n\txor\t%edx,%edx"
#define u64i128 "xor\t%edx,%edx"
#define i128f32 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floattisf"
#define i128f64 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floattidf"
#define i128f80 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floattixf"
#define u128f32 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floatuntisf"
#define u128f64 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floatuntidf"
#define u128f80 "mov\t%rax,%rdi\n\tmov\t%rdx,%rsi\n\tcall\t__floatuntixf"
#define f32i128 "call\t__fixsfti"
#define f64i128 "call\t__fixdfti"
#define f80i128 REDZONE("fstpt\t(%rsp)\n\tcall\t__fixxfti")
#define f32u128 "call\t__fixunssfti"
#define f64u128 "call\t__fixunsdfti"
#define f80u128 REDZONE("fstpt\t(%rsp)\n\tcall\t__fixunsxfti")

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80, I128, U128 };
static const char *const cast_table[13][13] = /* clang-format off */ {
  // i8    i16      i32      i64      u8      u16      u32      u64      f32      f64      f80      i128      u128
  {NULL,   NULL,    NULL,    i32i64,  i32u8,  i32u16,  NULL,    i32i64,  i32f32,  i32f64,  i32f80,  i32i128, i32i128}, // i8
  {i32i8,  NULL,    NULL,    i32i64,  i32u8,  i32u16,  NULL,    i32i64,  i32f32,  i32f64,  i32f80,  i32i128, i32i128}, // i16
  {i32i8,  i32i16,  NULL,    i32i64,  i32u8,  i32u16,  NULL,    i32i64,  i32f32,  i32f64,  i32f80,  i32i128, i32i128}, // i32
  {i32i8,  i32i16,  NULL,    NULL,    i32u8,  i32u16,  NULL,    NULL,    i64f32,  i64f64,  i64f80,  i64i128, i64i128}, // i64
  {i32i8,  NULL,    NULL,    i32i64,  NULL,   NULL,    NULL,    i32i64,  i32f32,  i32f64,  i32f80,  i32i128, i32i128}, // u8
  {i32i8,  i32i16,  NULL,    i32i64,  i32u8,  NULL,    NULL,    i32i64,  i32f32,  i32f64,  i32f80,  i32i128, i32i128}, // u16
  {i32i8,  i32i16,  NULL,    u32i64,  i32u8,  i32u16,  NULL,    u32i64,  u32f32,  u32f64,  u32f80,  u32i128, i32i128}, // u32
  {i32i8,  i32i16,  NULL,    NULL,    i32u8,  i32u16,  NULL,    NULL,    u64f32,  u64f64,  u64f80,  u64i128, u64i128}, // u64
  {f32i8,  f32i16,  f32i32,  f32i64,  f32u8,  f32u16,  f32u32,  f32u64,  NULL,    f32f64,  f32f80,  f32i128, f32u128}, // f32
  {f64i8,  f64i16,  f64i32,  f64i64,  f64u8,  f64u16,  f64u32,  f64u64,  f64f32,  NULL,    f64f80,  f64i128, f64u128}, // f64
  {f80i8,  f80i16,  f80i32,  f80i64,  f80u8,  f80u16,  f80u32,  f80u64,  f80f32,  f80f64,  NULL,    f80i128, f80u128}, // f80
  {i32i8,  i32i16,  NULL,    NULL,    i32u8,  i32u16,  NULL,    NULL,    i128f32, i128f64, i128f80, NULL,    NULL   }, // i128
  {i32i8,  i32i16,  NULL,    NULL,    i32u8,  i32u16,  NULL,    NULL,    u128f32, u128f64, u128f80, NULL,    NULL   }, // u128
} /* clang-format on */;

static int getTypeId(Type *ty) {
  switch (ty->kind) {
    case TY_CHAR:
      return ty->is_unsigned ? U8 : I8;
    case TY_SHORT:
      return ty->is_unsigned ? U16 : I16;
    case TY_INT:
      return ty->is_unsigned ? U32 : I32;
    case TY_LONG:
      return ty->is_unsigned ? U64 : I64;
    case TY_INT128:
      return ty->is_unsigned ? U128 : I128;
    case TY_FLOAT:
      return F32;
    case TY_DOUBLE:
      return F64;
    case TY_LDOUBLE:
      return F80;
    default:
      return U64;
  }
}

void gen_cast(Type *from, Type *to) {
  bool skew;
  if (to->kind == TY_VOID) return;
  if (to->kind == TY_BOOL) {
    cmp_zero(from);
    println("\tsetne\t%%al");
    println("\tmovzbl\t%%al,%%eax");
    return;
  }
  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  if (cast_table[t1][t2]) {
    if ((skew = (depth & 1) && strstr(cast_table[t1][t2], "call"))) {
      println("\tsub\t$8,%%rsp");
      depth++;
    }
    println("\t%s", cast_table[t1][t2]);
    if (skew) {
      println("\tadd\t$8,%%rsp");
      depth--;
    }
  }
}
