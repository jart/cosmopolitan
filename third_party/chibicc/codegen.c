#include "third_party/chibicc/chibicc.h"

#define GP_MAX 6
#define FP_MAX 8

static FILE *output_file;
static int depth;
static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *argreg16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char *argreg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;

static void gen_expr(Node *node);
static void gen_stmt(Node *node);

__attribute__((format(printf, 1, 2))) static void println(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(output_file, fmt, ap);
  fprintf(output_file, "\n");
}

static int count(void) {
  static int i = 1;
  return i++;
}

static void push(void) {
  println("  push %%rax");
  depth++;
}

static void pop(char *arg) {
  println("  pop %s", arg);
  depth--;
}

static void pushf(void) {
  println("  sub $8, %%rsp");
  println("  movsd %%xmm0, (%%rsp)");
  depth++;
}

static void popf(int reg) {
  println("  movsd (%%rsp), %%xmm%d", reg);
  println("  add $8, %%rsp");
  depth--;
}

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

static char *reg_dx(int sz) {
  switch (sz) {
    case 1:
      return "%dl";
    case 2:
      return "%dx";
    case 4:
      return "%edx";
    case 8:
      return "%rdx";
  }
  UNREACHABLE();
}

static char *reg_ax(int sz) {
  switch (sz) {
    case 1:
      return "%al";
    case 2:
      return "%ax";
    case 4:
      return "%eax";
    case 8:
      return "%rax";
  }
  UNREACHABLE();
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static void gen_addr(Node *node) {
  switch (node->kind) {
    case ND_VAR:
      // Variable-length array, which is always local.
      if (node->var->ty->kind == TY_VLA) {
        println("  mov %d(%%rbp), %%rax", node->var->offset);
        return;
      }

      // Local variable
      if (node->var->is_local) {
        println("  lea %d(%%rbp), %%rax", node->var->offset);
        return;
      }

      if (opt_fpic) {
        // Thread-local variable
        if (node->var->is_tls) {
          println("  data16 lea %s@tlsgd(%%rip), %%rdi", node->var->name);
          println("  .value 0x6666");
          println("  rex64");
          println("  call __tls_get_addr@PLT");
          return;
        }

        // Function or global variable
        println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name);
        return;
      }

      // Thread-local variable
      if (node->var->is_tls) {
        println("  mov %%fs:0, %%rax");
        println("  add $%s@tpoff, %%rax", node->var->name);
        return;
      }

      // Here, we generate an absolute address of a function or a global
      // variable. Even though they exist at a certain address at runtime,
      // their addresses are not known at link-time for the following
      // two reasons.
      //
      //  - Address randomization: Executables are loaded to memory as a
      //    whole but it is not known what address they are loaded to.
      //    Therefore, at link-time, relative address in the same
      //    exectuable (i.e. the distance between two functions in the
      //    same executable) is known, but the absolute address is not
      //    known.
      //
      //  - Dynamic linking: Dynamic shared objects (DSOs) or .so files
      //    are loaded to memory alongside an executable at runtime and
      //    linked by the runtime loader in memory. We know nothing
      //    about addresses of global stuff that may be defined by DSOs
      //    until the runtime relocation is complete.
      //
      // In order to deal with the former case, we use RIP-relative
      // addressing, denoted by `(%rip)`. For the latter, we obtain an
      // address of a stuff that may be in a shared object file from the
      // Global Offset Table using `@GOTPCREL(%rip)` notation.

      // Function
      if (node->ty->kind == TY_FUNC) {
        if (node->var->is_definition)
          println("  lea %s(%%rip), %%rax", node->var->name);
        else
          println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name);
        return;
      }

      // Global variable
      println("  lea %s(%%rip), %%rax", node->var->name);
      return;
    case ND_DEREF:
      gen_expr(node->lhs);
      return;
    case ND_COMMA:
      gen_expr(node->lhs);
      gen_addr(node->rhs);
      return;
    case ND_MEMBER:
      gen_addr(node->lhs);
      println("  add $%d, %%rax", node->member->offset);
      return;
    case ND_FUNCALL:
      if (node->ret_buffer) {
        gen_expr(node);
        return;
      }
      break;
    case ND_VLA_PTR:
      println("  lea %d(%%rbp), %%rax", node->var->offset);
      return;
  }

  error_tok(node->tok, "not an lvalue");
}

// Load a value from where %rax is pointing to.
static void load(Type *ty) {
  switch (ty->kind) {
    case TY_ARRAY:
    case TY_STRUCT:
    case TY_UNION:
    case TY_FUNC:
    case TY_VLA:
      // If it is an array, do not attempt to load a value to the
      // register because in general we can't load an entire array to a
      // register. As a result, the result of an evaluation of an array
      // becomes not the array itself but the address of the array.
      // This is where "array is automatically converted to a pointer to
      // the first element of the array in C" occurs.
      return;
    case TY_FLOAT:
      println("  movss (%%rax), %%xmm0");
      return;
    case TY_DOUBLE:
      println("  movsd (%%rax), %%xmm0");
      return;
    case TY_LDOUBLE:
      println("  fldt (%%rax)");
      return;
  }

  char *insn = ty->is_unsigned ? "movz" : "movs";

  // When we load a char or a short value to a register, we always
  // extend them to the size of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  if (ty->size == 1)
    println("  %sbl (%%rax), %%eax", insn);
  else if (ty->size == 2)
    println("  %swl (%%rax), %%eax", insn);
  else if (ty->size == 4)
    println("  movslq (%%rax), %%rax");
  else
    println("  mov (%%rax), %%rax");
}

