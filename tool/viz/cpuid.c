/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/cpuid4.internal.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/x86idnames.h"

#define CANIUSE(FEATURE) caniuse(#FEATURE, X86_HAVE(FEATURE))
#define SHOW(CONSTANT)   show(#CONSTANT, CONSTANT)

static void caniuse(const char *feature, bool present) {
  printf("%-20s%s%s%s\n", feature, present ? GREEN : RED,
         present ? "present" : "unavailable", RESET);
}

static void show(const char *constant, long value) {
  printf("%-20s%#lx\n", constant, value);
}

static void showvendor(void) {
  printf("%.*s%.*s%.*s", 4, &KCPUIDS(0H, EBX), 4, &KCPUIDS(0H, EDX), 4,
         &KCPUIDS(0H, ECX));
}

static void showmodel(void) {
  if (getx86processormodel(kX86ProcessorModelKey)) {
    printf(" %s",
           findnamebyid(kX86MarchNames,
                        getx86processormodel(kX86ProcessorModelKey)->march));
  }
}

static void showspeed(void) {
  if (KCPUIDS(16H, EAX)) {
    printf(" %.1f%s", KCPUIDS(16H, EAX) / 1000.0, "ghz");
  }
}

static void showstrata(void) {
  if (getx86processormodel(kX86ProcessorModelKey)) {
    printf(" (%s %s)",
           findnamebyid(kX86GradeNames,
                        getx86processormodel(kX86ProcessorModelKey)->grade),
           "Grade");
  }
}

void showcachesizes(void) {
  unsigned i;
  CPUID4_ITERATE(i, {
    printf("%-19s%s%s %u-way %,7u byte cache w/%s %,5u sets of %u byte lines "
           "shared across %u threads\n",
           gc(xasprintf("Level %u%s", CPUID4_CACHE_LEVEL,
                        CPUID4_CACHE_TYPE == 1
                            ? " data"
                            : CPUID4_CACHE_TYPE == 2 ? " code" : "")),
           CPUID4_IS_FULLY_ASSOCIATIVE ? " fully-associative" : "",
           CPUID4_COMPLEX_INDEXING ? " complexly-indexed" : "",
           CPUID4_WAYS_OF_ASSOCIATIVITY, CPUID4_CACHE_SIZE_IN_BYTES,
           CPUID4_PHYSICAL_LINE_PARTITIONS > 1
               ? gc(xasprintf(" %u physically partitioned"))
               : "",
           CPUID4_NUMBER_OF_SETS, CPUID4_SYSTEM_COHERENCY_LINE_SIZE,
           CPUID4_MAX_THREADS_SHARING_CACHE);
  });
}

