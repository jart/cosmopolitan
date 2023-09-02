#include "libc/x/xasprintf.h"
#include "third_party/chibicc/chibicc.h"

#define GP_MAX 6
#define FP_MAX 8

int depth;
static char argreg8[][5] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char argreg16[][5] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char argreg32[][5] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char argreg64[][5] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;
static char *lastline;

FILE *output_stream;

void flushln(void) {
  if (!lastline) return;
  fputs(lastline, output_stream);
  fputc('\n', output_stream);
  free(lastline);
  lastline = NULL;
}

static void processln(char *nextline) {
#define LASTEQUAL(S) (lastlen == strlen(S) && !memcmp(lastline, S, lastlen))
  size_t lastlen;
  if (lastline) {
    lastlen = strlen(lastline);
    // unsophisticated optimization pass to reduce asm noise a little bit
    if ((LASTEQUAL("\txor\t%eax,%eax") && !strcmp(nextline, "\tcltq")) ||
        (LASTEQUAL("\tmov\t$0x1,%eax") && !strcmp(nextline, "\tcltq")) ||
        (LASTEQUAL("\tmovslq\t(%rax),%rax") && !strcmp(nextline, "\tcltq"))) {
      free(nextline);
    } else if (LASTEQUAL("\tmov\t(%rax),%rax") &&
               !strcmp(nextline, "\tpush\t%rax")) {
      free(lastline);
      free(nextline);
      lastline = strdup("\tpush\t(%rax)");
    } else if (LASTEQUAL("\tmov\t$0x1,%eax") &&
               !strcmp(nextline, "\tpush\t%rax")) {
      free(lastline);
      free(nextline);
      lastline = strdup("\tpush\t$1");
    } else if (LASTEQUAL("\tpush\t(%rax)") &&
               !strcmp(nextline, "\tpop\t%rdi")) {
      free(lastline);
      free(nextline);
      lastline = strdup("\tmov\t(%rax),%rdi");
    } else if (LASTEQUAL("\tpush\t%rax") && !strcmp(nextline, "\tpop\t%rdi")) {
      free(lastline);
      free(nextline);
      lastline = strdup("\tmov\t%rax,%rdi");
    } else {
      flushln();
      lastline = nextline;
    }
  } else {
    lastline = nextline;
  }
#undef LASTEQUAL
}

static void emitlin(char *nextline) {
  processln(strdup(nextline));
}

void println(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  emitlin(xvasprintf(fmt, ap));
  va_end(ap);
}

int count(void) {
  static int i = 1;
  return i++;
}

void push(void) {
  emitlin("\tpush\t%rax");
  depth++;
}

void pop(char *arg) {
  println("\tpop\t%s", arg);
  depth--;
  fflush(output_stream);
  DCHECK_GE(depth, 0);
}

void push2(void) {
  emitlin("\tpush\t%rdx");
  emitlin("\tpush\t%rax");
  depth++;
  depth++;
}

void pop2(char *a, char *b) {
  println("\tpop\t%s", a);
  println("\tpop\t%s", b);
  depth--;
  depth--;
  DCHECK_GE(depth, 0);
}

void pushreg(const char *arg) {
  println("\tpush\t%%%s", arg);
  depth++;
}

void popreg(const char *arg) {
  println("\tpop\t%%%s", arg);
  depth--;
  DCHECK_GE(depth, 0);
}

static const char *nameof(Obj *obj) {
  if (obj->asmname) {
    return obj->asmname;
  } else {
    return obj->name;
  }
}

static void pushx(void) {
  emitlin("\tsub\t$16,%rsp");
  emitlin("\tmovdqu\t%xmm0,(%rsp)");
  depth += 2;
}

static void popx(int reg) {
  println("\tmovdqu\t(%%rsp),%%xmm%d", reg);
  emitlin("\tadd\t$16,%rsp");
  depth -= 2;
  DCHECK_GE(depth, 0);
}

static void pushf(Type *ty) {
  if (ty->vector_size == 16) {
    pushx();
  } else {
    emitlin("\tpush\t%rax");
    emitlin("\tmovsd\t%xmm0,(%rsp)");
    depth++;
  }
}

static void popf(Type *ty, int reg) {
  if (ty->vector_size == 16) {
    popx(reg);
  } else {
    println("\tmovsd\t(%%rsp),%%xmm%d", reg);
    emitlin("\tadd\t$8,%rsp");
    depth--;
  }
}

static void print_profiling_nop(void) {
  emitlin("\t.byte\t0x0f,0x1f,0x44,0x00,0x00");
}

static void print_visibility(Obj *obj) {
  if (obj->visibility) {
    if (!strcmp(obj->visibility, "hidden")) {
      println("\t.hidden\t%s", nameof(obj));
    } else if (!strcmp(obj->visibility, "protected")) {
      println("\t.protected %s", nameof(obj));
    } else {
      println("\t.globl\t%s", nameof(obj));
    }
  } else if (obj->is_static) {
    println("\t.local\t%s", nameof(obj));
  } else {
    println("\t.globl\t%s", nameof(obj));
  }
  if (obj->is_weak) {
    println("\t.weak\t%s", nameof(obj));
  }
}

static void print_align(int align) {
  if (align > 1) println("\t.align\t%d", align);
}

void print_loc(int64_t file, int64_t line) {
  // TODO: This is broken if file is different? See gperf codegen.
  return;
  static int64_t lastfile = -1;
  static int64_t lastline = -1;
  char *locbuf, *p;
  if (file != lastfile || line != lastline) {
    locbuf = malloc(2 + 4 + 1 + 20 + 1 + 20 + 1);
    p = stpcpy(locbuf, "\t.loc\t");
    p = FormatInt64(p, file);
    *p++ = ' ';
    FormatInt64(p, line);
    emitlin(locbuf);
    free(locbuf);
    lastfile = file;
    lastline = line;
  }
}

static void print_mov_imm(int64_t val, const char *reg64, const char *reg32) {
  if (val) {
    if (val != -1) {
      if (0 < val && val <= INT32_MAX) {
        println("\tmov\t$%#lx,%s", val, reg32);
      } else if (INT32_MIN <= val && val < 0) {
        println("\tmov\t$%#lx,%s", (int64_t)(int32_t)val, reg64);
      } else {
        println("\tmov\t$%#lx,%s", val, reg64);
      }
    } else {
      println("\tor\t$-1,%s", reg64);
    }
  } else {
    println("\txor\t%s,%s", reg32, reg32);
  }
}

static bool is_quotable_string(const char *s, size_t n) {
  size_t i;
  if (!n) return false;
  if (s[n - 1]) return false;
  for (i = 0; i < n - 1; ++i) {
    if (s[i] == '"') return false;
    if (s[i] == '\\') return false;
    if (s[i] < 0x20 || s[i] >= 0x7f) {
      return false;
    }
  }
  return true;
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

static char *reg_r8(int sz) {
  switch (sz) {
    case 1:
      return "%r8b";
    case 2:
      return "%r8w";
    case 4:
      return "%r8d";
    case 8:
      return "%r8";
  }
  UNREACHABLE();
}

static char *reg_di(int sz) {
  switch (sz) {
    case 1:
      return "%dil";
    case 2:
      return "%di";
    case 4:
      return "%edi";
    case 8:
      return "%rdi";
  }
  UNREACHABLE();
}

static char *reg_si(int sz) {
  switch (sz) {
    case 1:
      return "%sil";
    case 2:
      return "%si";
    case 4:
      return "%esi";
    case 8:
      return "%rsi";
  }
  UNREACHABLE();
}

static const char *gotpcrel(void) {
  if (opt_pic) {
    return "@gotpcrel(%rip)";
  } else {
    return "";
  }
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
// asm() wants this to not clobber flags or regs other than rax.
void gen_addr(Node *node) {
  switch (node->kind) {
    case ND_VAR:
      // Variable-length array, which is always local.
      if (node->var->ty->kind == TY_VLA) {
        println("\tmov\t%d(%%rbp),%%rax", node->var->offset);
        return;
      }
      // Local variable
      if (node->var->is_local) {
        println("\tlea\t%d(%%rbp),%%rax", node->var->offset);
        return;
      }
      if (opt_pic) {
        if (node->var->is_tls) {
          // Dynamic thread-local variable
          println("\tmov\t%%fs:0,%%rax");
          println("\tmov\t%s@dtpoff(%%rax),%%rax", nameof(node->var));
          return;
        }
        // Function or global variable
        println("\tmov\t%s@gotpcrel(%%rip),%%rax", nameof(node->var));
        return;
      }
      // Static thread-local variable
      if (node->var->is_tls) {
        println("\tmov\t%%fs:%s@tpoff,%%rax", nameof(node->var));
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
        if (!opt_pic) {
          println("\tmov\t$%s,%%eax", nameof(node->var));
        } else if (node->var->is_definition) {
          println("\tlea\t%s(%%rip),%%rax", nameof(node->var));
        } else {
          println("\tmov\t%s@gotpcrel(%%rip),%%rax", nameof(node->var));
        }
        return;
      }
      // Global variable
      if (opt_pic) {
        println("\tlea\t%s(%%rip),%%rax", nameof(node->var));
      } else {
        println("\tmov\t$%s,%%eax", nameof(node->var));
      }
      return;
    case ND_CAST:
      gen_expr(node->lhs);
      gen_cast(node->lhs->ty, node->ty);
      return;
    case ND_DEREF:
      gen_expr(node->lhs);
      return;
    case ND_COMMA:
      gen_expr(node->lhs);
      gen_addr(node->rhs);
      return;
    case ND_ASSIGN:
    case ND_COND:
      if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION) {
        gen_expr(node);
        return;
      }
      break;
    case ND_MEMBER:
      gen_addr(node->lhs);
      if (node->member->offset) {
        println("\tlea\t%d(%%rax),%%rax", node->member->offset);
      }
      return;
    case ND_FUNCALL:
      if (node->ret_buffer) {
        gen_expr(node);
        return;
      }
      break;
    case ND_VLA_PTR:
      println("\tlea\t%d(%%rbp),%%rax", node->var->offset);
      return;
    default:
      error_tok(node->tok, "not an lvalue %d", node->kind);
  }
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
      if (ty->vector_size == 16) {
        if (ty->align >= 16) {
          emitlin("\tmovaps\t(%rax),%xmm0");
        } else {
          emitlin("\tmovdqu\t(%rax),%xmm0");
        }
      } else {
        emitlin("\tmovss\t(%rax),%xmm0");
        return;
      }
    case TY_DOUBLE:
      if (ty->vector_size == 16) {
        if (ty->align >= 16) {
          emitlin("\tmovapd\t(%rax),%xmm0");
        } else {
          emitlin("\tmovdqu\t(%rax),%xmm0");
        }
      } else {
        emitlin("\tmovsd\t(%rax),%xmm0");
      }
      return;
    case TY_LDOUBLE:
      emitlin("\tfldt\t(%rax)");
      return;
    case TY_INT128:
      emitlin("\tmov\t8(%rax),%rdx");
      emitlin("\tmov\t(%rax),%rax");
      return;
    default:
      break;
  }
  char *insn = ty->is_unsigned ? "movz" : "movs";
  // When we load a char or a short value to a register, we always
  // extend them to the size of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  if (ty->size == 1) {
    println("\t%sbl\t(%%rax),%%eax", insn);
  } else if (ty->size == 2) {
    println("\t%swl\t(%%rax),%%eax", insn);
  } else if (ty->size == 4) {
    emitlin("\tmovslq\t(%rax),%rax");
  } else {
    emitlin("\tmov\t(%rax),%rax");
  }
}

