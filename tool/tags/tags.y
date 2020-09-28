/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
%include {
#include "libc/stdio/stdio.h"
#include "tool/tags/tags.h"
#include "libc/calls/calls.h"
#include "libc/str/str.h"
#include "third_party/dtoa/dtoa.h"
#include "libc/x/x.h"
#include "libc/runtime/gc.h"
#include "libc/math.h"
}

/**
 * C parser.
 *   
 *   “The grammar of C seems remarkably well-behaved. It is almost
 *    LALR(1). More precisely, the automaton built for C by an LALR(1)
 *    parser generator exhibits only two conflicts: (1) a shift/reduce
 *    conflict caused by dangling else ambiguity and (2) a shift/reduce
 *    conflict caused by the _Atomic( ambiguity. In both cases, the
 *    desired behavior can be obtained by instructing the parser to
 *    prefer shifting.” Quoth Jacques-Henri Jourdan & François Pottier
 * 
 */ 

%token_prefix TK_
%token_type {struct Token}
%type expr {long double}
%syntax_error { SyntaxError(); }

%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN.
%token FLOAT FOR GOTO IF INLINE INT LONG REGISTER RESTRICT RETURN SHORT SIGNED.
%token SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE.
%token ALIGNAS ALIGNOF ATOMIC BOOL COMPLEX GENERIC IMAGINARY NORETURN INCLUDE.
%token STATIC_ASSERT THREAD_LOCAL ELIF ENDIF IFDEF IFNDEF DEFINE UNDEF PRAGMA.
%token LINE ERROR SYMBOL ATTRIBUTE RESTRICT TYPEOF TYPEOF INLINE CONST LABEL.
%token FORCE_ALIGN_ARG_POINTER ALWAYS_INLINE GNU_INLINE ALIGNOF ASM AUTO_TYPE.
%token BYTE COMPLEX IMAG MAY_ALIAS NORETURN PACKED POINTER PRINTF REAL SCANF.
%token STRFMON STRFTIME STRONG TARGET TRANSPARENT_UNION VOLATILE WORD ALIAS.
%token ALIGNED ALLOC_ALIGN ALLOC_SIZE ARTIFICIAL ASSUME_ALIGNED COLD.
%token CONSTRUCTOR DESTRUCTOR COPY DEPRECATED ERROR WARNING EXTERNALLY_VISIBLE.
%token FLATTEN FORMAT GNU_FORMAT GNU_PRINTF GNU_SCANF FORMAT_ARG HOT IFUNC.
%token INTERRUPT INTERRUPT_HANDLER NO_CALLER_SAVED_REGISTERS LEAF MALLOC NO_ICF.
%token NO_INSTRUMENT_FUNCTION NO_PROFILE_INSTRUMENT_FUNCTION NO_REORDER.
%token NO_SANITIZE NO_SANITIZE_ADDRESS NO_ADDRESS_SAFETY_ANALYSIS.
%token NO_SANITIZE_THREAD NO_SANITIZE_UNDEFINED NO_SPLIT_STACK NO_STACK_LIMIT.
%token NOCLONE NOIPA NONNULL NOPLT NOTHROW OPTIMIZE PURE.
%token PATCHABLE_FUNCTION_ENTRY RETURNS_NONNULL RETURNS_TWICE SECTION SENTINEL.
%token SIMD TARGET_CLONES UNUSED USED VISIBILITY WARN_UNUSED_RESULT.
%token PARAMS_NONNULL WEAK VECTOR_SIZE MS_ABI MODE OPTNONE NODEBUG.

%left IF.
%left ELSE.
%right COMMA.
%right EQ ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN REM_ASSIGN.
%right SHL_ASSIGN SHR_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN.
%right QUESTION COLON.
%left OR_LOGICAL.
%left AND_LOGICAL.
%left OR.
%left XOR.
%left AND.
%left EQUAL NOTEQUAL.
%left LT LE GT GE.
%left SHL SHR.
%left ADD SUB.
%left STAR DIV REM.
%right TILDE EXCLAIM INC DEC AMP SIZEOF ALIGNOF.
%left DOT ARROW LP RP RSB LSB.

program ::= translation_unit.

translation_unit ::= external_declaration.
translation_unit ::= translation_unit external_declaration.

external_declaration ::= function_definition.
external_declaration ::= declaration.

function_definition ::= declaration_specifiers declarator declaration_list compound_statement.
function_definition ::= declaration_specifiers declarator compound_statement.

declaration_list ::= declaration.
declaration_list ::= declaration_list declaration.

jump_statement ::= GOTO IDENTIFIER SEMI.
jump_statement ::= CONTINUE SEMI.
jump_statement ::= BREAK SEMI.
jump_statement ::= RETURN SEMI.
jump_statement ::= RETURN expression SEMI.