int main(int argc, char *argv[]) {
  long tsc_aux;

  showvendor();
  showmodel();
  showspeed();
  showstrata();
  printf("\n");

  if (X86_HAVE(HYPERVISOR)) {
    unsigned eax, ebx, ecx, edx;
    asm("push\t%%rbx\n\t"
        "cpuid\n\t"
        "mov\t%%ebx,%1\n\t"
        "pop\t%%rbx"
        : "=a"(eax), "=rm"(ebx), "=c"(ecx), "=d"(edx)
        : "0"(0x40000000), "2"(0));
    printf("Running inside %.4s%.4s%.4s (eax=%#x)\n", &ebx, &ecx, &edx, eax);
  }

  printf("\n");
  SHOW(kX86CpuStepping);
  SHOW(kX86CpuModelid);
  SHOW(kX86CpuFamilyid);
  SHOW(kX86CpuType);
  SHOW(kX86CpuExtmodelid);
  SHOW(kX86CpuExtfamilyid);

  printf("\n");
  tsc_aux = rdpid();
  show("TSC_AUX", tsc_aux);
  show(" → core", TSC_AUX_CORE(tsc_aux));
  show(" → node", TSC_AUX_NODE(tsc_aux));

  printf("\n");
  printf("Caches\n");
  printf("──────\n");
  showcachesizes();

  printf("\n");
  printf("Features\n");
  printf("────────\n");
  CANIUSE(ACC);
  CANIUSE(ACPI);
  CANIUSE(ADX);
  CANIUSE(AES);
  CANIUSE(APIC);
  CANIUSE(ARCH_CAPABILITIES);
  CANIUSE(AVX);

  printf("%-20s%s%s%s%s\n", "AVX2", X86_HAVE(AVX2) ? GREEN : RED,
         X86_HAVE(AVX2) ? "present" : "unavailable", RESET,
         (!X86_HAVE(AVX2) && ({
           unsigned eax, ebx, ecx, edx;
           asm("push\t%%rbx\n\t"
               "cpuid\n\t"
               "mov\t%%ebx,%1\n\t"
               "pop\t%%rbx"
               : "=a"(eax), "=rm"(ebx), "=c"(ecx), "=d"(edx)
               : "0"(7), "2"(0));
           (void)eax;
           (void)ecx;
           (void)edx;
           !!(ebx & (1u << 5));
         }))
             ? " (disabled by operating system)"
             : "");

  CANIUSE(AVX512BW);
  CANIUSE(AVX512CD);
  CANIUSE(AVX512DQ);
  CANIUSE(AVX512ER);
  CANIUSE(AVX512F);
  CANIUSE(AVX512IFMA);
  CANIUSE(AVX512PF);
  CANIUSE(AVX512VBMI);
  CANIUSE(AVX512VL);
  CANIUSE(AVX512_4FMAPS);
  CANIUSE(AVX512_4VNNIW);
  CANIUSE(AVX512_BF16);
  CANIUSE(AVX512_BITALG);
  CANIUSE(AVX512_VBMI2);
  CANIUSE(AVX512_VNNI);
  CANIUSE(AVX512_VP2INTERSECT);
  CANIUSE(AVX512_VPOPCNTDQ);
  CANIUSE(BMI);
  CANIUSE(BMI2);
  CANIUSE(CID);
  CANIUSE(CLDEMOTE);
  CANIUSE(CLFLUSH);
  CANIUSE(CLFLUSHOPT);
  CANIUSE(CLWB);
  CANIUSE(CMOV);
  CANIUSE(CQM);
  CANIUSE(CX16);
  CANIUSE(CX8);
  CANIUSE(DCA);
  CANIUSE(DE);
  CANIUSE(DS);
  CANIUSE(DSCPL);
  CANIUSE(DTES64);
  CANIUSE(ERMS);
  CANIUSE(EST);
  CANIUSE(F16C);
  CANIUSE(FDP_EXCPTN_ONLY);
  CANIUSE(FLUSH_L1D);
  CANIUSE(FMA);
  CANIUSE(FPU);
  CANIUSE(FSGSBASE);
  CANIUSE(FXSR);
  CANIUSE(GBPAGES);
  CANIUSE(GFNI);
  CANIUSE(HLE);
  CANIUSE(HT);
  CANIUSE(HYPERVISOR);
  CANIUSE(IA64);
  CANIUSE(INTEL_PT);
  CANIUSE(INTEL_STIBP);
  CANIUSE(INVPCID);
  CANIUSE(LA57);
  CANIUSE(LM);
  CANIUSE(MCA);
  CANIUSE(MCE);
  CANIUSE(MD_CLEAR);
  CANIUSE(MMX);
  CANIUSE(MOVBE);
  CANIUSE(MOVDIR64B);
  CANIUSE(MOVDIRI);
  CANIUSE(MP);
  CANIUSE(MPX);
  CANIUSE(MSR);
  CANIUSE(MTRR);
  CANIUSE(MWAIT);
  CANIUSE(NX);
  CANIUSE(OSPKE);
  CANIUSE(OSXSAVE);
  CANIUSE(PAE);
  CANIUSE(PAT);
  CANIUSE(PBE);
  CANIUSE(PCID);
  CANIUSE(PCLMUL);
  CANIUSE(PCONFIG);
  CANIUSE(PDCM);
  CANIUSE(PGE);
  CANIUSE(PKU);
  CANIUSE(PN);
  CANIUSE(POPCNT);
  CANIUSE(PSE);
  CANIUSE(PSE36);
  CANIUSE(RDPID);
  CANIUSE(RDRND);
  CANIUSE(RDSEED);
  CANIUSE(RDTSCP);
  CANIUSE(RDT_A);
  CANIUSE(RTM);
  CANIUSE(SDBG);
  CANIUSE(SELFSNOOP);
  CANIUSE(SEP);
  CANIUSE(SHA);
  CANIUSE(SMAP);
  CANIUSE(SMEP);
  CANIUSE(SMX);
  CANIUSE(SPEC_CTRL);
  CANIUSE(SPEC_CTRL_SSBD);
  CANIUSE(SSE);
  CANIUSE(SSE2);
  CANIUSE(SSE3);
  CANIUSE(SSE4_1);
  CANIUSE(SSE4_2);
  CANIUSE(SSSE3);
  CANIUSE(SYSCALL);
  CANIUSE(TM2);
  CANIUSE(TME);
  CANIUSE(TSC);
  CANIUSE(TSC_ADJUST);
  CANIUSE(TSC_DEADLINE_TIMER);
  CANIUSE(TSX_FORCE_ABORT);
  CANIUSE(UMIP);
  CANIUSE(VAES);
  CANIUSE(VME);
  CANIUSE(VMX);
  CANIUSE(VPCLMULQDQ);
  CANIUSE(WAITPKG);
  CANIUSE(X2APIC);
  CANIUSE(XSAVE);
  CANIUSE(XTPR);
  CANIUSE(ZERO_FCS_FDS);

  printf("\n");
  printf("AMD Stuff\n");
  printf("─────────\n");
  CANIUSE(3DNOW);
  CANIUSE(3DNOWEXT);
  CANIUSE(3DNOWPREFETCH);
  CANIUSE(ABM);
  CANIUSE(BPEXT);
  CANIUSE(CMP_LEGACY);
  CANIUSE(CR8_LEGACY);
  CANIUSE(EXTAPIC);
  CANIUSE(FMA4);
  CANIUSE(FXSR_OPT);
  CANIUSE(IBS);
  CANIUSE(LAHF_LM);
  CANIUSE(LWP);
  CANIUSE(MISALIGNSSE);
  CANIUSE(MMXEXT);
  CANIUSE(MWAITX);
  CANIUSE(NODEID_MSR);
  CANIUSE(OSVW);
  CANIUSE(OVERFLOW_RECOV);
  CANIUSE(PERFCTR_CORE);
  CANIUSE(PERFCTR_LLC);
  CANIUSE(PERFCTR_NB);
  CANIUSE(PTSC);
  CANIUSE(SKINIT);
  CANIUSE(SMCA);
  CANIUSE(SSE4A);
  CANIUSE(SUCCOR);
  CANIUSE(SVM);
  CANIUSE(TBM);
  CANIUSE(TCE);
  CANIUSE(TOPOEXT);
  CANIUSE(WDT);
  CANIUSE(XOP);

  return 0;
}