static void gen_memcpy(size_t size) {
  switch (size) {
    case 0:
      break;
    case 1:
      emitlin("\tmov\t(%rax),%r8b");
      emitlin("\tmov\t%r8b,(%rdi)");
      break;
    case 2:
      emitlin("\tmov\t(%rax),%r8w");
      emitlin("\tmov\t%r8w,(%rdi)");
      break;
    case 3:
      emitlin("\tmov\t(%rax),%r8w");
      emitlin("\tmov\t%r8w,(%rdi)");
      emitlin("\tmov\t2(%rax),%r8b");
      emitlin("\tmov\t%r8b,2(%rdi)");
      break;
    case 4:
      emitlin("\tmov\t(%rax),%r8d");
      emitlin("\tmov\t%r8d,(%rdi)");
      break;
    case 5 ... 7:
      emitlin("\tmov\t(%rax),%r8d");
      println("\tmov\t%d(%%rax),%%r9d", size - 4);
      emitlin("\tmov\t%r8d,(%rdi)");
      println("\tmov\t%%r9d,%d(%%rdi)", size - 4);
      break;
    case 8:
      emitlin("\tmov\t(%rax),%r8");
      emitlin("\tmov\t%r8,(%rdi)");
      break;
    case 9 ... 15:
      emitlin("\tmov\t(%rax),%r8");
      println("\tmov\t%d(%%rax),%%r9", size - 8);
      emitlin("\tmov\t%r8,(%rdi)");
      println("\tmov\t%%r9,%d(%%rdi)", size - 8);
      break;
    case 16:
      emitlin("\tmovdqu\t(%rax),%xmm2");
      emitlin("\tmovdqu\t%xmm2,(%rdi)");
      break;
    case 17 ... 32:
      emitlin("\tmovdqu\t(%rax),%xmm2");
      println("\tmovdqu\t%d(%%rax),%%xmm3", size - 16);
      emitlin("\tmovdqu\t%xmm2,(%rdi)");
      println("\tmovdqu\t%%xmm3,%d(%%rdi)", size - 16);
      break;
    default:
      emitlin("\tmov\t%rax,%rsi");
      print_mov_imm(size, "%rcx", "%ecx");
      emitlin("\trep movsb");
      break;
  }
}

// Store %rax to an address that the stack top is pointing to.
static void store(Type *ty) {
  pop("%rdi");
  switch (ty->kind) {
    case TY_STRUCT:
    case TY_UNION:
      gen_memcpy(ty->size);
      return;
    case TY_FLOAT:
      if (ty->vector_size == 16) {
        if (ty->align >= 16) {
          emitlin("\tmovaps\t%xmm0,(%rdi)");
        } else {
          emitlin("\tmovdqu\t%xmm0,(%rdi)");
        }
      } else {
        emitlin("\tmovss\t%xmm0,(%rdi)");
      }
      return;
    case TY_DOUBLE:
      if (ty->vector_size == 16) {
        if (ty->align >= 16) {
          emitlin("\tmovapd\t%xmm0,(%rdi)");
        } else {
          emitlin("\tmovdqu\t%xmm0,(%rdi)");
        }
      } else {
        emitlin("\tmovsd\t%xmm0,(%rdi)");
      }
      return;
    case TY_LDOUBLE:
      emitlin("\tfstpt\t(%rdi)");
      return;
    case TY_INT128:
      emitlin("\tmov\t%rax,(%rdi)");
      emitlin("\tmov\t%rdx,8(%rdi)");
      return;
  }
  if (ty->vector_size == 16) {
    if (ty->align >= 16) {
      emitlin("\tmovdqa\t%xmm0,(%rdi)");
    } else {
      emitlin("\tmovdqu\t%xmm0,(%rdi)");
    }
  }
  if (ty->size == 1) {
    emitlin("\tmov\t%al,(%rdi)");
  } else if (ty->size == 2) {
    emitlin("\tmov\t%ax,(%rdi)");
  } else if (ty->size == 4) {
    emitlin("\tmov\t%eax,(%rdi)");
  } else {
    emitlin("\tmov\t%rax,(%rdi)");
  }
}

void cmp_zero(Type *ty) {
  switch (ty->kind) {
    case TY_FLOAT:
      emitlin("\txorps\t%xmm1,%xmm1");
      emitlin("\tucomiss\t%xmm1,%xmm0");
      return;
    case TY_DOUBLE:
      emitlin("\txorpd\t%xmm1,%xmm1");
      emitlin("\tucomisd\t%xmm1,%xmm0");
      return;
    case TY_LDOUBLE:
      emitlin("\tfldz");
      emitlin("\tfucomip");
      emitlin("\tfstp\t%st");
      return;
    case TY_INT128:
      emitlin("\tmov\t%rax,%r11");
      emitlin("\tor\t%rdx,%r11");
      return;
  }
  if (is_integer(ty) && ty->size <= 4) {
    emitlin("\ttest\t%eax,%eax");
  } else {
    emitlin("\ttest\t%rax,%rax");
  }
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
    for (Member *mem = ty->members; mem; mem = mem->next) {
      if (!has_flonum(mem->ty, lo, hi, offset + mem->offset)) {
        return false;
      }
    }
    return true;
  }
  if (ty->kind == TY_ARRAY) {
    for (int i = 0; i < ty->array_len; i++) {
      if (!has_flonum(ty->base, lo, hi, offset + ty->base->size * i)) {
        return false;
      }
    }
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
  int sz = ROUNDUP(ty->size, 8);
  println("\tsub\t$%d,%%rsp", sz);
  println("\tmov\t%%rsp,%%rdi");
  depth += sz / 8;
  gen_memcpy(ty->size);
}

static void push_args2(Node *args, bool first_pass) {
  if (!args) return;
  push_args2(args->next, first_pass);
  if ((first_pass && !args->pass_by_stack) ||
      (!first_pass && args->pass_by_stack)) {
    return;
  }
  gen_expr(args);
  switch (args->ty->kind) {
    case TY_STRUCT:
    case TY_UNION:
      push_struct(args->ty);
      break;
    case TY_FLOAT:
    case TY_DOUBLE:
      pushf(args->ty);
      break;
    case TY_LDOUBLE:
      emitlin("\tsub\t$16,%rsp");
      emitlin("\tfstpt\t(%rsp)");
      depth += 2;
      break;
    case TY_INT128:
      push2();
      break;
    default:
      push();
  }
  if (args->realign_stack) {
    pushreg("rbx");
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
          stack += ROUNDUP(ty->size, 8) / 8;
        } else {
          bool fp1 = has_flonum1(ty);
          bool fp2 = has_flonum2(ty);
          if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
            fp = fp + fp1 + fp2;
            gp = gp + !fp1 + !fp2;
          } else {
            arg->pass_by_stack = true;
            stack += ROUNDUP(ty->size, 8) / 8;
          }
        }
        break;
      case TY_FLOAT:
      case TY_DOUBLE:
        if (fp++ >= FP_MAX) {
          if ((stack & 1) && arg->ty->vector_size == 16) {
            arg->realign_stack = true;
            ++stack;
          }
          arg->pass_by_stack = true;
          ++stack;
        }
        break;
      case TY_LDOUBLE:
        arg->pass_by_stack = true;
        stack += 2;
        break;
      case TY_INT128:
        if (gp + 1 >= GP_MAX) {
          if (stack & 1) {
            arg->realign_stack = true;
            ++stack;
          }
          arg->pass_by_stack = true;
          stack += 2;
        } else {
          gp += 2;
        }
        break;
      default:
        if (gp++ >= GP_MAX) {
          arg->pass_by_stack = true;
          stack++;
        }
        break;
    }
  }
  if ((depth + stack) % 2 == 1) {
    emitlin("\tsub\t$8,%rsp");
    depth++;
    stack++;
  }
  push_args2(node->args, true);
  push_args2(node->args, false);
  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (node->ret_buffer && node->ty->size > 16) {
    println("\tlea\t%d(%%rbp),%%rax", node->ret_buffer->offset);
    push();
  }
  return stack;
}