// Store %rax to an address that the stack top is pointing to.
static void store(Type *ty) {
  pop("%rdi");

  switch (ty->kind) {
    case TY_STRUCT:
    case TY_UNION:
      for (int i = 0; i < ty->size; i++) {
        println("  mov %d(%%rax), %%r8b", i);
        println("  mov %%r8b, %d(%%rdi)", i);
      }
      return;
    case TY_FLOAT:
      println("  movss %%xmm0, (%%rdi)");
      return;
    case TY_DOUBLE:
      println("  movsd %%xmm0, (%%rdi)");
      return;
    case TY_LDOUBLE:
      println("  fstpt (%%rdi)");
      return;
  }

  if (ty->size == 1)
    println("  mov %%al, (%%rdi)");
  else if (ty->size == 2)
    println("  mov %%ax, (%%rdi)");
  else if (ty->size == 4)
    println("  mov %%eax, (%%rdi)");
  else
    println("  mov %%rax, (%%rdi)");
}

static void cmp_zero(Type *ty) {
  switch (ty->kind) {
    case TY_FLOAT:
      println("  xorps %%xmm1, %%xmm1");
      println("  ucomiss %%xmm1, %%xmm0");
      return;
    case TY_DOUBLE:
      println("  xorpd %%xmm1, %%xmm1");
      println("  ucomisd %%xmm1, %%xmm0");
      return;
    case TY_LDOUBLE:
      println("  fldz");
      println("  fucomip");
      println("  fstp %%st(0)");
      return;
  }

  if (is_integer(ty) && ty->size <= 4)
    println("  cmp $0, %%eax");
  else
    println("  cmp $0, %%rax");
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };

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
    case TY_FLOAT:
      return F32;
    case TY_DOUBLE:
      return F64;
    case TY_LDOUBLE:
      return F80;
  }
  return U64;
}

// The table for type casts
static char i32i8[] = "movsbl %al, %eax";
static char i32u8[] = "movzbl %al, %eax";
static char i32i16[] = "movswl %ax, %eax";
static char i32u16[] = "movzwl %ax, %eax";
static char i32f32[] = "cvtsi2ssl %eax, %xmm0";
static char i32i64[] = "movslq %eax, %rax";
static char i32f64[] = "cvtsi2sdl %eax, %xmm0";
static char i32f80[] = "mov %eax, -4(%rsp)\n  fildl -4(%rsp)";

static char u32f32[] = "mov %eax, %eax\n  cvtsi2ssq %rax, %xmm0";
static char u32i64[] = "mov %eax, %eax";
static char u32f64[] = "mov %eax, %eax\n  cvtsi2sdq %rax, %xmm0";
static char u32f80[] =
    "mov %eax, %eax\n  mov %rax, -8(%rsp)\n  fildll -8(%rsp)";

static char i64f32[] = "cvtsi2ssq %rax, %xmm0";
static char i64f64[] = "cvtsi2sdq %rax, %xmm0";
static char i64f80[] = "movq %rax, -8(%rsp)\n  fildll -8(%rsp)";

static char u64f32[] = "cvtsi2ssq %rax, %xmm0";
static char u64f64[] =
    "test %rax,%rax\n  js 1f\n  pxor %xmm0,%xmm0\n  cvtsi2sd %rax,%xmm0\n  jmp "
    "2f\n1: mov %rax,%rdi\n  and $1,%eax\n  pxor %xmm0,%xmm0\n  shr %rdi\n  "
    "or %rax,%rdi\n  cvtsi2sd %rdi,%xmm0\n  addsd %xmm0,%xmm0\n  2:";
static char u64f80[] =
    "mov %rax, -8(%rsp)\n  fildq -8(%rsp)\n  test %rax, %rax\n  jns 1f;"
    "mov $1602224128, %eax\n  mov %eax, -4(%rsp)\n  fadds -4(%rsp)\n  1:";

static char f32i8[] = "cvttss2sil %xmm0, %eax\n  movsbl %al, %eax";
static char f32u8[] = "cvttss2sil %xmm0, %eax\n  movzbl %al, %eax";
static char f32i16[] = "cvttss2sil %xmm0, %eax\n  movswl %ax, %eax";
static char f32u16[] = "cvttss2sil %xmm0, %eax\n  movzwl %ax, %eax";
static char f32i32[] = "cvttss2sil %xmm0, %eax";
static char f32u32[] = "cvttss2siq %xmm0, %rax";
static char f32i64[] = "cvttss2siq %xmm0, %rax";
static char f32u64[] = "cvttss2siq %xmm0, %rax";
static char f32f64[] = "cvtss2sd %xmm0, %xmm0";
static char f32f80[] = "movss %xmm0, -4(%rsp)\n  flds -4(%rsp)";

static char f64i8[] = "cvttsd2sil %xmm0, %eax\n  movsbl %al, %eax";
static char f64u8[] = "cvttsd2sil %xmm0, %eax\n  movzbl %al, %eax";
static char f64i16[] = "cvttsd2sil %xmm0, %eax\n  movswl %ax, %eax";
static char f64u16[] = "cvttsd2sil %xmm0, %eax\n  movzwl %ax, %eax";
static char f64i32[] = "cvttsd2sil %xmm0, %eax";
static char f64u32[] = "cvttsd2siq %xmm0, %rax";
static char f64i64[] = "cvttsd2siq %xmm0, %rax";
static char f64u64[] = "cvttsd2siq %xmm0, %rax";
static char f64f32[] = "cvtsd2ss %xmm0, %xmm0";
static char f64f80[] = "movsd %xmm0, -8(%rsp)\n  fldl -8(%rsp)";

#define FROM_F80_1                                                \
  "fnstcw -10(%rsp)\n  movzwl -10(%rsp), %eax\n  or $12, %ah\n  " \
  "mov %ax, -12(%rsp)\n  fldcw -12(%rsp)\n  "

#define FROM_F80_2 " -24(%rsp)\n  fldcw -10(%rsp)\n  "

