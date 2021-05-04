/* Automatically generated.  Do not edit */
/* See the tool/mkopcodeh.tcl script for details */
#define OP_Noop      0
#define OP_Explain   1
#define OP_Abortable 2

/* Properties such as "out2" or "jump" that are specified in
** comments following the "case" for each opcode in the vdbe.c
** are encoded into bitvectors as follows:
*/
#define OPFLG_JUMP 0x01 /* jump:  P2 holds jmp target */
#define OPFLG_IN1  0x02 /* in1:   P1 is an input */
#define OPFLG_IN2  0x04 /* in2:   P2 is an input */
#define OPFLG_IN3  0x08 /* in3:   P3 is an input */
#define OPFLG_OUT2 0x10 /* out2:  P2 is an output */
#define OPFLG_OUT3 0x20 /* out3:  P3 is an output */
#define OPFLG_INITIALIZER \
  { /*   0 */             \
    0x00, 0x00, 0x00,     \
  }

/* The sqlite3P2Values() routine is able to run faster if it knows
** the value of the largest JUMP opcode.  The smaller the maximum
** JUMP opcode the better, so the mkopcodeh.tcl script that
** generated this include file strives to group all JUMP opcodes
** together near the beginning of the list.
*/
#define SQLITE_MX_JUMP_OPCODE -1 /* Maximum JUMP opcode */