static void copy_ret_buffer(Obj *var) {
  /* todo(jart): wat */
  Type *ty = var->ty;
  int gp = 0, fp = 0;
  if (has_flonum1(ty)) {
    assert(ty->size == 4 || 8 <= ty->size);
    if (ty->size == 4) {
      println("\tmovss\t%%xmm0,%d(%%rbp)", var->offset);
    } else {
      println("\tmovsd\t%%xmm0,%d(%%rbp)", var->offset);
    }
    fp++;
  } else {
    for (int i = 0; i < MIN(8, ty->size); i++) {
      println("\tmov\t%%al,%d(%%rbp)", var->offset + i);
      emitlin("\tshr\t$8,%rax");
    }
    gp++;
  }
  if (ty->size > 8) {
    if (has_flonum2(ty)) {
      assert(ty->size == 12 || ty->size == 16);
      if (ty->size == 12) {
        println("\tmovss\t%%xmm%d,%d(%%rbp)", fp, var->offset + 8);
      } else {
        println("\tmovsd\t%%xmm%d,%d(%%rbp)", fp, var->offset + 8);
      }
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx"; /* TODO: isn't ax clobbered? */
      for (int i = 8; i < MIN(16, ty->size); i++) {
        println("\tmov\t%s,%d(%%rbp)", reg1, var->offset + i);
        println("\tshr\t$8,%s", reg2);
      }
    }
  }
}

static void copy_struct_reg(void) {
  /* todo(jart): wat */
  Type *ty = current_fn->ty->return_ty;
  int gp = 0, fp = 0;
  emitlin("\tmov\t%rax,%rdi");
  if (has_flonum(ty, 0, 8, 0)) {
    assert(ty->size == 4 || 8 <= ty->size);
    if (ty->size == 4) {
      emitlin("\tmovss\t(%rdi),%xmm0");
    } else if (ty->size == 8) {
      emitlin("\tmovsd\t(%rdi),%xmm0");
    } else {
      emitlin("\tmovdqu\t(%rdi),%xmm0");
    }
    fp++;
  } else {
    emitlin("\txor\t%eax,%eax");
    for (int i = MIN(8, ty->size) - 1; i >= 0; i--) {
      emitlin("\tshl\t$8,%rax");
      println("\tmov\t%d(%%rdi),%%al", i);
    }
    gp++;
  }
  if (ty->size > 8) {
    if (has_flonum(ty, 8, 16, 0)) {
      assert(ty->size == 12 || ty->size == 16);
      if (ty->size == 4) {
        println("\tmovss\t8(%%rdi),%%xmm%d", fp);
      } else {
        println("\tmovsd\t8(%%rdi),%%xmm%d", fp);
      }
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";
      char *reg3 = (gp == 0) ? "%eax" : "%edx";
      println("\txor\t%s,%s", reg3, reg3);
      for (int i = MIN(16, ty->size) - 1; i >= 8; i--) {
        println("\tshl\t$8,%s", reg2);
        println("\tmov\t%d(%%rdi),%s", i, reg1);
      }
    }
  }
}

static void copy_struct_mem(void) {
  Type *ty = current_fn->ty->return_ty;
  Obj *var = current_fn->params;
  println("\tmov\t%d(%%rbp),%%rdi", var->offset);
  for (int i = 0; i < ty->size; i++) {
    println("\tmov\t%d(%%rax),%%dl", i);
    println("\tmov\t%%dl,%d(%%rdi)", i);
  }
}

static void builtin_alloca(void) {
  // Align size to 16 bytes.
  emitlin("\tadd\t$15,%rdi");
  emitlin("\tand\t$-16,%rdi");
  // Shift the temporary area by %rdi.
  println("\tmov\t%d(%%rbp),%%rcx", current_fn->alloca_bottom->offset);
  emitlin("\
\tsub\t%rsp,%rcx\n\
\tmov\t%rsp,%rax\n\
\tsub\t%rdi,%rsp\n\
\tmov\t%rsp,%rdx\n\
1:\n\
\ttest\t%rcx,%rcx\n\
\tje\t2f\n\
\tmov\t(%rax),%r8b\n\
\tmov\t%r8b,(%rdx)\n\
\tinc\t%rdx\n\
\tinc\t%rax\n\
\tdec\t%rcx\n\
\tjmp\t1b\n\
2:");
  // Move alloca_bottom pointer.
  println("\tmov\t%d(%%rbp),%%rax", current_fn->alloca_bottom->offset);
  emitlin("\tsub\t%rdi,%rax");
  println("\tmov\t%%rax,%d(%%rbp)", current_fn->alloca_bottom->offset);
}

static void gen_comis(Node *node, const char *op, int a, int b,
                      const char *pred) {
  if (a != b) {
    gen_expr(node->args->next);
    pushf(node->args->next->ty);
    gen_expr(node->args);
    popf(node->args->next->ty, 1);
  } else {
    gen_expr(node->args);
  }
  emitlin("\txor\t%eax,%eax");
  println("\t%s\t%%xmm%d,%%xmm%d", op, a, b);
  println("\tset%s\t%%al", pred);
}

static bool gen_builtin_funcall(Node *node, const char *name) {
  if (!strcmp(name, "alloca")) {
    gen_expr(node->args);
    emitlin("\tmov\t%rax,%rdi");
    builtin_alloca();
    return true;
  } else if (!strcmp(name, "trap")) {
    emitlin("\tint3");
    return true;
  } else if (!strcmp(name, "ia32_pause")) {
    emitlin("\tpause");
    return true;
  } else if (!strcmp(name, "unreachable")) {
    emitlin("\tud2");
    return true;
  } else if (!strcmp(name, "frame_address")) {
    if (is_const_expr(node->args) && !eval(node->args)) {
      emitlin("\tmov\t%rbp,%rax");
    } else {
      error_tok(node->args->tok, "must be 0");
    }
    return true;
  } else if (!strcmp(name, "ctz")) {
    gen_expr(node->args);
    emitlin("\tbsf\t%eax,%eax");
    return true;
  } else if (!strcmp(name, "ctzl") || !strcmp(name, "ctzll")) {
    gen_expr(node->args);
    emitlin("\tbsf\t%rax,%rax");
    return true;
  } else if (!strcmp(name, "clz")) {
    gen_expr(node->args);
    emitlin("\
\tbsr\t%eax,%eax\n\
\txor\t$31,%eax");
    return true;
  } else if (!strcmp(name, "clzl") || !strcmp(name, "clzll")) {
    gen_expr(node->args);
    emitlin("\
\tbsr\t%rax,%rax\n\
\txor\t$63,%eax");
    return true;
  } else if (!strcmp(name, "ffs") || !strcmp(name, "ffsl") ||
             !strcmp(name, "ffsll")) {
    char regprefix;
    gen_expr(node->args);
    emitlin("\tor\t$-1,%edi");
    regprefix = endswith(name, "l") ? 'r' : 'e';
    println("\tbsf\t%%%cax,%%%cax", regprefix, regprefix);
    emitlin("\tcmovz\t%edi,%eax");
    emitlin("\tinc\t%eax");
    return true;
  } else if (!strcmp(name, "bswap16")) {
    gen_expr(node->args);
    emitlin("\txchg\t%al,%ah");
    return true;
  } else if (!strcmp(name, "bswap32")) {
    gen_expr(node->args);
    emitlin("\tbswap\t%eax");
    return true;
  } else if (!strcmp(name, "bswap64")) {
    gen_expr(node->args);
    emitlin("\tbswap\t%rax");
    return true;
  } else if (!strcmp(name, "popcount")) {
    gen_expr(node->args);
    if (opt_popcnt) {
      emitlin("\tpopcnt\t%eax,%eax");
    } else {
      emitlin("\tmov\t%eax,%edi");
      emitlin("\tcall\t__popcountsi2");
    }
    return true;
  } else if (!strcmp(name, "popcountl") || !strcmp(name, "popcountll")) {
    gen_expr(node->args);
    if (opt_popcnt) {
      emitlin("\tpopcnt\t%rax,%rax");
    } else {
      emitlin("\tmov\t%rax,%rdi");
      emitlin("\tcall\t__popcountdi2");
    }
    return true;
  } else if (!strcmp(name, "memcpy")) {
    if (is_const_expr(node->args->next->next)) {
      gen_expr(node->args);
      push();
      gen_expr(node->args->next);
      emitlin("\tmov\t(%rsp),%rdi");
      gen_memcpy(eval(node->args->next->next));
      pop("%rax");
      return true;
    }
  } else if (!strcmp(name, "logbl")) {
    gen_expr(node->args);
    emitlin("\
\tfxtract\n\
\tfstp\t%st");
    return true;
  } else if (!strcmp(name, "isgreater")) {
    gen_comis(node, "comisd", 1, 0, "a");
    return true;
  } else if (!strcmp(name, "isgreaterequal")) {
    gen_comis(node, "comisd", 1, 0, "ae");
    return true;
  } else if (!strcmp(name, "isless")) {
    gen_comis(node, "comisd", 0, 1, "a");
    return true;
  } else if (!strcmp(name, "islessequal")) {
    gen_comis(node, "comisd", 0, 1, "ae");
    return true;
  } else if (!strcmp(name, "islessgreater")) {
    gen_comis(node, "ucomisd", 1, 0, "ne");
    return true;
  } else if (!strcmp(name, "isunordered")) {
    gen_comis(node, "ucomisd", 1, 0, "p");
    return true;
  } else if (!strcmp(name, "isnan")) {
    gen_comis(node, "ucomisd", 0, 0, "p");
    return true;
  } else if (!strcmp(name, "nanf")) {
    emitlin("\
\tmov\t$0x7fc00000,%eax\n\
\tmovd\t%eax,%xmm0");
    return true;
  } else if (!strcmp(name, "nan")) {
    emitlin("\
\tmov\t$0x7fffffffffffffff,%rax\n\
\tmovq\t%rax,%xmm0");
    return true;
  } else if (!strcmp(name, "nanl")) {
    emitlin("\
\tpush\t$0x7fc00000\n\
\tflds\t(%rsp)\n\
\tpop\t%rax");
    return true;
  } else if (!strcmp(name, "inff") || !strcmp(name, "huge_valf")) {
    emitlin("\
\tmov\t$0x7f800000,%eax\n\
\tmovd\t%eax,%xmm0");
    return true;
  } else if (!strcmp(name, "inf") || !strcmp(name, "huge_val")) {
    emitlin("\
\tmov\t$0x7ff0000000000000,%rax\n\
\tmovq\t%rax,%xmm0");
    return true;
  } else if (!strcmp(name, "infl") || !strcmp(name, "huge_vall")) {
    emitlin("\
\tpush\t$0x7f800000\n\
\tflds\t(%rsp)\n\
\tpop\t%rax");
    return true;
  } else if (!strcmp(name, "isinf")) {
    gen_expr(node->args);
    emitlin("\
\tmov\t$0x7fffffffffffffff,%rax\n\
\tmovq\t%rax,%xmm1\n\
\tandps\t%xmm1,%xmm0\n\
\tmov\t$0x7fefffffffffffff,%rax\n\
\tmovq\t%rax,%xmm1\n\
\txor\t%eax,%eax\n\
\tcomisd\t%xmm1,%xmm0\n\
\tseta\t%al");
    return true;
  } else if (!strcmp(name, "isfinite")) {
    gen_expr(node->args);
    emitlin("\
\tmov\t$0x7fffffffffffffff,%rax\n\
\tmovq\t%rax,%xmm1\n\
\tandps\t%xmm1,%xmm0\n\
\tmov\t$0x7fefffffffffffff,%rax\n\
\tmovq\t%rax,%xmm1\n\
\txor\t%eax,%eax\n\
\tcomisd\t%xmm0,%xmm1\n\
\tsetnb\t%al");
    return true;
  } else if (!strcmp(name, "signbitf")) {
    gen_expr(node->args);
    emitlin("\
\tmovd\t%xmm0,%eax\n\
\tand\t$-2147483648,%eax");
    return true;
  } else if (!strcmp(name, "signbit")) {
    gen_expr(node->args);
    emitlin("\
\tmovmskpd\t%xmm0,%eax\n\
\tand\t$1,%eax");
    return true;
  } else if (!strcmp(name, "signbitl")) {
    gen_expr(node->args);
    emitlin("\
\tfxam\n\
\tfnstsw\t%ax\n\
\tfstp\t%st\n\
\tand\t$0x200,%eax");
    return true;
  }
  return false;
}