iteration_statement ::= WHILE LP expression RP statement.
iteration_statement ::= DO statement WHILE LP expression RP SEMI.
iteration_statement ::= FOR LP expression_statement expression_statement RP statement.
iteration_statement ::= FOR LP expression_statement expression_statement expression RP statement.
iteration_statement ::= FOR LP declaration expression_statement RP statement.
iteration_statement ::= FOR LP declaration expression_statement expression RP statement.

selection_statement ::= IF LP expression RP statement ELSE statement.
selection_statement ::= IF LP expression RP statement.
selection_statement ::= SWITCH LP expression RP statement.

expression_statement ::= SEMI.
expression_statement ::= expression SEMI.

block_item ::= declaration.
block_item ::= statement.

block_item_list ::= block_item.
block_item_list ::= block_item_list block_item.

compound_statement ::= LCB RCB.
compound_statement ::= LCB block_item_list RCB.

labeled_statement ::= IDENTIFIER COLON statement.
labeled_statement ::= CASE constant_expression COLON statement.
labeled_statement ::= DEFAULT COLON statement.

statement ::= labeled_statement.
statement ::= compound_statement.
statement ::= expression_statement.
statement ::= selection_statement.
statement ::= iteration_statement.
statement ::= jump_statement.

static_assert_declaration ::= STATIC_ASSERT LP constant_expression COMMA STRING_LITERAL RP SEMI.

designator ::= LSB constant_expression RSB.
designator ::= DOT IDENTIFIER.

designator_list ::= designator.
designator_list ::= designator_list designator.

designation ::= designator_list EQ.

initializer_list ::= designation initializer.
initializer_list ::= initializer.
initializer_list ::= initializer_list COMMA designation initializer.
initializer_list ::= initializer_list COMMA initializer.

initializer ::= LCB initializer_list RCB.
initializer ::= LCB initializer_list COMMA RCB.
initializer ::= assignment_expression.

direct_abstract_declarator ::= LP abstract_declarator RP.
direct_abstract_declarator ::= LSB RSB.
direct_abstract_declarator ::= LSB STAR RSB.
direct_abstract_declarator ::= LSB STATIC type_qualifier_list assignment_expression RSB.
direct_abstract_declarator ::= LSB STATIC assignment_expression RSB.
direct_abstract_declarator ::= LSB type_qualifier_list STATIC assignment_expression RSB.
direct_abstract_declarator ::= LSB type_qualifier_list assignment_expression RSB.
direct_abstract_declarator ::= LSB type_qualifier_list RSB.
direct_abstract_declarator ::= LSB assignment_expression RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB STAR RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB STATIC type_qualifier_list assignment_expression RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB STATIC assignment_expression RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB type_qualifier_list assignment_expression RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB type_qualifier_list STATIC assignment_expression RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB type_qualifier_list RSB.
direct_abstract_declarator ::= direct_abstract_declarator LSB assignment_expression RSB.
direct_abstract_declarator ::= LP RP.
direct_abstract_declarator ::= LP parameter_type_list RP.
direct_abstract_declarator ::= direct_abstract_declarator LP RP.
direct_abstract_declarator ::= direct_abstract_declarator LP parameter_type_list RP.

abstract_declarator ::= pointer direct_abstract_declarator.
abstract_declarator ::= pointer.
abstract_declarator ::= direct_abstract_declarator.

type_name ::= specifier_qualifier_list abstract_declarator.
type_name ::= specifier_qualifier_list.

primary_expression ::= IDENTIFIER.
primary_expression ::= constant.
primary_expression ::= string.
primary_expression ::= LP expression RP.
primary_expression ::= generic_selection.

constant ::= I_CONSTANT.
constant ::= F_CONSTANT.
constant ::= ENUMERATION_CONSTANT.

enumeration_constant ::= IDENTIFIER.

string ::= STRING_LITERAL.
string ::= FUNC_NAME.

generic_selection ::= GENERIC LP assignment_expression COMMA generic_assoc_list RP.

generic_assoc_list ::= generic_association.
generic_assoc_list ::= generic_assoc_list COMMA generic_association.

generic_association ::= type_name COLON assignment_expression.
generic_association ::= DEFAULT COLON assignment_expression.

postfix_expression ::= primary_expression.
postfix_expression ::= postfix_expression LSB expression RSB.
postfix_expression ::= postfix_expression LP RP.
postfix_expression ::= postfix_expression LP argument_expression_list RP.
postfix_expression ::= postfix_expression DOT IDENTIFIER.
postfix_expression ::= postfix_expression ARROW IDENTIFIER.
postfix_expression ::= postfix_expression INC.
postfix_expression ::= postfix_expression DEC.
postfix_expression ::= LP type_name RP LCB initializer_list RCB.
postfix_expression ::= LP type_name RP LCB initializer_list COMMA RCB.

