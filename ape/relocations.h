#ifndef COSMOPOLITAN_APE_RELOCATIONS_H_
#define COSMOPOLITAN_APE_RELOCATIONS_H_
/*─────────────────────────────────────────────────────────────────────────────╗
│ αcτµαlly pδrταblε εxεcµταblε § relocations                                   │
╚──────────────────────────────────────────────────────────────────────────────╝
  One of the things αcτµαlly pδrταblε εxεcµταblε does a good job
  abstracting, is how a program needs to exist at three addresses
  simultaneously during the early stages of the loading process.

  By default, the linker calculates all symbols using virtual addresses.
  In some cases it's necessary to use addend macros that change virtual
  addresses into the other two types: physical and real. */

#define IMAGE_BASE_REAL 0x2000

#ifndef IMAGE_BASE_VIRTUAL
#define IMAGE_BASE_VIRTUAL 0x400000
#endif

#ifndef IMAGE_BASE_PHYSICAL
#define IMAGE_BASE_PHYSICAL 0x100000
#endif

/**
 * Returns Relative Virtual Address.
 */
#define RVA(x) ((x) - (IMAGE_BASE_VIRTUAL))

/**
 * Adjusts virtual address so it's relative to load address.
 */
#define PHYSICAL(x) ((x) - (IMAGE_BASE_VIRTUAL - IMAGE_BASE_PHYSICAL))

/**
 * Makes high-entropy read-only addresses relocatable in real mode.
 */
#define REAL(x) ((x) - (IMAGE_BASE_VIRTUAL - IMAGE_BASE_REAL))

#if IMAGE_BASE_VIRTUAL % 0x1000 != 0
#error "IMAGE_BASE_VIRTUAL must be 4kb aligned"
#endif
#if IMAGE_BASE_PHYSICAL % 0x1000 != 0
#error "IMAGE_BASE_PHYSICAL must be 4kb aligned"
#endif
#if IMAGE_BASE_REAL % 0x1000 != 0
#error "IMAGE_BASE_REAL must be 4kb aligned"
#endif

#endif /* COSMOPOLITAN_APE_RELOCATIONS_H_ */