static int GetSseIntSuffix(Type *ty) {
  switch (ty->kind) {
    case TY_CHAR:
      return 'b';
    case TY_SHORT:
      return 'w';
    case TY_INT:
      return 'd';
    case TY_LONG:
      return 'q';
    default:
      UNREACHABLE();
  }
}

static void HandleAtomicArithmetic(Node *node, const char *op) {
  gen_expr(node->lhs);
  push();
  gen_expr(node->rhs);
  pop("%r9");
  println("\tmov\t%s,%s", reg_ax(node->ty->size), reg_si(node->ty->size));
  println("\tmov\t(%%r9),%s", reg_ax(node->ty->size));
  println("1:\tmov\t%s,%s", reg_ax(node->ty->size), reg_dx(node->ty->size));
  println("\tmov\t%s,%s", reg_ax(node->ty->size), reg_di(node->ty->size));
  println("\t%s\t%s,%s", op, reg_si(node->ty->size), reg_dx(node->ty->size));
  println("\tlock cmpxchg\t%s,(%%r9)", reg_dx(node->ty->size));
  println("\tjnz\t1b");
  println("\tmov\t%s,%s", reg_di(node->ty->size), reg_ax(node->ty->size));
}

// Generate code for a given node.
void gen_expr(Node *node) {
  char fbuf[32];
  print_loc(node->tok->file->file_no, node->tok->line_no);
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
          g_ffmt_p(fbuf, &u.f32, 7, sizeof(fbuf), 0);
          println("\tmov\t$%#x,%%eax\t# float %s", u.u32, fbuf);
          emitlin("\tmovq\t%rax,%xmm0");
          return;
        }
        case TY_DOUBLE: {
          union {
            double f64;
            uint64_t u64;
          } u = {node->fval};
          g_dfmt_p(fbuf, &u.f64, 16, sizeof(fbuf), 0);
          println("\tmov\t$%#lx,%%rax\t# double %s", u.u64, fbuf);
          emitlin("\tmovq\t%rax,%xmm0");
          return;
        }
        case TY_LDOUBLE: {
          union {
            long double f80;
            uint64_t u64[2];
          } u;
          bzero(&u, sizeof(u));
          u.f80 = node->fval;
          g_xfmt_p(fbuf, &u.f80, 19, sizeof(fbuf), 0);
          println("\tmov\t$%lu,%%rax\t# long double %s", u.u64[0], fbuf);
          emitlin("\tmov\t%rax,-16(%rsp)");
          println("\tmov\t$%lu,%%rax", u.u64[1]);
          emitlin("\tmov\t%rax,-8(%rsp)");
          emitlin("\tfldt\t-16(%rsp)");
          return;
        }
        case TY_INT128:
          print_mov_imm(node->val, "%rax", "%eax");
          if (node->ty->is_unsigned) {
            emitlin("\txor\t%edx,%edx");
          } else {
            emitlin("\tcqto");
          }
          return;
        default:
          print_mov_imm(node->val, "%rax", "%eax");
          return;
      }
    }
    case ND_NEG:
      gen_expr(node->lhs);
      switch (node->ty->kind) {
        case TY_FLOAT:
          emitlin("\tmov\t$-2147483648,%eax");
          emitlin("\tmovd\t%eax,%xmm1");
          if (node->ty->vector_size == 16) {
            emitlin("\tpshufd\t$0,%xmm1");
          }
          emitlin("\txorps\t%xmm1,%xmm0");
          return;
        case TY_DOUBLE:
          emitlin("\tmov\t$1,%eax");
          emitlin("\tror\t%rax");
          emitlin("\tmovq\t%rax,%xmm1");
          if (node->ty->vector_size == 16) {
            emitlin("\tpshufd\t$0b01000100,%xmm1");
          }
          emitlin("\txorpd\t%xmm1,%xmm0");
          return;
        case TY_LDOUBLE:
          emitlin("\tfchs");
          return;
        case TY_INT128:
          emitlin("\tneg\t%rax");
          emitlin("\tadc\t$0,%rdx");
          emitlin("\tneg\t%rdx");
          return;
      }
      char *ax;
      if (node->lhs->ty->kind == TY_LONG || node->lhs->ty->base) {
        ax = "%rax";
      } else {
        ax = "%eax";
      }
      println("\tneg\t%s", ax);
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
        println("\tshl\t$%d,%%rax", 64 - mem->bit_width - mem->bit_offset);
        if (mem->ty->is_unsigned) {
          println("\tshr\t$%d,%%rax", 64 - mem->bit_width);
        } else {
          println("\tsar\t$%d,%%rax", 64 - mem->bit_width);
        }
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
        // If the lhs is a bitfield, we need to read the current value
        // from memory and merge it with a new value.
        emitlin("\tmov\t%rax,%r8");
        Member *mem = node->lhs->member;
        emitlin("\tmov\t%rax,%rdi");
        println("\tshl\t$%d,%%rdi", 64 - mem->bit_width);
        println("\tshr\t$%d,%%rdi", 64 - mem->bit_width - mem->bit_offset);
        emitlin("\tmov\t(%rsp),%rax");
        load(mem->ty);
        unsigned long mask = ((1ul << mem->bit_width) - 1) << mem->bit_offset;
        println("\tmov\t$%#lx,%%r9", ~mask);
        emitlin("\tand\t%r9,%rax");
        emitlin("\tor\t%rdi,%rax");
        store(node->ty);
        emitlin("\tmov\t%r8,%rax");
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
      gen_cast(node->lhs->ty, node->ty);
      return;
    case ND_MEMZERO:
      switch (node->var->ty->size) {
        case 1:
          println("\tmovb\t$0,%d(%%rbp)", node->var->offset);
          break;
        case 2:
          println("\tmovw\t$0,%d(%%rbp)", node->var->offset);
          break;
        case 4:
          println("\tmovl\t$0,%d(%%rbp)", node->var->offset);
          break;
        case 8:
          println("\tmovq\t$0,%d(%%rbp)", node->var->offset);
          break;
        case 9 ... 16:
          emitlin("\txor\t%eax,%eax");
          println("\tmov\t%%rax,%d(%%rbp)", node->var->offset);
          println("\tmov\t%%rax,%d(%%rbp)",
                  node->var->offset + 8 - (16 - node->var->ty->size));
          break;
        default:
          // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
          print_mov_imm(node->var->ty->size, "%rcx", "%ecx");
          println("\tlea\t%d(%%rbp),%%rdi", node->var->offset);
          emitlin("\txor\t%eax,%eax");
          emitlin("\trep stosb");
          break;
      }
      return;
    case ND_COND: {
      int c = count();
      gen_expr(node->cond);
      cmp_zero(node->cond->ty);
      println("\tje\t.L.else.%d", c);
      gen_expr(node->then);
      println("\tjmp\t.L.end.%d", c);
      println(".L.else.%d:", c);
      gen_expr(node->els);
      println(".L.end.%d:", c);
      return;
    }
    case ND_NOT:
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      emitlin("\tsete\t%al");
      emitlin("\tmovzbq\t%al,%rax");
      return;
    case ND_BITNOT:
      gen_expr(node->lhs);
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\tnot\t%rax");
        emitlin("\tnot\t%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        emitlin("\tpcmpeqd\t%xmm1,%xmm1");
        emitlin("\tpxor\t%xmm1,%xmm0");
      } else {
        emitlin("\tnot\t%rax");
      }
      return;
    case ND_LOGAND: {
      int c = count();
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      println("\tje\t.L.false.%d", c);
      gen_expr(node->rhs);
      cmp_zero(node->rhs->ty);
      println("\tje\t.L.false.%d", c);
      emitlin("\tmov\t$1,%eax");
      println("\tjmp\t.L.end.%d", c);
      println(".L.false.%d:", c);
      emitlin("\txor\t%eax,%eax");
      println(".L.end.%d:", c);
      return;
    }
    case ND_LOGOR: {
      int c = count();
      gen_expr(node->lhs);
      cmp_zero(node->lhs->ty);
      println("\tjne\t.L.true.%d", c);
      gen_expr(node->rhs);
      cmp_zero(node->rhs->ty);
      println("\tjne\t.L.true.%d", c);
      emitlin("\txor\t%eax,%eax");
      println("\tjmp\t.L.end.%d", c);
      println(".L.true.%d:", c);
      emitlin("\tmov\t$1,%eax");
      println(".L.end.%d:", c);
      return;
    }
    case ND_FUNCALL: {
      const char *funcname = NULL;
      if (node->lhs->kind == ND_VAR) {
        if (startswith(nameof(node->lhs->var), "__builtin_")) {
          funcname = nameof(node->lhs->var) + 10;
          if (gen_builtin_funcall(node, funcname)) {
            return;
          }
        } else if (!opt_no_builtin) {
          if (gen_builtin_funcall(node, nameof(node->lhs->var))) {
            return;
          }
        }
      }
      int stack_args = push_args(node);
      if (!funcname) {
        if (node->lhs->kind == ND_VAR && !node->lhs->var->is_local &&
            !node->lhs->var->is_tls && node->lhs->ty->kind == TY_FUNC) {
          if (!opt_pic || node->lhs->var->is_definition) {
            funcname = nameof(node->lhs->var);
          } else {
            funcname = xasprintf("%s@gotpcrel(%%rip)", nameof(node->lhs->var));
          }
        } else {
          gen_expr(node->lhs);
          funcname = NULL;
        }
      }
      int gp = 0, fp = 0;
      // If the return type is a large struct/union, the caller passes
      // a pointer to a buffer as if it were the first argument.
      if (node->ret_buffer && node->ty->size > 16) {
        pop(argreg64[gp++]);
      }
      for (Node *arg = node->args; arg; arg = arg->next) {
        Type *ty = arg->ty;
        switch (ty->kind) {
          case TY_STRUCT:
          case TY_UNION:
            if (ty->size > 16) continue;
            bool fp1 = has_flonum1(ty);
            bool fp2 = has_flonum2(ty);
            if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
              if (fp1) {
                popf(ty, fp++);
              } else {
                pop(argreg64[gp++]);
              }
              if (ty->size > 8) {
                if (fp2) {
                  popf(ty, fp++);
                } else {
                  pop(argreg64[gp++]);
                }
              }
            }
            break;
          case TY_FLOAT:
          case TY_DOUBLE:
            if (fp < FP_MAX) {
              popf(ty, fp++);
            }
            break;
          case TY_LDOUBLE:
            break;
          case TY_INT128:
            if (gp + 1 < GP_MAX) {
              int a = gp++;
              int b = gp++;
              pop2(argreg64[a], argreg64[b]);
            }
            break;
          default:
            if (gp < GP_MAX) {
              pop(argreg64[gp++]);
            }
            break;
        }
      }
      if (funcname) {
        if (node->lhs->ty->is_variadic) {
          print_mov_imm(fp, "%rax", "%eax");
        }
        println("\tcall\t%s", funcname);
      } else {
        if (!node->lhs->ty->is_variadic) {
          emitlin("\tcall\t*%rax");
        } else {
          emitlin("\tmov\t%rax,%r10");
          print_mov_imm(fp, "%rax", "%eax");
          emitlin("\tcall\t*%r10");
        }
      }
      if (stack_args) {
        println("\tadd\t$%d,%%rsp", stack_args * 8);
      }
      depth -= stack_args;
      // It looks like the most significant 48 or 56 bits in RAX may
      // contain garbage if a function return type is short or bool/char,
      // respectively. We clear the upper bits here.
      switch (node->ty->kind) {
        case TY_BOOL:
          emitlin("\tmovzbl\t%al,%eax");
          return;
        case TY_CHAR:
          if (node->ty->is_unsigned) {
            emitlin("\tmovzbl\t%al,%eax");
          } else {
            emitlin("\tmovsbl\t%al,%eax");
          }
          return;
        case TY_SHORT:
          if (node->ty->is_unsigned) {
            emitlin("\tmovzwl\t%ax,%eax");
          } else {
            emitlin("\tcwtl");
          }
          return;
      }
      // If the return type is a small struct, a value is returned
      // using up to two registers.
      if (node->ret_buffer && node->ty->size <= 16) {
        copy_ret_buffer(node->ret_buffer);
        println("\tlea\t%d(%%rbp),%%rax", node->ret_buffer->offset);
      }
      return;
    }
    case ND_LABEL_VAL:
      if (opt_pic) {
        println("\tlea\t%s(%%rip),%%rax", node->unique_label);
      } else {
        println("\tmov\t$%s,%%eax", node->unique_label);
      }
      return;
    case ND_CAS:
      gen_expr(node->cas_addr);
      push();
      gen_expr(node->cas_new);
      push();
      gen_expr(node->cas_old);
      emitlin("\tmov\t%rax,%r8");
      load(node->cas_old->ty->base);
      pop("%rdx");  // new
      pop("%rdi");  // addr
      println("\tlock cmpxchg %s,(%%rdi)", reg_dx(node->ty->size));
      println("\tmov\t%s,(%%r8)", reg_ax(node->ty->size));
      emitlin("\tsetz\t%al");
      emitlin("\tmovzbl\t%al,%eax");
      return;
    case ND_EXCH_N:
    case ND_TESTANDSET: {
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      println("\txchg\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    }
    case ND_TESTANDSETA: {
      gen_expr(node->lhs);
      push();
      println("\tmov\t$1,%%eax");
      pop("%rdi");
      println("\txchg\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    }
    case ND_LOAD: {
      gen_expr(node->rhs);
      push();
      gen_expr(node->lhs);
      println("\tmov\t(%%rax),%s", reg_ax(node->ty->size));
      pop("%rdi");
      println("\tmov\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    }
    case ND_LOAD_N: {
      gen_expr(node->lhs);
      println("\tmov\t(%%rax),%s", reg_ax(node->ty->size));
      return;
    }
    case ND_STORE: {
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      println("\tmov\t(%%rax),%s", reg_ax(node->ty->size));
      println("\tmov\t%s,(%%rdi)", reg_ax(node->ty->size));
      if (node->memorder) {
        println("\tmfence");
      }
      return;
    }
    case ND_STORE_N:
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      println("\tmov\t%s,(%%rdi)", reg_ax(node->ty->size));
      if (node->memorder) {
        println("\tmfence");
      }
      return;
    case ND_CLEAR:
      gen_expr(node->lhs);
      println("\tmov\t%%rax,%%rdi");
      println("\txor\t%%eax,%%eax");
      println("\tmov\t%s,(%%rdi)", reg_ax(node->ty->size));
      if (node->memorder) {
        println("\tmfence");
      }
      return;
    case ND_FETCHADD:
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      println("\txadd\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    case ND_FETCHSUB:
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      println("\tneg\t%s", reg_ax(node->ty->size));
      println("\txadd\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    case ND_FETCHXOR:
      HandleAtomicArithmetic(node, "xor");
      return;
    case ND_FETCHAND:
      HandleAtomicArithmetic(node, "and");
      return;
    case ND_FETCHOR:
      HandleAtomicArithmetic(node, "or");
      return;
    case ND_SUBFETCH:
      gen_expr(node->lhs);
      push();
      gen_expr(node->rhs);
      pop("%rdi");
      push();
      println("\tneg\t%s", reg_ax(node->ty->size));
      println("\txadd\t%s,(%%rdi)", reg_ax(node->ty->size));
      pop("%rdi");
      println("\tsub\t%s,%s", reg_di(node->ty->size), reg_ax(node->ty->size));
      return;
    case ND_RELEASE:
      gen_expr(node->lhs);
      push();
      pop("%rdi");
      println("\txor\t%%eax,%%eax");
      println("\tmov\t%s,(%%rdi)", reg_ax(node->ty->size));
      return;
    case ND_FPCLASSIFY:
      gen_fpclassify(node->fpc);
      return;
  }
  switch (node->lhs->ty->kind) {
    case TY_FLOAT:
    case TY_DOUBLE: {
      gen_expr(node->rhs);
      pushf(node->rhs->ty);
      gen_expr(node->lhs);
      popf(node->rhs->ty, 1);
      char sd = node->lhs->ty->kind == TY_FLOAT ? 's' : 'd';
      char ps = node->lhs->ty->vector_size == 16 ? 'p' : 's';
      switch (node->kind) {
        case ND_ADD:
          println("\tadd%c%c\t%%xmm1,%%xmm0", ps, sd);
          return;
        case ND_SUB:
          println("\tsub%c%c\t%%xmm1,%%xmm0", ps, sd);
          return;
        case ND_MUL:
          println("\tmul%c%c\t%%xmm1,%%xmm0", ps, sd);
          return;
        case ND_DIV:
          println("\tdiv%c%c\t%%xmm1,%%xmm0", ps, sd);
          return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
          println("\tucomis%c\t%%xmm0,%%xmm1", sd);
          if (node->kind == ND_EQ) {
            emitlin("\
\tsete\t%al\n\
\tsetnp\t%dl\n\
\tand\t%dl,%al");
          } else if (node->kind == ND_NE) {
            emitlin("\
\tsetne\t%al\n\
\tsetp\t%dl\n\
\tor\t%dl,%al");
          } else if (node->kind == ND_LT) {
            emitlin("\tseta\t%al");
          } else {
            emitlin("\tsetae\t%al");
          }
          emitlin("\tand\t$1,%al");
          emitlin("\tmovzbl\t%al,%eax");
          return;
      }
      error_tok(node->tok, "invalid expression");
    }
    case TY_LDOUBLE: {
      gen_expr(node->lhs);
      gen_expr(node->rhs);
      switch (node->kind) {
        case ND_ADD:
          emitlin("\tfaddp");
          return;
        case ND_SUB:
          emitlin("\tfsubrp");
          return;
        case ND_MUL:
          emitlin("\tfmulp");
          return;
        case ND_DIV:
          emitlin("\tfdivrp");
          return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
          emitlin("\tfcomip");
          emitlin("\tfstp\t%st");
          if (node->kind == ND_EQ)
            emitlin("\tsete\t%al");
          else if (node->kind == ND_NE)
            emitlin("\tsetne\t%al");
          else if (node->kind == ND_LT)
            emitlin("\tseta\t%al");
          else
            emitlin("\tsetae\t%al");
          emitlin("\tmovzbl\t%al,%eax");
          return;
      }
      error_tok(node->tok, "invalid expression");
    }
  }
  if (node->lhs->ty->vector_size == 16) {
    gen_expr(node->rhs);
    pushx();
    gen_expr(node->lhs);
    popx(1);
  } else if (node->rhs->ty->kind == TY_INT128) {
    gen_expr(node->rhs);
    push2();
    gen_expr(node->lhs);
    pop2("%rdi", "%rsi");
  } else if (!opt_pic && is_const_expr(node->rhs)) {
    /* shortcut path for immediates */
    char **label = NULL;
    uint64_t val = eval2(node->rhs, &label);
    gen_expr(node->lhs);
    if (label) {
      println("\tmov\t$%s%+ld,%%edi", *label, val);
    } else {
      print_mov_imm(val, "%rdi", "%edi");
    }
  } else {
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("%rdi");
  }
  char *ax, *di;
  if (node->lhs->ty->kind == TY_LONG || node->lhs->ty->base) {
    ax = "%rax";
    di = "%rdi";
  } else {
    ax = "%eax";
    di = "%edi";
  }
  switch (node->kind) {
    case ND_PMOVMSKB:
      println("\tmovdqu\t(%%rax),%%xmm0");
      println("\tpmovmskb\t%%xmm0,%%rax");
      break;
    case ND_MOVNTDQ:
      println("\tmovdqu\t(%%rdi),%%xmm0");
      println("\tmovntdq\t%%xmm0,(%%rax)");
      break;
    case ND_ADD:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\tadd\t%rdi,%rax");
        emitlin("\tadc\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        println("\tpadd%c\t%%xmm1,%%xmm0", GetSseIntSuffix(node->lhs->ty));
      } else {
        println("\tadd\t%s,%s", di, ax);
      }
      break;
    case ND_SUB:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\tsub\t%rdi,%rax");
        emitlin("\tsbb\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        println("\tpsub%c\t%%xmm1,%%xmm0", GetSseIntSuffix(node->lhs->ty));
      } else {
        println("\tsub\t%s,%s", di, ax);
      }
      break;
    case ND_MUL:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\
\timul\t%rdi,%rdx\n\
\timul\t%rax,%rsi\n\
\tadd\t%rdx,%rsi\n\
\tmul\t%rdi\n\
\tadd\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        switch (node->lhs->ty->kind) {
          case TY_CHAR:
            emitlin("\
\tmovaps\t%xmm1,%xmm2\n\
\tmovaps\t%xmm0,%xmm3\n\
\tpunpcklbw %xmm0,%xmm3\n\
\tpunpcklbw %xmm1,%xmm2\n\
\tpmullw\t%xmm3,%xmm2\n\
\tpunpckhbw %xmm0,%xmm0\n\
\tpcmpeqd\t%xmm3,%xmm3\n\
\tpunpckhbw %xmm1,%xmm1\n\
\tpmullw\t%xmm0,%xmm1\n\
\tandps\t%xmm3,%xmm2\n\
\tmovaps\t%xmm2,%xmm0\n\
\tandps\t%xmm3,%xmm1\n\
\tpackuswb %xmm1,%xmm0");
            break;
          case TY_SHORT:
            emitlin("\tpmullw\t%xmm1,%xmm0");
            break;
          case TY_INT:
            if (opt_sse4) {
              emitlin("\tpmulld\t%xmm1,%xmm0");
            } else {
              emitlin("\
\tmovaps\t%xmm0,%xmm2\n\
\tpsrlq\t$32,%xmm0\n\
\tpmuludq\t%xmm1,%xmm2\n\
\tpsrlq\t$32,%xmm1\n\
\tpmuludq\t%xmm1,%xmm0\n\
\tpshufd\t$8,%xmm2,%xmm2\n\
\tpshufd\t$8,%xmm0,%xmm1\n\
\tpunpckldq %xmm1,%xmm2\n\
\tmovaps\t%xmm2,%xmm0");
            }
          case TY_LONG:
            emitlin("\
\tmovaps\t%xmm1,%xmm2\n\
\tmovaps\t%xmm0,%xmm3\n\
\tpmuludq\t%xmm1,%xmm3\n\
\tmovaps\t%xmm2,%xmm4\n\
\tmovaps\t%xmm0,%xmm1\n\
\tpsrlq\t$32,%xmm4\n\
\tpsrlq\t$32,%xmm1\n\
\tpmuludq\t%xmm4,%xmm0\n\
\tpmuludq\t%xmm2,%xmm1\n\
\tpaddq\t%xmm0,%xmm1\n\
\tpsllq\t$32,%xmm1\n\
\tpaddq\t%xmm1,%xmm3\n\
\tmovaps\t%xmm3,%xmm0");
            break;
          default:
            UNREACHABLE();
        }
      } else {
        println("\timul\t%s,%s", di, ax);
      }
      break;
    case ND_DIV:
    case ND_REM:
      if (node->lhs->ty->kind == TY_INT128) {
        bool skew;
        if ((skew = (depth & 1))) {
          emitlin("\tsub\t$8,%rsp");
          depth++;
        }
        emitlin("\
\tmov\t%rsi,%rcx\n\
\tmov\t%rdx,%rsi\n\
\tmov\t%rdi,%rdx\n\
\tmov\t%rax,%rdi");
        if (node->kind == ND_DIV) {
          if (node->ty->is_unsigned) {
            emitlin("\tcall\t__udivti3");
          } else {
            emitlin("\tcall\t__divti3");
          }
        } else {
          if (node->ty->is_unsigned) {
            emitlin("\tcall\t__umodti3");
          } else {
            emitlin("\tcall\t__modti3");
          }
        }
        if (skew) {
          emitlin("\tadd\t$8,%rsp");
          depth--;
        }
      } else if (node->lhs->ty->vector_size == 16) {
        error_tok(node->tok, "no div/rem sse instruction");
      } else {
        if (node->ty->is_unsigned) {
          emitlin("\txor\t%edx,%edx");
          println("\tdiv\t%s", di);
        } else {
          if (node->lhs->ty->size == 8) {
            emitlin("\tcqto");
          } else {
            emitlin("\tcltd");
          }
          println("\tidiv\t%s", di);
        }
        if (node->kind == ND_REM) {
          emitlin("\tmov\t%rdx,%rax");
        }
      }
      break;
    case ND_BINAND:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\tand\t%rdi,%rax");
        emitlin("\tand\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        emitlin("\tpand\t%xmm1,%xmm0");
      } else {
        println("\tand\t%s,%s", di, ax);
      }
      break;
    case ND_BINOR:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\tor\t%rdi,%rax");
        emitlin("\tor\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        emitlin("\tpor\t%xmm1,%xmm0");
      } else {
        println("\tor\t%s,%s", di, ax);
      }
      break;
    case ND_BINXOR:
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\txor\t%rdi,%rax");
        emitlin("\txor\t%rsi,%rdx");
      } else if (node->lhs->ty->vector_size == 16) {
        emitlin("\tpxor\t%xmm1,%xmm0");
      } else {
        println("\txor\t%s,%s", di, ax);
      }
      break;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      if (node->lhs->ty->kind == TY_INT128) {
        switch (node->kind) {
          case ND_EQ:
            emitlin("\
\txor\t%rax,%rdi\n\
\txor\t%rdx,%rsi\n\
\tor\t%rsi,%rdi\n\
\tsete\t%al");
            break;
          case ND_NE:
            emitlin("\
\txor\t%rax,%rdi\n\
\txor\t%rdx,%rsi\n\
\tor\t%rsi,%rdi\n\
\tsetne\t%al");
            break;
          case ND_LT:
            if (node->lhs->ty->is_unsigned) {
              emitlin("\
\tcmp\t%rdi,%rax\n\
\tmov\t%rdx,%rax\n\
\tsbb\t%rsi,%rax\n\
\tsetc\t%al");
            } else {
              emitlin("\
\tcmp\t%rdi,%rax\n\
\tmov\t%rdx,%rax\n\
\tsbb\t%rsi,%rax\n\
\tsetl\t%al");
            }
            break;
          case ND_LE:
            if (node->lhs->ty->is_unsigned) {
              emitlin("\
\tcmp\t%rax,%rdi\n\
\tsbb\t%rdx,%rsi\n\
\tsetnc\t%al");
            } else {
              emitlin("\
\tcmp\t%rax,%rdi\n\
\tsbb\t%rdx,%rsi\n\
\tsetge\t%al");
            }
            break;
        }
      } else if (node->lhs->ty->vector_size == 16) {
        switch (node->kind) {
          case ND_EQ:
            switch (node->lhs->ty->kind) {
              case TY_CHAR:
              case TY_SHORT:
              case TY_INT:
                println("\tpcmpeq%c\t%%xmm1,%%xmm0",
                        GetSseIntSuffix(node->lhs->ty));
                break;
              default:
                error_tok(node->tok, "todo sse eq");
            }
            break;
          case ND_NE:
            switch (node->lhs->ty->kind) {
              case TY_CHAR:
              case TY_SHORT:
              case TY_INT:
                println("\tpcmpeq%c\t%%xmm1,%%xmm0",
                        GetSseIntSuffix(node->lhs->ty));
                emitlin("\tpcmpeqd\t%xmm1,%xmm1\n"
                        "\tandnps\t%xmm1,%xmm0");
                break;
              default:
                error_tok(node->tok, "todo sse ne");
            }
            break;
          case ND_LT:
            if (node->lhs->ty->is_unsigned) {
              switch (node->lhs->ty->kind) {
                case TY_CHAR:
                case TY_SHORT:
                  println("\tpsubus%c\t%%xmm0,%%xmm1",
                          GetSseIntSuffix(node->lhs->ty));
                  emitlin("\tmovaps\t%xmm1,%xmm0\n"
                          "\txorps\t%xmm1,%xmm1");
                  println("\tpcmpeq%c\t%%xmm1,%%xmm0",
                          GetSseIntSuffix(node->lhs->ty));
                  emitlin("\tpcmpeqd\t%xmm1,%xmm1\n"
                          "\tandnps\t%xmm1,%xmm0");
                  break;
                default:
                  error_tok(node->tok, "todo sse ltu");
              }
            } else {
              switch (node->lhs->ty->kind) {
                case TY_CHAR:
                case TY_SHORT:
                case TY_INT:
                  println("\tpcmpgt%c\t%%xmm0,%%xmm1",
                          GetSseIntSuffix(node->lhs->ty));
                  emitlin("\tmovaps\t%xmm1,%xmm0");
                  break;
                default:
                  error_tok(node->tok, "todo sse lt");
              }
            }
            break;
          case ND_LE:
            if (node->lhs->ty->is_unsigned) {
              switch (node->lhs->ty->kind) {
                case TY_CHAR:
                  emitlin("\tpminub\t%xmm0,%xmm1\n"
                          "\tpcmpeqb\t%xmm1,%xmm0");
                  break;
                case TY_SHORT:
                  emitlin("\tpsubusw\t%xmm1,%xmm0\n"
                          "\txorps\t%xmm1,%xmm1\n"
                          "\tpcmpeqw\t%xmm1,%xmm0");
                  break;
                case TY_INT:
                  emitlin("\tmov\t$-2147483648,%eax\n"
                          "\tmovd\t%eax,%xmm2\n"
                          "\tpshufd\t$0,%xmm2");
                  emitlin("\tpcmpgtd\t%xmm1,%xmm0");
                  emitlin("\tpcmpeqd\t%xmm1,%xmm1\n"
                          "\tandnps\t%xmm1,%xmm0");
                  break;
                default:
                  error_tok(node->tok, "todo sse leu");
              }
            } else {
              switch (node->lhs->ty->kind) {
                case TY_SHORT:
                  emitlin("\tpminsw\t%xmm0,%xmm1\n"
                          "\tpcmpeqw\t%xmm1,%xmm0");
                  break;
                case TY_CHAR:
                case TY_INT:
                  println("\tpcmpgt%c\t%%xmm1,%%xmm0",
                          GetSseIntSuffix(node->lhs->ty));
                  emitlin("\tpcmpeqd\t%xmm1,%xmm1\n"
                          "\tandnps\t%xmm1,%xmm0");
                  break;
                default:
                  error_tok(node->tok, "todo sse le");
              }
            }
            break;
        }
      } else {
        println("\tcmp\t%s,%s", di, ax);
        if (node->kind == ND_EQ) {
          emitlin("\tsete\t%al");
        } else if (node->kind == ND_NE) {
          emitlin("\tsetne\t%al");
        } else if (node->kind == ND_LT) {
          if (node->lhs->ty->is_unsigned) {
            emitlin("\tsetb\t%al");
          } else {
            emitlin("\tsetl\t%al");
          }
        } else if (node->kind == ND_LE) {
          if (node->lhs->ty->is_unsigned) {
            emitlin("\tsetbe\t%al");
          } else {
            emitlin("\tsetle\t%al");
          }
        }
      }
      emitlin("\tmovzbl\t%al,%eax");
      break;
    case ND_SHL:
      emitlin("\tmov\t%edi,%ecx");
      if (node->lhs->ty->kind == TY_INT128) {
        emitlin("\
\tshld\t%cl,%rax,%rdx\n\
\tshl\t%cl,%rax\n\
\txor\t%edi,%edi\n\
\tand\t$64,%cl\n\
\tcmovne\t%rax,%rdx\n\
\tcmovne\t%rdi,%rax");
      } else if (node->lhs->ty->vector_size == 16) {
        error_tok(node->tok, "todo sse shl");
      } else {
        println("\tshl\t%%cl,%s", ax);
      }
      break;
    case ND_SHR:
      emitlin("\tmov\t%edi,%ecx");
      if (node->lhs->ty->is_unsigned) {
        if (node->lhs->ty->kind == TY_INT128) {
          emitlin("\
\tshrd\t%cl,%rdx,%rax\n\
\tshr\t%cl,%rdx\n\
\txor\t%edi,%edi\n\
\tand\t$64,%cl\n\
\tcmovne\t%rdx,%rax\n\
\tcmovne\t%rdi,%rdx");
        } else if (node->lhs->ty->vector_size == 16) {
          error_tok(node->tok, "todo sse shr");
        } else {
          println("\tshr\t%%cl,%s", ax);
        }
      } else {
        if (node->lhs->ty->kind == TY_INT128) {
          emitlin("\
\tshrd\t%cl,%rdx,%rax\n\
\tsar\t%cl,%rdx\n\
\tmov\t%rdx,%rdi\n\
\tsar\t$63,%rdi\n\
\tand\t$64,%cl\n\
\tcmovne\t%rdx,%rax\n\
\tcmovne\t%rdi,%rdx");
        } else if (node->lhs->ty->vector_size == 16) {
          error_tok(node->tok, "todo sse sar");
        } else {
          println("\tsar\t%%cl,%s", ax);
        }
      }
      break;
    default:
      error_tok(node->tok, "invalid expression");
  }
}

void gen_stmt(Node *node) {
  print_loc(node->tok->file->file_no, node->tok->line_no);
  switch (node->kind) {
    case ND_IF: {
      int c = count();
      gen_expr(node->cond);
      cmp_zero(node->cond->ty);
      println("\tje\t.L.else.%d", c);
      gen_stmt(node->then);
      println("\tjmp\t.L.end.%d", c);
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
        println("\tje\t%s", node->brk_label);
      }
      gen_stmt(node->then);
      println("%s:", node->cont_label);
      if (node->inc) gen_expr(node->inc);
      println("\tjmp\t.L.begin.%d", c);
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
      println("\tjne\t.L.begin.%d", c);
      println("%s:", node->brk_label);
      return;
    }
    case ND_SWITCH:  // TODO(jart): 64-bit?
      gen_expr(node->cond);
      for (Node *n = node->case_next; n; n = n->case_next) {
        char *ax = (node->cond->ty->size == 8) ? "%rax" : "%eax";
        char *di = (node->cond->ty->size == 8) ? "%rdi" : "%edi";
        if (n->begin == n->end) {
          println("\tcmp\t$%ld,%s", n->begin, ax);
          println("\tje\t%s", n->label);
          continue;
        }
        // [GNU] Case ranges
        println("\tmov\t%s,%s", ax, di);
        println("\tsub\t$%ld,%s", n->begin, di);
        println("\tcmp\t$%ld,%s", n->end - n->begin, di);
        println("\tjbe\t%s", n->label);
      }
      if (node->default_case) {
        println("\tjmp\t%s", node->default_case->label);
      }
      println("\tjmp\t%s", node->brk_label);
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
      println("\tjmp\t%s", node->unique_label);
      return;
    case ND_GOTO_EXPR:
      gen_expr(node->lhs);
      emitlin("\tjmp\t*%rax");
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
            if (ty->size <= 16) {
              copy_struct_reg();
            } else {
              copy_struct_mem();
            }
            break;
        }
      }
      println("\tjmp\t.L.return.%s", nameof(current_fn));
      return;
    case ND_EXPR_STMT:
      gen_expr(node->lhs);
      return;
    case ND_ASM:
      gen_asm(node->azm);
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
        case TY_INT128:
          gp++;
          if (gp++ < GP_MAX) continue;
        default:
          if (gp++ < GP_MAX) continue;
      }
      top = ROUNDUP(top, 8);
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
      bottom = ROUNDUP(bottom, align);
      var->offset = -bottom;
    }
    fn->stack_size = ROUNDUP(bottom, 16);
  }
}