static char f80i8[] = FROM_F80_1 "fistps" FROM_F80_2 "movsbl -24(%rsp), %eax";
static char f80u8[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";
static char f80i16[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";
static char f80u16[] = FROM_F80_1 "fistpl" FROM_F80_2 "movswl -24(%rsp), %eax";
static char f80i32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";
static char f80u32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";
static char f80i64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";
static char f80u64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";
static char f80f32[] = "fstps -8(%rsp)\n  movss -8(%rsp), %xmm0";
static char f80f64[] = "fstpl -8(%rsp)\n  movsd -8(%rsp), %xmm0";

static const char *const cast_table[11][11] = /* clang-format off */ {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i8
  {i32i8, NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i16
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i32
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   i64f32, i64f64, i64f80}, // i64
  {i32i8, NULL,   NULL,   i32i64, NULL,  NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u8
  {i32i8, i32i16, NULL,   i32i64, i32u8, NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u16
  {i32i8, i32i16, NULL,   u32i64, i32u8, i32u16, NULL,   u32i64, u32f32, u32f64, u32f80}, // u32
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   u64f32, u64f64, u64f80}, // u64
  {f32i8, f32i16, f32i32, f32i64, f32u8, f32u16, f32u32, f32u64, NULL,   f32f64, f32f80}, // f32
  {f64i8, f64i16, f64i32, f64i64, f64u8, f64u16, f64u32, f64u64, f64f32, NULL,   f64f80}, // f64
  {f80i8, f80i16, f80i32, f80i64, f80u8, f80u16, f80u32, f80u64, f80f32, f80f64, NULL},   // f80
} /* clang-format on */;

static void cast(Type *from, Type *to) {
  if (to->kind == TY_VOID) return;

  if (to->kind == TY_BOOL) {
    cmp_zero(from);
    println("  setne %%al");
    println("  movzbl %%al, %%eax");
    return;
  }

  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  if (cast_table[t1][t2]) println("  %s", cast_table[t1][t2]);
}

// Structs or unions equal or smaller than 16 bytes are passed
// using up to two registers.
//
// If the first 8 bytes contains only floating-point type members,
// they are passed in an XMM register. Otherwise, they are passed
// in a general-purpose register.
//
// If a struct/union is larger than 8 bytes, the same rule is
// applied to the the next 8 byte chunk.
//
// This function returns true if `ty` has only floating-point
// members in its byte range [lo, hi).
static bool has_flonum(Type *ty, int lo, int hi, int offset) {
  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
    for (Member *mem = ty->members; mem; mem = mem->next)
      if (!has_flonum(mem->ty, lo, hi, offset + mem->offset)) return false;
    return true;
  }

  if (ty->kind == TY_ARRAY) {
    for (int i = 0; i < ty->array_len; i++)
      if (!has_flonum(ty->base, lo, hi, offset + ty->base->size * i))
        return false;
    return true;
  }

  return offset < lo || hi <= offset || ty->kind == TY_FLOAT ||
         ty->kind == TY_DOUBLE;
}

static bool has_flonum1(Type *ty) {
  return has_flonum(ty, 0, 8, 0);
}

static bool has_flonum2(Type *ty) {
  return has_flonum(ty, 8, 16, 0);
}

static void push_struct(Type *ty) {
  int sz = align_to(ty->size, 8);
  println("  sub $%d, %%rsp", sz);
  depth += sz / 8;

  for (int i = 0; i < ty->size; i++) {
    println("  mov %d(%%rax), %%r10b", i);
    println("  mov %%r10b, %d(%%rsp)", i);
  }
}

static void push_args2(Node *args, bool first_pass) {
  if (!args) return;
  push_args2(args->next, first_pass);

  if ((first_pass && !args->pass_by_stack) ||
      (!first_pass && args->pass_by_stack))
    return;

  gen_expr(args);

  switch (args->ty->kind) {
    case TY_STRUCT:
    case TY_UNION:
      push_struct(args->ty);
      break;
    case TY_FLOAT:
    case TY_DOUBLE:
      pushf();
      break;
    case TY_LDOUBLE:
      println("  sub $16, %%rsp");
      println("  fstpt (%%rsp)");
      depth += 2;
      break;
    default:
      push();
  }
}

// Load function call arguments. Arguments are already evaluated and
// stored to the stack as local variables. What we need to do in this
// function is to load them to registers or push them to the stack as
// specified by the x86-64 psABI. Here is what the spec says:
//
// - Up to 6 arguments of integral type are passed using RDI, RSI,
//   RDX, RCX, R8 and R9.
//
// - Up to 8 arguments of floating-point type are passed using XMM0 to
//   XMM7.
//
// - If all registers of an appropriate type are already used, push an
//   argument to the stack in the right-to-left order.
//
// - Each argument passed on the stack takes 8 bytes, and the end of
//   the argument area must be aligned to a 16 byte boundary.
//
// - If a function is variadic, set the number of floating-point type
//   arguments to RAX.
static int push_args(Node *node) {
  int stack = 0, gp = 0, fp = 0;

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (node->ret_buffer && node->ty->size > 16) gp++;

  // Load as many arguments to the registers as possible.
  for (Node *arg = node->args; arg; arg = arg->next) {
    Type *ty = arg->ty;

    switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
        if (ty->size > 16) {
          arg->pass_by_stack = true;
          stack += align_to(ty->size, 8) / 8;
        } else {
          bool fp1 = has_flonum1(ty);
          bool fp2 = has_flonum2(ty);

          if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
            fp = fp + fp1 + fp2;
            gp = gp + !fp1 + !fp2;
          } else {
            arg->pass_by_stack = true;
            stack += align_to(ty->size, 8) / 8;
          }
        }
        break;
      case TY_FLOAT:
      case TY_DOUBLE:
        if (fp++ >= FP_MAX) {
          arg->pass_by_stack = true;
          stack++;
        }
        break;
      case TY_LDOUBLE:
        arg->pass_by_stack = true;
        stack += 2;
        break;
      default:
        if (gp++ >= GP_MAX) {
          arg->pass_by_stack = true;
          stack++;
        }
    }
  }

  if ((depth + stack) % 2 == 1) {
    println("  sub $8, %%rsp");
    depth++;
    stack++;
  }

  push_args2(node->args, true);
  push_args2(node->args, false);

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (node->ret_buffer && node->ty->size > 16) {
    println("  lea %d(%%rbp), %%rax", node->ret_buffer->offset);
    push();
  }

  return stack;
}