argument_expression_list ::= assignment_expression.
argument_expression_list ::= argument_expression_list COMMA assignment_expression.

unary_expression ::= postfix_expression.
unary_expression ::= INC unary_expression.
unary_expression ::= DEC unary_expression.
unary_expression ::= unary_operator cast_expression.
unary_expression ::= SIZEOF unary_expression.
unary_expression ::= SIZEOF LP type_name RP.
unary_expression ::= ALIGNOF LP type_name RP.

unary_operator ::= AMP.
unary_operator ::= STAR.
unary_operator ::= ADD.
unary_operator ::= SUB.
unary_operator ::= TILDE.
unary_operator ::= EXCLAIM.

cast_expression ::= unary_expression.
cast_expression ::= LP type_name RP cast_expression.

multiplicative_expression ::= cast_expression.
multiplicative_expression ::= multiplicative_expression STAR cast_expression.
multiplicative_expression ::= multiplicative_expression DIV cast_expression.
multiplicative_expression ::= multiplicative_expression REM cast_expression.

additive_expression ::= multiplicative_expression.
additive_expression ::= additive_expression ADD multiplicative_expression.
additive_expression ::= additive_expression SUB multiplicative_expression.

shift_expression ::= additive_expression.
shift_expression ::= shift_expression SHL additive_expression.
shift_expression ::= shift_expression SHR additive_expression.

relational_expression ::= shift_expression.
relational_expression ::= relational_expression LT shift_expression.
relational_expression ::= relational_expression GT shift_expression.
relational_expression ::= relational_expression LE shift_expression.
relational_expression ::= relational_expression GE shift_expression.

equality_expression ::= relational_expression.
equality_expression ::= equality_expression EQUAL relational_expression.
equality_expression ::= equality_expression NOTEQUAL relational_expression.

and_expression ::= equality_expression.
and_expression ::= and_expression AMP equality_expression.

exclusive_or_expression ::= and_expression.
exclusive_or_expression ::= exclusive_or_expression XOR and_expression.
inclusive_or_expression ::= exclusive_or_expression.
inclusive_or_expression ::= inclusive_or_expression OR exclusive_or_expression.

logical_and_expression ::= inclusive_or_expression.
logical_and_expression ::= logical_and_expression AND_LOGICAL inclusive_or_expression.

logical_or_expression ::= logical_and_expression.
logical_or_expression ::= logical_or_expression OR_LOGICAL logical_and_expression.

conditional_expression ::= logical_or_expression.
conditional_expression ::= logical_or_expression QUESTION expression COLON conditional_expression.

assignment_expression ::= conditional_expression.
assignment_expression ::= unary_expression assignment_operator assignment_expression.

assignment_operator ::= EQ.
assignment_operator ::= MUL_ASSIGN.
assignment_operator ::= DIV_ASSIGN.
assignment_operator ::= REM_ASSIGN.
assignment_operator ::= ADD_ASSIGN.
assignment_operator ::= SUB_ASSIGN.
assignment_operator ::= SHL_ASSIGN.
assignment_operator ::= SHR_ASSIGN.
assignment_operator ::= AND_ASSIGN.
assignment_operator ::= XOR_ASSIGN.
assignment_operator ::= OR_ASSIGN.

expression ::= assignment_expression.
expression ::= expression COMMA assignment_expression.

constant_expression ::= conditional_expression. /* with constraints */

declaration ::= declaration_specifiers SEMI.
declaration ::= declaration_specifiers init_declarator_list SEMI.
declaration ::= static_assert_declaration.

declaration_specifiers ::= storage_class_specifier declaration_specifiers.
declaration_specifiers ::= storage_class_specifier.
declaration_specifiers ::= type_specifier declaration_specifiers.
declaration_specifiers ::= type_specifier.
declaration_specifiers ::= type_qualifier declaration_specifiers.
declaration_specifiers ::= type_qualifier.
declaration_specifiers ::= function_specifier declaration_specifiers.
declaration_specifiers ::= function_specifier.
declaration_specifiers ::= alignment_specifier declaration_specifiers.
declaration_specifiers ::= alignment_specifier.

init_declarator_list ::= init_declarator.
init_declarator_list ::= init_declarator_list COMMA init_declarator.

init_declarator ::= declarator EQ initializer.
init_declarator ::= declarator.

storage_class_specifier ::= TYPEDEF. /* identifiers must be flagged as TYPEDEF_NAME */
storage_class_specifier ::= EXTERN.
storage_class_specifier ::= STATIC.
storage_class_specifier ::= THREAD_LOCAL.
storage_class_specifier ::= AUTO.
storage_class_specifier ::= REGISTER.