static void emit_data(Obj *prog) {
  for (Obj *var = prog; var; var = var->next) {
    if (var->is_function || !var->is_definition) continue;
    flushln();
    fputc('\n', output_stream);
    print_visibility(var);
    int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)
                    ? MAX(16, var->align)
                    : var->align;
    if (opt_common && var->is_tentative) {
      println("\t.comm\t%s,%d,%d", nameof(var), var->ty->size, align);
    } else {
      if (var->section) {
        println("\t.section %s,\"aw\",@%s", var->section,
                var->init_data ? "progbits" : "nobits");
      } else if (var->is_tls) {
        println("\t.section .t%s,\"awT\",@%s",
                var->init_data ? "progbits" : "nobits",
                var->init_data ? "data" : "bss");
#if 0 /* TODO: unflag if assigned to array */
      } else if (align <= 1 && var->is_string_literal) {
        println("\t.section .rodata.str1.1,\"aSM\",@progbits,1");
#endif
      } else if (opt_data_sections) {
        println("\tsection .%s.%s", var->init_data ? "data" : "bss",
                nameof(var));
      } else {
        println("\t.%s", var->init_data ? "data" : "bss");
      }
      print_align(align);
      println("\t.type\t%s,@object", nameof(var));
      /* println("\t.size\t%s,%d", nameof(var), var->ty->size); */
      println("%s:", nameof(var));
      if (var->init_data) {
        int pos = 0;
        Relocation *rel = var->rel;
        if (!rel && is_quotable_string(var->init_data, var->ty->size)) {
          println("\t.asciz\t\"%s\"", var->init_data);
        } else {
          while (pos < var->ty->size) {
            if (rel && rel->offset == pos) {
              assert(pos + 8 <= var->ty->size);
              println("\t.quad\t%s%+ld", *rel->label, rel->addend);
              rel = rel->next;
              pos += 8;
            } else {
              println("\t.byte\t%d", var->init_data[pos++]);
            }
          }
        }
      } else {
        println("\t.zero\t%d", var->ty->size);
      }
    }
  }
}