static void copy_ret_buffer(Obj *var) {
  Type *ty = var->ty;
  int gp = 0, fp = 0;

  if (has_flonum1(ty)) {
    assert(ty->size == 4 || 8 <= ty->size);
    if (ty->size == 4)
      println("  movss %%xmm0, %d(%%rbp)", var->offset);
    else
      println("  movsd %%xmm0, %d(%%rbp)", var->offset);
    fp++;
  } else {
    for (int i = 0; i < MIN(8, ty->size); i++) {
      println("  mov %%al, %d(%%rbp)", var->offset + i);
      println("  shr $8, %%rax");
    }
    gp++;
  }

  if (ty->size > 8) {
    if (has_flonum2(ty)) {
      assert(ty->size == 12 || ty->size == 16);
      if (ty->size == 12)
        println("  movss %%xmm%d, %d(%%rbp)", fp, var->offset + 8);
      else
        println("  movsd %%xmm%d, %d(%%rbp)", fp, var->offset + 8);
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";
      for (int i = 8; i < MIN(16, ty->size); i++) {
        println("  mov %s, %d(%%rbp)", reg1, var->offset + i);
        println("  shr $8, %s", reg2);
      }
    }
  }
}

static void copy_struct_reg(void) {
  Type *ty = current_fn->ty->return_ty;
  int gp = 0, fp = 0;

  println("  mov %%rax, %%rdi");

  if (has_flonum(ty, 0, 8, 0)) {
    assert(ty->size == 4 || 8 <= ty->size);
    if (ty->size == 4)
      println("  movss (%%rdi), %%xmm0");
    else
      println("  movsd (%%rdi), %%xmm0");
    fp++;
  } else {
    println("  mov $0, %%rax");
    for (int i = MIN(8, ty->size) - 1; i >= 0; i--) {
      println("  shl $8, %%rax");
      println("  mov %d(%%rdi), %%al", i);
    }
    gp++;
  }

  if (ty->size > 8) {
    if (has_flonum(ty, 8, 16, 0)) {
      assert(ty->size == 12 || ty->size == 16);
      if (ty->size == 4)
        println("  movss 8(%%rdi), %%xmm%d", fp);
      else
        println("  movsd 8(%%rdi), %%xmm%d", fp);
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";
      println("  mov $0, %s", reg2);
      for (int i = MIN(16, ty->size) - 1; i >= 8; i--) {
        println("  shl $8, %s", reg2);
        println("  mov %d(%%rdi), %s", i, reg1);
      }
    }
  }
}

static void copy_struct_mem(void) {
  Type *ty = current_fn->ty->return_ty;
  Obj *var = current_fn->params;

  println("  mov %d(%%rbp), %%rdi", var->offset);

  for (int i = 0; i < ty->size; i++) {
    println("  mov %d(%%rax), %%dl", i);
    println("  mov %%dl, %d(%%rdi)", i);
  }
}

static void builtin_alloca(void) {
  // Align size to 16 bytes.
  println("  add $15, %%rdi");
  println("  and $0xfffffff0, %%edi");

  // Shift the temporary area by %rdi.
  println("  mov %d(%%rbp), %%rcx", current_fn->alloca_bottom->offset);
  println("  sub %%rsp, %%rcx");
  println("  mov %%rsp, %%rax");
  println("  sub %%rdi, %%rsp");
  println("  mov %%rsp, %%rdx");
  println("1:");
  println("  cmp $0, %%rcx");
  println("  je 2f");
  println("  mov (%%rax), %%r8b");
  println("  mov %%r8b, (%%rdx)");
  println("  inc %%rdx");
  println("  inc %%rax");
  println("  dec %%rcx");
  println("  jmp 1b");
  println("2:");

  // Move alloca_bottom pointer.
  println("  mov %d(%%rbp), %%rax", current_fn->alloca_bottom->offset);
  println("  sub %%rdi, %%rax");
  println("  mov %%rax, %d(%%rbp)", current_fn->alloca_bottom->offset);
}