type_specifier ::= VOID.
type_specifier ::= CHAR.
type_specifier ::= SHORT.
type_specifier ::= INT.
type_specifier ::= LONG.
type_specifier ::= FLOAT.
type_specifier ::= DOUBLE.
type_specifier ::= SIGNED.
type_specifier ::= UNSIGNED.
type_specifier ::= BOOL.
type_specifier ::= COMPLEX.
type_specifier ::= IMAGINARY. /* non-mandated extension */
type_specifier ::= struct_or_union_specifier.
type_specifier ::= enum_specifier.
type_specifier ::= TYPEDEF_NAME. /* after it has been defined as such */

struct_or_union_specifier ::= struct_or_union LCB struct_declaration_list RCB.
struct_or_union_specifier ::= struct_or_union IDENTIFIER LCB struct_declaration_list RCB.
struct_or_union_specifier ::= struct_or_union IDENTIFIER.

struct_or_union ::= STRUCT.
struct_or_union ::= UNION.

struct_declaration_list ::= struct_declaration.
struct_declaration_list ::= struct_declaration_list struct_declaration.

struct_declaration ::= specifier_qualifier_list SEMI. /* for anonymous struct/union */
struct_declaration ::= specifier_qualifier_list struct_declarator_list SEMI.
struct_declaration ::= static_assert_declaration.

specifier_qualifier_list ::= type_specifier specifier_qualifier_list.
specifier_qualifier_list ::= type_specifier.
specifier_qualifier_list ::= type_qualifier specifier_qualifier_list.
specifier_qualifier_list ::= type_qualifier.

struct_declarator_list ::= struct_declarator.
struct_declarator_list ::= struct_declarator_list COMMA struct_declarator.

struct_declarator ::= COLON constant_expression.
struct_declarator ::= declarator COLON constant_expression.
struct_declarator ::= declarator.

enum_specifier ::= ENUM LCB enumerator_list RCB.
enum_specifier ::= ENUM LCB enumerator_list COMMA RCB.
enum_specifier ::= ENUM IDENTIFIER LCB enumerator_list RCB.
enum_specifier ::= ENUM IDENTIFIER LCB enumerator_list COMMA RCB.
enum_specifier ::= ENUM IDENTIFIER.

enumerator_list ::= enumerator.
enumerator_list ::= enumerator_list COMMA enumerator.

enumerator ::= enumeration_constant EQ constant_expression.
enumerator ::= enumeration_constant.

type_qualifier ::= CONST.
type_qualifier ::= RESTRICT.
type_qualifier ::= VOLATILE.
type_qualifier ::= ATOMIC.

function_specifier ::= INLINE.
function_specifier ::= NORETURN.

alignment_specifier ::= ALIGNAS LP type_name RP.
alignment_specifier ::= ALIGNAS LP constant_expression RP.

declarator ::= pointer direct_declarator.
declarator ::= direct_declarator.

direct_declarator ::= IDENTIFIER.
direct_declarator ::= LP declarator RP.
direct_declarator ::= direct_declarator LSB RSB.
direct_declarator ::= direct_declarator LSB STAR RSB.
direct_declarator ::= direct_declarator LSB STATIC type_qualifier_list assignment_expression RSB.
direct_declarator ::= direct_declarator LSB STATIC assignment_expression RSB.
direct_declarator ::= direct_declarator LSB type_qualifier_list STAR RSB.
direct_declarator ::= direct_declarator LSB type_qualifier_list STATIC assignment_expression RSB.
direct_declarator ::= direct_declarator LSB type_qualifier_list assignment_expression RSB.
direct_declarator ::= direct_declarator LSB type_qualifier_list RSB.
direct_declarator ::= direct_declarator LSB assignment_expression RSB.
direct_declarator ::= direct_declarator LP parameter_type_list RP.
direct_declarator ::= direct_declarator LP RP.
direct_declarator ::= direct_declarator LP identifier_list RP.

pointer ::= STAR type_qualifier_list pointer.
pointer ::= STAR type_qualifier_list.
pointer ::= STAR pointer.
pointer ::= STAR.

type_qualifier_list ::= type_qualifier.
type_qualifier_list ::= type_qualifier_list type_qualifier.

parameter_type_list ::= parameter_list COMMA ELLIPSIS.
parameter_type_list ::= parameter_list.

parameter_list ::= parameter_declaration.
parameter_list ::= parameter_list COMMA parameter_declaration.

parameter_declaration ::= declaration_specifiers declarator.
parameter_declaration ::= declaration_specifiers abstract_declarator.
parameter_declaration ::= declaration_specifiers.

identifier_list ::= IDENTIFIER.
identifier_list ::= identifier_list COMMA IDENTIFIER.