static void store_fp(Obj *fn, int r, int offset, int sz) {
  switch (sz) {
    case 4:
      println("\tmovss\t%%xmm%d,%d(%%rbp)", r, offset);
      return;
    case 8:
      println("\tmovsd\t%%xmm%d,%d(%%rbp)", r, offset);
      return;
    case 16:
      println("\t%s\t%%xmm%d,%d(%%rbp)",
              fn->is_force_align_arg_pointer ? "movups" : "movaps", r, offset);
      return;
  }
  UNREACHABLE();
}

static void store_gp(int r, int offset, int sz) {
  switch (sz) {
    case 1:
      println("\tmov\t%s,%d(%%rbp)", argreg8[r], offset);
      return;
    case 2:
      println("\tmov\t%s,%d(%%rbp)", argreg16[r], offset);
      return;
    case 4:
      println("\tmov\t%s,%d(%%rbp)", argreg32[r], offset);
      return;
    case 8:
      println("\tmov\t%s,%d(%%rbp)", argreg64[r], offset);
      return;
    default:
      for (int i = 0; i < sz; i++) {
        println("\tmov\t%s,%d(%%rbp)", argreg8[r], offset + i);
        println("\tshr\t$8,%s", argreg64[r]);
      }
      return;
  }
}

static void emit_function_hook(void) {
  if (opt_nop_mcount) {
    print_profiling_nop();
  } else if (opt_fentry) {
    println("\tcall\t__fentry__%s", gotpcrel());
  } else if (opt_pg) {
    println("\tcall\tmcount%s", gotpcrel());
  } else {
    print_profiling_nop();
  }
}