// Generate code for a given node.
static void gen_expr(Node *node) {
  println("  .loc %d %d", node->tok->file->file_no, node->tok->line_no);

  switch (node->kind) {
    case ND_NULL_EXPR:
      return;
    case ND_NUM: {
      switch (node->ty->kind) {
        case TY_FLOAT: {
          union {
            float f32;
            uint32_t u32;
          } u = {node->fval};
          println("  mov $%u, %%eax  # float %Lf", u.u32, node->fval);
          println("  movq %%rax, %%xmm0");
          return;
        }
        case TY_DOUBLE: {
          union {
            double f64;
            uint64_t u64;
          } u = {node->fval};
          println("  mov $%lu, %%rax  # double %Lf", u.u64, node->fval);
          println("  movq %%rax, %%xmm0");
          return;
        }
        case TY_LDOUBLE: {
          union {
            long double f80;
            uint64_t u64[2];
          } u;
          memset(&u, 0, sizeof(u));
          u.f80 = node->fval;
          println("  mov $%lu, %%rax  # long double %Lf", u.u64[0], node->fval);
          println("  mov %%rax, -16(%%rsp)");
          println("  mov $%lu, %%rax", u.u64[1]);
          println("  mov %%rax, -8(%%rsp)");
          println("  fldt -16(%%rsp)");
          return;
        }
      }

      println("  mov $%ld, %%rax", node->val);
      return;
    }
    case ND_NEG:
      gen_expr(node->lhs);

      switch (node->ty->kind) {
        case TY_FLOAT:
          println("  mov $1, %%rax");
          println("  shl $31, %%rax");
          println("  movq %%rax, %%xmm1");
          println("  xorps %%xmm1, %%xmm0");
          return;
        case TY_DOUBLE:
          println("  mov $1, %%rax");
          println("  shl $63, %%rax");
          println("  movq %%rax, %%xmm1");
          println("  xorpd %%xmm1, %%xmm0");
          return;
        case TY_LDOUBLE:
          println("  fchs");
          return;
      }

      println("  neg %%rax");
      return;
    case ND_VAR:
      gen_addr(node);
      load(node->ty);
      return;
    case ND_MEMBER: {
      gen_addr(node);
      load(node->ty);

      Member *mem = node->member;
      if (mem->is_bitfield) {
        println("  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);
        if (mem->ty->is_unsigned)
          println("  shr $%d, %%rax", 64 - mem->bit_width);
        else
          println("  sar $%d, %%rax", 64 - mem->bit_width);
      }
      return;
    }
    case ND_DEREF:
      gen_expr(node->lhs);
      load(node->ty);
      return;
    case ND_ADDR:
      gen_addr(node->lhs);
      return;
    case ND_ASSIGN:
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);

      if (node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield) {
        println("  mov %%rax, %%r8");

        // If the lhs is a bitfield, we need to read the current value
        // from memory and merge it with a new value.
        Member *mem = node->lhs->member;
        println("  mov %%rax, %%rdi");
        println("  and $%ld, %%rdi", (1L << mem->bit_width) - 1);
        println("  shl $%d, %%rdi", mem->bit_offset);

        println("  mov (%%rsp), %%rax");
        load(mem->ty);

        long mask = ((1L << mem->bit_width) - 1) << mem->bit_offset;
        println("  mov $%ld, %%r9", ~mask);
        println("  and %%r9, %%rax");
        println("  or %%rdi, %%rax");
        store(node->ty);
        println("  mov %%r8, %%rax");
        return;
      }

      store(node->ty);
      return;
    case ND_STMT_EXPR:
      for (Node *n = node->body; n; n = n->next) gen_stmt(n);
      return;
    case ND_COMMA:
      gen_expr(node->lhs);
      gen_expr(node->rhs);
      return;
    case ND_CAST:
      gen_expr(node->lhs);
      cast(node->lhs->ty, node->ty);
      return;
    case ND_MEMZERO:
      // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
      println("  mov $%d, %%rcx", node->var->ty->size);
      println("  lea %d(%%rbp), %%rdi", node->var->offset);
      println("  mov $0, %%al");
      println("  rep stosb");
      return;
    case ND_COND: {
      int c = count();
      gen_expr(node->cond);
      cmp_zero(node->cond->ty);
      println("  je .L.else.%d", c);
      gen_expr(node->then);
      println("  jmp .L.end.%d", c);
      println(".L.else.%d:", c);
      gen_expr(node->els);
      println(".L.end.%d:", c);
      return;
    }
    case ND_NOT:
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      println("  sete %%al");
      println("  movzbl %%al, %%rax");
      return;
    case ND_BITNOT:
      gen_expr(node->lhs);
      println("  not %%rax");
      return;
    case ND_LOGAND: {
      int c = count();
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      println("  je .L.false.%d", c);
      gen_expr(node->rhs);
      cmp_zero(node->rhs->ty);
      println("  je .L.false.%d", c);
      println("  mov $1, %%rax");
      println("  jmp .L.end.%d", c);
      println(".L.false.%d:", c);
      println("  mov $0, %%rax");
      println(".L.end.%d:", c);
      return;
    }
    case ND_LOGOR: {
      int c = count();
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      println("  jne .L.true.%d", c);
      gen_expr(node->rhs);
      cmp_zero(node->rhs->ty);
      println("  jne .L.true.%d", c);
      println("  mov $0, %%rax");
      println("  jmp .L.end.%d", c);
      println(".L.true.%d:", c);
      println("  mov $1, %%rax");
      println(".L.end.%d:", c);
      return;
    }
    case ND_FUNCALL: {
      if (node->lhs->kind == ND_VAR &&
          !strcmp(node->lhs->var->name, "alloca")) {
        gen_expr(node->args);
        println("  mov %%rax, %%rdi");
        builtin_alloca();
        return;
      }

      int stack_args = push_args(node);
      gen_expr(node->lhs);

      int gp = 0, fp = 0;

      // If the return type is a large struct/union, the caller passes
      // a pointer to a buffer as if it were the first argument.
      if (node->ret_buffer && node->ty->size > 16) pop(argreg64[gp++]);

      for (Node *arg = node->args; arg; arg = arg->next) {
        Type *ty = arg->ty;

        switch (ty->kind) {
          case TY_STRUCT:
          case TY_UNION:
            if (ty->size > 16) continue;

            bool fp1 = has_flonum1(ty);
            bool fp2 = has_flonum2(ty);

            if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
              if (fp1)
                popf(fp++);
              else
                pop(argreg64[gp++]);

              if (ty->size > 8) {
                if (fp2)
                  popf(fp++);
                else
                  pop(argreg64[gp++]);
              }
            }
            break;
          case TY_FLOAT:
          case TY_DOUBLE:
            if (fp < FP_MAX) popf(fp++);
            break;
          case TY_LDOUBLE:
            break;
          default:
            if (gp < GP_MAX) pop(argreg64[gp++]);
        }
      }

      println("  mov %%rax, %%r10");
      println("  mov $%d, %%eax", fp);
      println("  call *%%r10");
      println("  add $%d, %%rsp", stack_args * 8);

      depth -= stack_args;

      // It looks like the most significant 48 or 56 bits in RAX may
      // contain garbage if a function return type is short or bool/char,
      // respectively. We clear the upper bits here.
      switch (node->ty->kind) {
        case TY_BOOL:
          println("  movzbl %%al, %%eax");
          return;
        case TY_CHAR:
          if (node->ty->is_unsigned)
            println("  movzbl %%al, %%eax");
          else
            println("  movsbl %%al, %%eax");
          return;
        case TY_SHORT:
          if (node->ty->is_unsigned)
            println("  movzwl %%ax, %%eax");
          else
            println("  movswl %%ax, %%eax");
          return;
      }

      // If the return type is a small struct, a value is returned
      // using up to two registers.
      if (node->ret_buffer && node->ty->size <= 16) {
        copy_ret_buffer(node->ret_buffer);
        println("  lea %d(%%rbp), %%rax", node->ret_buffer->offset);
      }

      return;
    }
    case ND_LABEL_VAL:
      println("  lea %s(%%rip), %%rax", node->unique_label);
      return;
    case ND_CAS: {
      gen_expr(node->cas_addr);
      push();
      gen_expr(node->cas_new);
      push();
      gen_expr(node->cas_old);
      println("  mov %%rax, %%r8");
      load(node->cas_old->ty->base);
      pop("%rdx");  // new
      pop("%rdi");  // addr

      int sz = node->cas_addr->ty->base->size;
      println("  lock cmpxchg %s, (%%rdi)", reg_dx(sz));
      println("  sete %%cl");
      println("  je 1f");
      println("  mov %s, (%%r8)", reg_ax(sz));
      println("1:");
      println("  movzbl %%cl, %%eax");
      return;
    }
    case ND_EXCH: {
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");

      int sz = node->lhs->ty->base->size;
      println("  xchg %s, (%%rdi)", reg_ax(sz));
      return;
    }
  }

  switch (node->lhs->ty->kind) {
    case TY_FLOAT:
    case TY_DOUBLE: {
      gen_expr(node->rhs);
      pushf();
      gen_expr(node->lhs);
      popf(1);

      char *sz = (node->lhs->ty->kind == TY_FLOAT) ? "ss" : "sd";

      switch (node->kind) {
        case ND_ADD:
          println("  add%s %%xmm1, %%xmm0", sz);
          return;
        case ND_SUB:
          println("  sub%s %%xmm1, %%xmm0", sz);
          return;
        case ND_MUL:
          println("  mul%s %%xmm1, %%xmm0", sz);
          return;
        case ND_DIV:
          println("  div%s %%xmm1, %%xmm0", sz);
          return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
          println("  ucomi%s %%xmm0, %%xmm1", sz);

          if (node->kind == ND_EQ) {
            println("  sete %%al");
            println("  setnp %%dl");
            println("  and %%dl, %%al");
          } else if (node->kind == ND_NE) {
            println("  setne %%al");
            println("  setp %%dl");
            println("  or %%dl, %%al");
          } else if (node->kind == ND_LT) {
            println("  seta %%al");
          } else {
            println("  setae %%al");
          }

          println("  and $1, %%al");
          println("  movzb %%al, %%rax");
          return;
      }

      error_tok(node->tok, "invalid expression");
    }
    case TY_LDOUBLE: {
      gen_expr(node->lhs);
      gen_expr(node->rhs);

      switch (node->kind) {
        case ND_ADD:
          println("  faddp");
          return;
        case ND_SUB:
          println("  fsubrp");
          return;
        case ND_MUL:
          println("  fmulp");
          return;
        case ND_DIV:
          println("  fdivrp");
          return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
          println("  fcomip");
          println("  fstp %%st(0)");

          if (node->kind == ND_EQ)
            println("  sete %%al");
          else if (node->kind == ND_NE)
            println("  setne %%al");
          else if (node->kind == ND_LT)
            println("  seta %%al");
          else
            println("  setae %%al");

          println("  movzb %%al, %%rax");
          return;
      }

      error_tok(node->tok, "invalid expression");
    }
  }

  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  char *ax, *di, *dx;

  if (node->lhs->ty->kind == TY_LONG || node->lhs->ty->base) {
    ax = "%rax";
    di = "%rdi";
    dx = "%rdx";
  } else {
    ax = "%eax";
    di = "%edi";
    dx = "%edx";
  }

  switch (node->kind) {
    case ND_ADD:
      println("  add %s, %s", di, ax);
      return;
    case ND_SUB:
      println("  sub %s, %s", di, ax);
      return;
    case ND_MUL:
      println("  imul %s, %s", di, ax);
      return;
    case ND_DIV:
    case ND_MOD:
      if (node->ty->is_unsigned) {
        println("  mov $0, %s", dx);
        println("  div %s", di);
      } else {
        if (node->lhs->ty->size == 8)
          println("  cqo");
        else
          println("  cdq");
        println("  idiv %s", di);
      }

      if (node->kind == ND_MOD) println("  mov %%rdx, %%rax");
      return;
    case ND_BITAND:
      println("  and %s, %s", di, ax);
      return;
    case ND_BITOR:
      println("  or %s, %s", di, ax);
      return;
    case ND_BITXOR:
      println("  xor %s, %s", di, ax);
      return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      println("  cmp %s, %s", di, ax);

      if (node->kind == ND_EQ) {
        println("  sete %%al");
      } else if (node->kind == ND_NE) {
        println("  setne %%al");
      } else if (node->kind == ND_LT) {
        if (node->lhs->ty->is_unsigned)
          println("  setb %%al");
        else
          println("  setl %%al");
      } else if (node->kind == ND_LE) {
        if (node->lhs->ty->is_unsigned)
          println("  setbe %%al");
        else
          println("  setle %%al");
      }

      println("  movzb %%al, %%rax");
      return;
    case ND_SHL:
      println("  mov %%rdi, %%rcx");
      println("  shl %%cl, %s", ax);
      return;
    case ND_SHR:
      println("  mov %%rdi, %%rcx");
      if (node->lhs->ty->is_unsigned)
        println("  shr %%cl, %s", ax);
      else
        println("  sar %%cl, %s", ax);
      return;
  }

  error_tok(node->tok, "invalid expression");
}

