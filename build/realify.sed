#-*-mode:sed;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: et ft=sed ts=8 sw=8 fenc=utf-8 :vi ────────────────┘
#
# SYNOPSIS
#
#   sed -i -f build/realify.sed foo.s
#
# OVERVIEW
#
#   This converts ints and longs to shorts while preserving System V ABI
#   x86_64 compatibility. This works better than gcc -m16 because we can
#   avoid the ASZ and OSZ prefixes in most cases while also avoiding the
#   legacy 32-bit calling conventions.

# remove comments
s/[ \t][ \t]*#.*//

s/leave\(q\|\)/leavew/
s/call\(q\|\)/callw/
s/ret\(q\|\)/retw/
s/popq\t%rbp/pop\t%bp/
s/pushq\t%rbp/push\t%bp/
s/pushq\t\(.*\)/sub\t$6,%sp\n\tpush\t\1/
s/popq\t\(.*\)/pop\t\1\n\tadd\t$6,%sp/

# # preserve hardcoded stack offsets
# # bloats code size 13%
# s/leave\(q\|\)/leavew\n\tadd\t$6,%sp/
# s/call\(q\|\)\t/sub\t$6,%sp\n\tcallw\t/
# s/ret\(q\|\)/retw\t$6/
# s/pushq\t\(.*\)/sub\t$6,%sp\n\tpush\t\1/
# s/popq\t\(.*\)/pop\t\1\n\tadd\t$6,%sp/

s/, /,/g

# 32-bitify
s/rax/eax/g
s/rbx/ebx/g
s/rcx/ecx/g
s/rdx/edx/g
s/rbp/ebp/g
s/rdi/edi/g
s/rsi/esi/g
s/rsp/esp/g

# unextension
s/movswl/mov/
s/movzwl/mov/
s/movslq/mov/
s/movzlq/mov/
s/movsbl/movsbw/

# unsuffix
s/^\(\t\(fild\|fist\|fistp\|fiadd\|fisub\|fisubr\|fimul\|fidiv\|fidivr\|ficom\)\)q\t/\1\t/
s/^\(\t\(mov\|add\|adc\|cmp\|test\|lea\|sbb\|mul\|imul\|div\|idiv\|in\|out\|xor\|sub\|and\|or\|rol\|ror\|rcl\|rcr\|shl\|shr\|sal\|sar\|inc\|dec\|not\|neg\)\)l\t/\1w\t/
s/^\(\t[a-z]*\)q\t/\1w\t/
s/movsww/mov/

# remove fluff
s/mov\t%eax,%eax//
s/mov\t%ebx,%ebx//
s/mov\t%ecx,%ecx//
s/mov\t%edx,%edx//
s/mov\t%ebp,%ebp//
s/mov\t%edi,%edi//
s/mov\t%esi,%esi//
s/mov\t%esp,%esp//

# make pic absolute
s/(%rip)//

# legal real mode modrm
s/(%ebx)/(%bx)/
s/(%edi)/(%di)/
s/(%esi)/(%si)/
s/(%ebp)/(%bp)/
s/(%ebx,%esi\(,1\|\))/(%bx,%si)/
s/(%ebx,%edi\(,1\|\))/(%bx,%di)/
s/(%ebp,%esi\(,1\|\))/(%bp,%si)/
s/(%ebp,%edi\(,1\|\))/(%bp,%di)/