static void save_caller_saved_registers(void) {
  emitlin("\
\tpush\t%rdi\n\
\tpush\t%rsi\n\
\tpush\t%rdx\n\
\tpush\t%rcx\n\
\tpush\t%r8\n\
\tpush\t%r9\n\
\tpush\t%r10\n\
\tpush\t%r11");
}

static void restore_caller_saved_registers(void) {
  emitlin("\
\tpop\t%r11\n\
\tpop\t%r10\n\
\tpop\t%r9\n\
\tpop\t%r8\n\
\tpop\t%rcx\n\
\tpop\t%rdx\n\
\tpop\t%rsi\n\
\tpop\t%rdi");
}

static void emit_text(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function || !fn->is_definition) continue;
    // No code is emitted for "static inline" functions
    // if no one is referencing them.
    if (!fn->is_live) continue;
    flushln();
    fputc('\n', output_stream);
    if (fn->section) {
      println("\t.section %s,\"a\",@progbits", fn->section);
    } else if (opt_function_sections) {
      println("\tsection .text.%s", nameof(fn));
    } else {
      emitlin("\t.text");
    }
    print_visibility(fn);
    print_align(fn->align);
    println("\t.type\t%s,@function", nameof(fn));
    println("%s:", nameof(fn));
    current_fn = fn;
    // Prologue
    emitlin("\tpush\t%rbp");
    emitlin("\tmov\t%rsp,%rbp");
    if (!fn->is_no_instrument_function) {
      emit_function_hook();
    }
    println("\tsub\t$%d,%%rsp", fn->stack_size);
    println("\tmov\t%%rsp,%d(%%rbp)", fn->alloca_bottom->offset);
    // Save arg registers if function is variadic
    if (fn->va_area) {
      int gp = 0, fp = 0;
      for (Obj *var = fn->params; var; var = var->next) {
        if (is_flonum(var->ty)) {
          fp++;
        } else {
          gp++;
        }
      }
      int off = fn->va_area->offset;
      // va_elem
      println("\tmovl\t$%d,%d(%%rbp)", gp * 8, off);           // gp_offset
      println("\tmovl\t$%d,%d(%%rbp)", fp * 8 + 48, off + 4);  // fp_offset
      println("\tmov\t%%rbp,%d(%%rbp)", off + 8);  // overflow_arg_area
      println("\taddq\t$16,%d(%%rbp)", off + 8);
      println("\tmov\t%%rbp,%d(%%rbp)", off + 16);  // reg_save_area
      println("\taddq\t$%d,%d(%%rbp)", off + 24, off + 16);
      // __reg_save_area__
      println("\tmov\t%%rdi,%d(%%rbp)", off + 24);
      println("\tmov\t%%rsi,%d(%%rbp)", off + 32);
      println("\tmov\t%%rdx,%d(%%rbp)", off + 40);
      println("\tmov\t%%rcx,%d(%%rbp)", off + 48);
      println("\tmov\t%%r8,%d(%%rbp)", off + 56);
      println("\tmov\t%%r9,%d(%%rbp)", off + 64);
      println("\tmovsd\t%%xmm0,%d(%%rbp)", off + 72);
      println("\tmovsd\t%%xmm1,%d(%%rbp)", off + 80);
      println("\tmovsd\t%%xmm2,%d(%%rbp)", off + 88);
      println("\tmovsd\t%%xmm3,%d(%%rbp)", off + 96);
      println("\tmovsd\t%%xmm4,%d(%%rbp)", off + 104);
      println("\tmovsd\t%%xmm5,%d(%%rbp)", off + 112);
      println("\tmovsd\t%%xmm6,%d(%%rbp)", off + 120);
      println("\tmovsd\t%%xmm7,%d(%%rbp)", off + 128);
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
          if (has_flonum(ty, 0, 8, 0)) {
            store_fp(fn, fp++, var->offset, MIN(8, ty->size));
          } else {
            store_gp(gp++, var->offset, MIN(8, ty->size));
          }
          if (ty->size > 8) {
            if (has_flonum(ty, 8, 16, 0)) {
              store_fp(fn, fp++, var->offset + 8, ty->size - 8);
            } else {
              store_gp(gp++, var->offset + 8, ty->size - 8);
            }
          }
          break;
        case TY_FLOAT:
        case TY_DOUBLE:
          store_fp(fn, fp++, var->offset, ty->size);
          break;
        case TY_INT128:
          store_gp(gp++, var->offset + 0, 8);
          store_gp(gp++, var->offset + 8, 8);
          break;
        default:
          store_gp(gp++, var->offset, ty->size);
      }
    }
    if (fn->is_force_align_arg_pointer) {
      emitlin("\tand\t$-16,%rsp");
    }
    if (fn->is_no_caller_saved_registers) {
      save_caller_saved_registers();
    }
    // Emit code
    gen_stmt(fn->body);
    assert(!depth);
    // [https://www.sigbus.info/n1570#5.1.2.2.3p1] The C spec defines
    // a special rule for the main function. Reaching the end of the
    // main function is equivalent to returning 0, even though the
    // behavior is undefined for the other functions.
    if (strcmp(nameof(fn), "main") == 0) {
      emitlin("\txor\t%eax,%eax");
    }
    // Epilogue
    println(".L.return.%s:", nameof(fn));
    if (fn->is_noreturn) {
      emitlin("\tud2");
    } else {
      if (fn->is_no_caller_saved_registers) {
        restore_caller_saved_registers();
      }
      emitlin("\tleave");
      emitlin("\tret");
    }
    /* println("\t.size\t%s,.-%s", nameof(fn), nameof(fn)); */
    if (fn->is_constructor) {
      emitlin("\t.section .ctors,\"aw\",@progbits");
      emitlin("\t.align\t8");
      println("\t.quad\t%s", nameof(fn));
    }
    if (fn->is_destructor) {
      emitlin("\t.section .dtors,\"aw\",@progbits");
      emitlin("\t.align\t8");
      println("\t.quad\t%s", nameof(fn));
    }
  }
}

static void emit_staticasms(StaticAsm *a) {
  if (!a) return;
  emit_staticasms(a->next);
  gen_asm(a->body);
}

void codegen(Obj *prog, FILE *out) {
  output_stream = out;
  File **files = get_input_files();
  println("# -*- mode:unix-assembly -*-");
  for (int i = 0; files[i]; i++) {
    println("\t.file\t%d %`'s", files[i]->file_no, files[i]->name);
  }
  assign_lvar_offsets(prog);
  emit_staticasms(staticasms);
  emit_data(prog);
  emit_text(prog);
  flushln();
}