static void gen_stmt(Node *node) {
  println("  .loc %d %d", node->tok->file->file_no, node->tok->line_no);

  switch (node->kind) {
    case ND_IF: {
      int c = count();
      gen_expr(node->cond);
      cmp_zero(node->cond->ty);
      println("  je  .L.else.%d", c);
      gen_stmt(node->then);
      println("  jmp .L.end.%d", c);
      println(".L.else.%d:", c);
      if (node->els) gen_stmt(node->els);
      println(".L.end.%d:", c);
      return;
    }
    case ND_FOR: {
      int c = count();
      if (node->init) gen_stmt(node->init);
      println(".L.begin.%d:", c);
      if (node->cond) {
        gen_expr(node->cond);
        cmp_zero(node->cond->ty);
        println("  je %s", node->brk_label);
      }
      gen_stmt(node->then);
      println("%s:", node->cont_label);
      if (node->inc) gen_expr(node->inc);
      println("  jmp .L.begin.%d", c);
      println("%s:", node->brk_label);
      return;
    }
    case ND_DO: {
      int c = count();
      println(".L.begin.%d:", c);
      gen_stmt(node->then);
      println("%s:", node->cont_label);
      gen_expr(node->cond);
      cmp_zero(node->cond->ty);
      println("  jne .L.begin.%d", c);
      println("%s:", node->brk_label);
      return;
    }
    case ND_SWITCH:
      gen_expr(node->cond);

      for (Node *n = node->case_next; n; n = n->case_next) {
        char *ax = (node->cond->ty->size == 8) ? "%rax" : "%eax";
        char *di = (node->cond->ty->size == 8) ? "%rdi" : "%edi";

        if (n->begin == n->end) {
          println("  cmp $%ld, %s", n->begin, ax);
          println("  je %s", n->label);
          continue;
        }

        // [GNU] Case ranges
        println("  mov %s, %s", ax, di);
        println("  sub $%ld, %s", n->begin, di);
        println("  cmp $%ld, %s", n->end - n->begin, di);
        println("  jbe %s", n->label);
      }

      if (node->default_case) println("  jmp %s", node->default_case->label);

      println("  jmp %s", node->brk_label);
      gen_stmt(node->then);
      println("%s:", node->brk_label);
      return;
    case ND_CASE:
      println("%s:", node->label);
      gen_stmt(node->lhs);
      return;
    case ND_BLOCK:
      for (Node *n = node->body; n; n = n->next) gen_stmt(n);
      return;
    case ND_GOTO:
      println("  jmp %s", node->unique_label);
      return;
    case ND_GOTO_EXPR:
      gen_expr(node->lhs);
      println("  jmp *%%rax");
      return;
    case ND_LABEL:
      println("%s:", node->unique_label);
      gen_stmt(node->lhs);
      return;
    case ND_RETURN:
      if (node->lhs) {
        gen_expr(node->lhs);
        Type *ty = node->lhs->ty;

        switch (ty->kind) {
          case TY_STRUCT:
          case TY_UNION:
            if (ty->size <= 16)
              copy_struct_reg();
            else
              copy_struct_mem();
            break;
        }
      }

      println("  jmp .L.return.%s", current_fn->name);
      return;
    case ND_EXPR_STMT:
      gen_expr(node->lhs);
      return;
    case ND_ASM:
      println("  %s", node->asm_str);
      return;
  }

  error_tok(node->tok, "invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function) continue;

    // If a function has many parameters, some parameters are
    // inevitably passed by stack rather than by register.
    // The first passed-by-stack parameter resides at RBP+16.
    int top = 16;
    int bottom = 0;

    int gp = 0, fp = 0;

    // Assign offsets to pass-by-stack parameters.
    for (Obj *var = fn->params; var; var = var->next) {
      Type *ty = var->ty;

      switch (ty->kind) {
        case TY_STRUCT:
        case TY_UNION:
          if (ty->size <= 16) {
            bool fp1 = has_flonum(ty, 0, 8, 0);
            bool fp2 = has_flonum(ty, 8, 16, 8);
            if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
              fp = fp + fp1 + fp2;
              gp = gp + !fp1 + !fp2;
              continue;
            }
          }
          break;
        case TY_FLOAT:
        case TY_DOUBLE:
          if (fp++ < FP_MAX) continue;
          break;
        case TY_LDOUBLE:
          break;
        default:
          if (gp++ < GP_MAX) continue;
      }

      top = align_to(top, 8);
      var->offset = top;
      top += var->ty->size;
    }

    // Assign offsets to pass-by-register parameters and local variables.
    for (Obj *var = fn->locals; var; var = var->next) {
      if (var->offset) continue;

      // AMD64 System V ABI has a special alignment rule for an array of
      // length at least 16 bytes. We need to align such array to at least
      // 16-byte boundaries. See p.14 of
      // https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-draft.pdf.
      int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)
                      ? MAX(16, var->align)
                      : var->align;

      bottom += var->ty->size;
      bottom = align_to(bottom, align);
      var->offset = -bottom;
    }

    fn->stack_size = align_to(bottom, 16);
  }
}