# we need the asz prefix
s/(%eax,%eax/(%EAX,%EAX/
s/(%eax,%ebp/(%EAX,%EBP/
s/(%eax,%ebx/(%EAX,%EBX/
s/(%eax,%ecx/(%EAX,%ECX/
s/(%eax,%edi/(%EAX,%EDI/
s/(%eax,%edx/(%EAX,%EDX/
s/(%eax,%esi/(%EAX,%ESI/
s/(%ebp,%eax/(%EBP,%EAX/
s/(%ebp,%ebp/(%EBP,%EBP/
s/(%ebp,%ebx/(%EBP,%EBX/
s/(%ebp,%ecx/(%EBP,%ECX/
s/(%ebp,%edi/(%EBP,%EDI/
s/(%ebp,%edx/(%EBP,%EDX/
s/(%ebp,%esi/(%EBP,%ESI/
s/(%ebx,%eax/(%EBX,%EAX/
s/(%ebx,%ebp/(%EBX,%EBP/
s/(%ebx,%ebx/(%EBX,%EBX/
s/(%ebx,%ecx/(%EBX,%ECX/
s/(%ebx,%edi/(%EBX,%EDI/
s/(%ebx,%edx/(%EBX,%EDX/
s/(%ebx,%esi/(%EBX,%ESI/
s/(%ecx,%eax/(%ECX,%EAX/
s/(%ecx,%ebp/(%ECX,%EBP/
s/(%ecx,%ebx/(%ECX,%EBX/
s/(%ecx,%ecx/(%ECX,%ECX/
s/(%ecx,%edi/(%ECX,%EDI/
s/(%ecx,%edx/(%ECX,%EDX/
s/(%ecx,%esi/(%ECX,%ESI/
s/(%edi,%eax/(%EDI,%EAX/
s/(%edi,%ebp/(%EDI,%EBP/
s/(%edi,%ebx/(%EDI,%EBX/
s/(%edi,%ecx/(%EDI,%ECX/
s/(%edi,%edi/(%EDI,%EDI/
s/(%edi,%edx/(%EDI,%EDX/
s/(%edi,%esi/(%EDI,%ESI/
s/(%edx,%eax/(%EDX,%EAX/
s/(%edx,%ebp/(%EDX,%EBP/
s/(%edx,%ebx/(%EDX,%EBX/
s/(%edx,%ecx/(%EDX,%ECX/
s/(%edx,%edi/(%EDX,%EDI/
s/(%edx,%edx/(%EDX,%EDX/
s/(%edx,%esi/(%EDX,%ESI/
s/(%esi,%eax/(%ESI,%EAX/
s/(%esi,%ebp/(%ESI,%EBP/
s/(%esi,%ebx/(%ESI,%EBX/
s/(%esi,%ecx/(%ESI,%ECX/
s/(%esi,%edi/(%ESI,%EDI/
s/(%esi,%edx/(%ESI,%EDX/
s/(%esi,%esi/(%ESI,%ESI/
s/(%esp,%eax/(%ESP,%EAX/
s/(%esp,%ebp/(%ESP,%EBP/
s/(%esp,%ebx/(%ESP,%EBX/
s/(%esp,%ecx/(%ESP,%ECX/
s/(%esp,%edi/(%ESP,%EDI/
s/(%esp,%edx/(%ESP,%EDX/
s/(%esp,%esi/(%ESP,%ESI/
s/(,%eax/(,%EAX/
s/(,%ebx/(,%EBX/
s/(,%ecx/(,%ECX/
s/(,%edx/(,%EDX/
s/(,%esi/(,%ESI/
s/(,%edi/(,%EDI/
s/(,%ebp/(,%EBP/
s/(%eax)/(%EAX)/
s/(%ecx)/(%ECX)/
s/(%edx)/(%EDX)/
s/(%esp)/(%ESP)/

# 16bitify
s/eax/ax/g
s/ebx/bx/g
s/ecx/cx/g
s/edx/dx/g
s/ebp/bp/g
s/edi/di/g
s/esi/si/g
s/esp/sp/g

# sigh :\
# gcc needs a flag for not using rex byte regs. workaround:
# - %dil can be avoided through copious use of STOS() macro
# - %sil can be avoided through copious use of LODS() macro
# - %bpl shouldn't be allocated due to -fno-omit-frame-pointer
# - %spl shouldn't be allocated like ever
# beyond that there's only a few cases where %dil and %sil
# need some handcoded asm() macros to workaround, for example
# if ARG1 is long and you say (ARG1 & 1) gcc will use %dil
# so just kludge it using asm("and\t$1,%0" : "+Q"(ARG1))
#s/dil/bl/g
#s/sil/bh/g
#s/spl/bl/g
#s/bpl/bh/g

# nope
s/cltq//
