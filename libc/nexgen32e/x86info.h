#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_X86INFO_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_X86INFO_H_
#include "libc/nexgen32e/kcpuids.h"

#define kX86CpuStepping    ((KCPUIDS(1H, EAX) >> 0) & 15)
#define kX86CpuModelid     ((KCPUIDS(1H, EAX) >> 4) & 15)
#define kX86CpuFamilyid    ((KCPUIDS(1H, EAX) >> 8) & 15)
#define kX86CpuType        ((KCPUIDS(1H, EAX) >> 12) & 3)
#define kX86CpuExtmodelid  ((KCPUIDS(1H, EAX) >> 16) & 15)
#define kX86CpuExtfamilyid ((KCPUIDS(1H, EAX) >> 20) & 255)

#define kX86CpuFamily \
  (kX86CpuFamilyid + (kX86CpuFamilyid == 15 ? kX86CpuExtfamilyid : 0))

#define kX86CpuModel                                                       \
  (kX86CpuModelid |                                                        \
   (kX86CpuFamilyid == 6 || kX86CpuFamilyid == 15 ? kX86CpuExtmodelid : 0) \
       << 4)

#define kX86ProcessorModelKey                                                 \
  (kX86CpuExtfamilyid << 12 | kX86CpuFamilyid << 8 | kX86CpuExtmodelid << 4 | \
   kX86CpuModelid)

#define X86_MARCH_UNKNOWN        0
#define X86_MARCH_CORE2          1
#define X86_MARCH_NEHALEM        2
#define X86_MARCH_WESTMERE       3
#define X86_MARCH_SANDYBRIDGE    4
#define X86_MARCH_IVYBRIDGE      5
#define X86_MARCH_HASWELL        6
#define X86_MARCH_BROADWELL      7
#define X86_MARCH_SKYLAKE        8
#define X86_MARCH_KABYLAKE       9
#define X86_MARCH_CANNONLAKE     10
#define X86_MARCH_ICELAKE        11
#define X86_MARCH_TIGERLAKE      12
#define X86_MARCH_BONNELL        13
#define X86_MARCH_SALTWELL       14
#define X86_MARCH_SILVERMONT     15
#define X86_MARCH_AIRMONT        16
#define X86_MARCH_GOLDMONT       17
#define X86_MARCH_GOLDMONTPLUS   18
#define X86_MARCH_TREMONT        19
#define X86_MARCH_KNIGHTSLANDING 20
#define X86_MARCH_KNIGHTSMILL    21
#define X86_MARCH_SAPPHIRERAPIDS 22
#define X86_MARCH_ALDERLAKE      23
#define X86_MARCH_COMETLAKE      24
#define X86_MARCH_RAPTORLAKE     25
#define X86_MARCH_ROCKETLAKE     26

#define X86_GRADE_UNKNOWN   0
#define X86_GRADE_APPLIANCE 1
#define X86_GRADE_MOBILE    2
#define X86_GRADE_TABLET    3
#define X86_GRADE_DESKTOP   4
#define X86_GRADE_CLIENT    5
#define X86_GRADE_DENSITY   6
#define X86_GRADE_SERVER    7
#define X86_GRADE_SCIENCE   8

struct X86ProcessorModel {
  short key;
  unsigned char march;
  unsigned char grade;
};

extern const size_t kX86ProcessorModelCount;
extern const struct X86ProcessorModel kX86ProcessorModels[];

const struct X86ProcessorModel *getx86processormodel(short) nosideeffect;

#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_X86INFO_H_ */