static void emit_data(Obj *prog) {
  for (Obj *var = prog; var; var = var->next) {
    if (var->is_function || !var->is_definition) continue;

    if (var->is_static)
      println("  .local %s", var->name);
    else
      println("  .globl %s", var->name);

    int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)
                    ? MAX(16, var->align)
                    : var->align;

    // Common symbol
    if (opt_fcommon && var->is_tentative && !var->is_tls) {
      println("  .comm %s, %d, %d", var->name, var->ty->size, align);
      continue;
    }

    // .data or .tdata
    if (var->init_data) {
      if (var->is_tls)
        println("  .section .tdata,\"awT\",@progbits");
      else
        println("  .data");

      println("  .type %s, @object", var->name);
      println("  .size %s, %d", var->name, var->ty->size);
      println("  .align %d", align);
      println("%s:", var->name);

      Relocation *rel = var->rel;
      int pos = 0;
      while (pos < var->ty->size) {
        if (rel && rel->offset == pos) {
          println("  .quad %s%+ld", *rel->label, rel->addend);
          rel = rel->next;
          pos += 8;
        } else {
          println("  .byte %d", var->init_data[pos++]);
        }
      }
      continue;
    }

    // .bss or .tbss
    if (var->is_tls)
      println("  .section .tbss,\"awT\",@nobits");
    else
      println("  .bss");

    println("  .align %d", align);
    println("%s:", var->name);
    println("  .zero %d", var->ty->size);
  }
}

static void store_fp(int r, int offset, int sz) {
  switch (sz) {
    case 4:
      println("  movss %%xmm%d, %d(%%rbp)", r, offset);
      return;
    case 8:
      println("  movsd %%xmm%d, %d(%%rbp)", r, offset);
      return;
  }
  UNREACHABLE();
}

static void store_gp(int r, int offset, int sz) {
  switch (sz) {
    case 1:
      println("  mov %s, %d(%%rbp)", argreg8[r], offset);
      return;
    case 2:
      println("  mov %s, %d(%%rbp)", argreg16[r], offset);
      return;
    case 4:
      println("  mov %s, %d(%%rbp)", argreg32[r], offset);
      return;
    case 8:
      println("  mov %s, %d(%%rbp)", argreg64[r], offset);
      return;
    default:
      for (int i = 0; i < sz; i++) {
        println("  mov %s, %d(%%rbp)", argreg8[r], offset + i);
        println("  shr $8, %s", argreg64[r]);
      }
      return;
  }
}

static void emit_text(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function || !fn->is_definition) continue;

    // No code is emitted for "static inline" functions
    // if no one is referencing them.
    if (!fn->is_live) continue;

    if (fn->is_static)
      println("  .local %s", fn->name);
    else
      println("  .globl %s", fn->name);

    println("  .text");
    println("  .type %s, @function", fn->name);
    println("%s:", fn->name);
    current_fn = fn;

    // Prologue
    println("  push %%rbp");
    println("  mov %%rsp, %%rbp");
    println("  sub $%d, %%rsp", fn->stack_size);
    println("  mov %%rsp, %d(%%rbp)", fn->alloca_bottom->offset);

    // Save arg registers if function is variadic
    if (fn->va_area) {
      int gp = 0, fp = 0;
      for (Obj *var = fn->params; var; var = var->next) {
        if (is_flonum(var->ty))
          fp++;
        else
          gp++;
      }

      int off = fn->va_area->offset;

      // va_elem
      println("  movl $%d, %d(%%rbp)", gp * 8, off);           // gp_offset
      println("  movl $%d, %d(%%rbp)", fp * 8 + 48, off + 4);  // fp_offset
      println("  movq %%rbp, %d(%%rbp)", off + 8);  // overflow_arg_area
      println("  addq $16, %d(%%rbp)", off + 8);
      println("  movq %%rbp, %d(%%rbp)", off + 16);  // reg_save_area
      println("  addq $%d, %d(%%rbp)", off + 24, off + 16);

      // __reg_save_area__
      println("  movq %%rdi, %d(%%rbp)", off + 24);
      println("  movq %%rsi, %d(%%rbp)", off + 32);
      println("  movq %%rdx, %d(%%rbp)", off + 40);
      println("  movq %%rcx, %d(%%rbp)", off + 48);
      println("  movq %%r8, %d(%%rbp)", off + 56);
      println("  movq %%r9, %d(%%rbp)", off + 64);
      println("  movsd %%xmm0, %d(%%rbp)", off + 72);
      println("  movsd %%xmm1, %d(%%rbp)", off + 80);
      println("  movsd %%xmm2, %d(%%rbp)", off + 88);
      println("  movsd %%xmm3, %d(%%rbp)", off + 96);
      println("  movsd %%xmm4, %d(%%rbp)", off + 104);
      println("  movsd %%xmm5, %d(%%rbp)", off + 112);
      println("  movsd %%xmm6, %d(%%rbp)", off + 120);
      println("  movsd %%xmm7, %d(%%rbp)", off + 128);
    }

    // Save passed-by-register arguments to the stack
    int gp = 0, fp = 0;
    for (Obj *var = fn->params; var; var = var->next) {
      if (var->offset > 0) continue;

      Type *ty = var->ty;

      switch (ty->kind) {
        case TY_STRUCT:
        case TY_UNION:
          assert(ty->size <= 16);
          if (has_flonum(ty, 0, 8, 0))
            store_fp(fp++, var->offset, MIN(8, ty->size));
          else
            store_gp(gp++, var->offset, MIN(8, ty->size));

          if (ty->size > 8) {
            if (has_flonum(ty, 8, 16, 0))
              store_fp(fp++, var->offset + 8, ty->size - 8);
            else
              store_gp(gp++, var->offset + 8, ty->size - 8);
          }
          break;
        case TY_FLOAT:
        case TY_DOUBLE:
          store_fp(fp++, var->offset, ty->size);
          break;
        default:
          store_gp(gp++, var->offset, ty->size);
      }
    }

    // Emit code
    gen_stmt(fn->body);
    assert(depth == 0);

    // The C spec defines a special rule for the main function.
    // Reaching the end of the main function is equivalent to
    // returning 0, even though the behavior is undefined for the
    // other functions. See C11 5.1.2.2.3.
    if (strcmp(fn->name, "main") == 0) println("  mov $0, %%rax");

    // Epilogue
    println(".L.return.%s:", fn->name);
    println("  mov %%rbp, %%rsp");
    println("  pop %%rbp");
    println("  ret");
  }
}

static void emit_staticasms(StaticAsm *a) {
  if (!a) return;
  emit_staticasms(a->next);
  println("  .loc %d %d", a->body->tok->file->file_no, a->body->tok->line_no);
  println("  %s", a->body->asm_str);
}

void codegen(Obj *prog, FILE *out) {
  output_file = out;

  File **files = get_input_files();
  for (int i = 0; files[i]; i++)
    println("  .file %d \"%s\"", files[i]->file_no, files[i]->name);

  assign_lvar_offsets(prog);
  emit_staticasms(staticasms);
  emit_data(prog);
  emit_text(prog);
}
