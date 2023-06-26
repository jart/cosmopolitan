// clang-format off
#include "third_party/mold/elf/mold.h"

namespace mold::elf {

template <>
std::string rel_to_string<X86_64>(u32 r_type) {
  switch (r_type) {
  case R_X86_64_NONE: return "R_X86_64_NONE";
  case R_X86_64_64: return "R_X86_64_64";
  case R_X86_64_PC32: return "R_X86_64_PC32";
  case R_X86_64_GOT32: return "R_X86_64_GOT32";
  case R_X86_64_PLT32: return "R_X86_64_PLT32";
  case R_X86_64_COPY: return "R_X86_64_COPY";
  case R_X86_64_GLOB_DAT: return "R_X86_64_GLOB_DAT";
  case R_X86_64_JUMP_SLOT: return "R_X86_64_JUMP_SLOT";
  case R_X86_64_RELATIVE: return "R_X86_64_RELATIVE";
  case R_X86_64_GOTPCREL: return "R_X86_64_GOTPCREL";
  case R_X86_64_32: return "R_X86_64_32";
  case R_X86_64_32S: return "R_X86_64_32S";
  case R_X86_64_16: return "R_X86_64_16";
  case R_X86_64_PC16: return "R_X86_64_PC16";
  case R_X86_64_8: return "R_X86_64_8";
  case R_X86_64_PC8: return "R_X86_64_PC8";
  case R_X86_64_DTPMOD64: return "R_X86_64_DTPMOD64";
  case R_X86_64_DTPOFF64: return "R_X86_64_DTPOFF64";
  case R_X86_64_TPOFF64: return "R_X86_64_TPOFF64";
  case R_X86_64_TLSGD: return "R_X86_64_TLSGD";
  case R_X86_64_TLSLD: return "R_X86_64_TLSLD";
  case R_X86_64_DTPOFF32: return "R_X86_64_DTPOFF32";
  case R_X86_64_GOTTPOFF: return "R_X86_64_GOTTPOFF";
  case R_X86_64_TPOFF32: return "R_X86_64_TPOFF32";
  case R_X86_64_PC64: return "R_X86_64_PC64";
  case R_X86_64_GOTOFF64: return "R_X86_64_GOTOFF64";
  case R_X86_64_GOTPC32: return "R_X86_64_GOTPC32";
  case R_X86_64_GOT64: return "R_X86_64_GOT64";
  case R_X86_64_GOTPCREL64: return "R_X86_64_GOTPCREL64";
  case R_X86_64_GOTPC64: return "R_X86_64_GOTPC64";
  case R_X86_64_GOTPLT64: return "R_X86_64_GOTPLT64";
  case R_X86_64_PLTOFF64: return "R_X86_64_PLTOFF64";
  case R_X86_64_SIZE32: return "R_X86_64_SIZE32";
  case R_X86_64_SIZE64: return "R_X86_64_SIZE64";
  case R_X86_64_GOTPC32_TLSDESC: return "R_X86_64_GOTPC32_TLSDESC";
  case R_X86_64_TLSDESC_CALL: return "R_X86_64_TLSDESC_CALL";
  case R_X86_64_TLSDESC: return "R_X86_64_TLSDESC";
  case R_X86_64_IRELATIVE: return "R_X86_64_IRELATIVE";
  case R_X86_64_GOTPCRELX: return "R_X86_64_GOTPCRELX";
  case R_X86_64_REX_GOTPCRELX: return "R_X86_64_REX_GOTPCRELX";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<I386>(u32 r_type) {
  switch (r_type) {
  case R_386_NONE: return "R_386_NONE";
  case R_386_32: return "R_386_32";
  case R_386_PC32: return "R_386_PC32";
  case R_386_GOT32: return "R_386_GOT32";
  case R_386_PLT32: return "R_386_PLT32";
  case R_386_COPY: return "R_386_COPY";
  case R_386_GLOB_DAT: return "R_386_GLOB_DAT";
  case R_386_JUMP_SLOT: return "R_386_JUMP_SLOT";
  case R_386_RELATIVE: return "R_386_RELATIVE";
  case R_386_GOTOFF: return "R_386_GOTOFF";
  case R_386_GOTPC: return "R_386_GOTPC";
  case R_386_32PLT: return "R_386_32PLT";
  case R_386_TLS_TPOFF: return "R_386_TLS_TPOFF";
  case R_386_TLS_IE: return "R_386_TLS_IE";
  case R_386_TLS_GOTIE: return "R_386_TLS_GOTIE";
  case R_386_TLS_LE: return "R_386_TLS_LE";
  case R_386_TLS_GD: return "R_386_TLS_GD";
  case R_386_TLS_LDM: return "R_386_TLS_LDM";
  case R_386_16: return "R_386_16";
  case R_386_PC16: return "R_386_PC16";
  case R_386_8: return "R_386_8";
  case R_386_PC8: return "R_386_PC8";
  case R_386_TLS_GD_32: return "R_386_TLS_GD_32";
  case R_386_TLS_GD_PUSH: return "R_386_TLS_GD_PUSH";
  case R_386_TLS_GD_CALL: return "R_386_TLS_GD_CALL";
  case R_386_TLS_GD_POP: return "R_386_TLS_GD_POP";
  case R_386_TLS_LDM_32: return "R_386_TLS_LDM_32";
  case R_386_TLS_LDM_PUSH: return "R_386_TLS_LDM_PUSH";
  case R_386_TLS_LDM_CALL: return "R_386_TLS_LDM_CALL";
  case R_386_TLS_LDM_POP: return "R_386_TLS_LDM_POP";
  case R_386_TLS_LDO_32: return "R_386_TLS_LDO_32";
  case R_386_TLS_IE_32: return "R_386_TLS_IE_32";
  case R_386_TLS_LE_32: return "R_386_TLS_LE_32";
  case R_386_TLS_DTPMOD32: return "R_386_TLS_DTPMOD32";
  case R_386_TLS_DTPOFF32: return "R_386_TLS_DTPOFF32";
  case R_386_TLS_TPOFF32: return "R_386_TLS_TPOFF32";
  case R_386_SIZE32: return "R_386_SIZE32";
  case R_386_TLS_GOTDESC: return "R_386_TLS_GOTDESC";
  case R_386_TLS_DESC_CALL: return "R_386_TLS_DESC_CALL";
  case R_386_TLS_DESC: return "R_386_TLS_DESC";
  case R_386_IRELATIVE: return "R_386_IRELATIVE";
  case R_386_GOT32X: return "R_386_GOT32X";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<ARM64>(u32 r_type) {
  switch (r_type) {
  case R_AARCH64_NONE: return "R_AARCH64_NONE";
  case R_AARCH64_ABS64: return "R_AARCH64_ABS64";
  case R_AARCH64_ABS32: return "R_AARCH64_ABS32";
  case R_AARCH64_ABS16: return "R_AARCH64_ABS16";
  case R_AARCH64_PREL64: return "R_AARCH64_PREL64";
  case R_AARCH64_PREL32: return "R_AARCH64_PREL32";
  case R_AARCH64_PREL16: return "R_AARCH64_PREL16";
  case R_AARCH64_MOVW_UABS_G0: return "R_AARCH64_MOVW_UABS_G0";
  case R_AARCH64_MOVW_UABS_G0_NC: return "R_AARCH64_MOVW_UABS_G0_NC";
  case R_AARCH64_MOVW_UABS_G1: return "R_AARCH64_MOVW_UABS_G1";
  case R_AARCH64_MOVW_UABS_G1_NC: return "R_AARCH64_MOVW_UABS_G1_NC";
  case R_AARCH64_MOVW_UABS_G2: return "R_AARCH64_MOVW_UABS_G2";
  case R_AARCH64_MOVW_UABS_G2_NC: return "R_AARCH64_MOVW_UABS_G2_NC";
  case R_AARCH64_MOVW_UABS_G3: return "R_AARCH64_MOVW_UABS_G3";
  case R_AARCH64_MOVW_SABS_G0: return "R_AARCH64_MOVW_SABS_G0";
  case R_AARCH64_MOVW_SABS_G1: return "R_AARCH64_MOVW_SABS_G1";
  case R_AARCH64_MOVW_SABS_G2: return "R_AARCH64_MOVW_SABS_G2";
  case R_AARCH64_LD_PREL_LO19: return "R_AARCH64_LD_PREL_LO19";
  case R_AARCH64_ADR_PREL_LO21: return "R_AARCH64_ADR_PREL_LO21";
  case R_AARCH64_ADR_PREL_PG_HI21: return "R_AARCH64_ADR_PREL_PG_HI21";
  case R_AARCH64_ADR_PREL_PG_HI21_NC: return "R_AARCH64_ADR_PREL_PG_HI21_NC";
  case R_AARCH64_ADD_ABS_LO12_NC: return "R_AARCH64_ADD_ABS_LO12_NC";
  case R_AARCH64_LDST8_ABS_LO12_NC: return "R_AARCH64_LDST8_ABS_LO12_NC";
  case R_AARCH64_TSTBR14: return "R_AARCH64_TSTBR14";
  case R_AARCH64_CONDBR19: return "R_AARCH64_CONDBR19";
  case R_AARCH64_JUMP26: return "R_AARCH64_JUMP26";
  case R_AARCH64_CALL26: return "R_AARCH64_CALL26";
  case R_AARCH64_LDST16_ABS_LO12_NC: return "R_AARCH64_LDST16_ABS_LO12_NC";
  case R_AARCH64_LDST32_ABS_LO12_NC: return "R_AARCH64_LDST32_ABS_LO12_NC";
  case R_AARCH64_LDST64_ABS_LO12_NC: return "R_AARCH64_LDST64_ABS_LO12_NC";
  case R_AARCH64_MOVW_PREL_G0: return "R_AARCH64_MOVW_PREL_G0";
  case R_AARCH64_MOVW_PREL_G0_NC: return "R_AARCH64_MOVW_PREL_G0_NC";
  case R_AARCH64_MOVW_PREL_G1: return "R_AARCH64_MOVW_PREL_G1";
  case R_AARCH64_MOVW_PREL_G1_NC: return "R_AARCH64_MOVW_PREL_G1_NC";
  case R_AARCH64_MOVW_PREL_G2: return "R_AARCH64_MOVW_PREL_G2";
  case R_AARCH64_MOVW_PREL_G2_NC: return "R_AARCH64_MOVW_PREL_G2_NC";
  case R_AARCH64_MOVW_PREL_G3: return "R_AARCH64_MOVW_PREL_G3";
  case R_AARCH64_LDST128_ABS_LO12_NC: return "R_AARCH64_LDST128_ABS_LO12_NC";
  case R_AARCH64_ADR_GOT_PAGE: return "R_AARCH64_ADR_GOT_PAGE";
  case R_AARCH64_LD64_GOT_LO12_NC: return "R_AARCH64_LD64_GOT_LO12_NC";
  case R_AARCH64_LD64_GOTPAGE_LO15: return "R_AARCH64_LD64_GOTPAGE_LO15";
  case R_AARCH64_PLT32: return "R_AARCH64_PLT32";
  case R_AARCH64_TLSGD_ADR_PREL21: return "R_AARCH64_TLSGD_ADR_PREL21";
  case R_AARCH64_TLSGD_ADR_PAGE21: return "R_AARCH64_TLSGD_ADR_PAGE21";
  case R_AARCH64_TLSGD_ADD_LO12_NC: return "R_AARCH64_TLSGD_ADD_LO12_NC";
  case R_AARCH64_TLSGD_MOVW_G1: return "R_AARCH64_TLSGD_MOVW_G1";
  case R_AARCH64_TLSGD_MOVW_G0_NC: return "R_AARCH64_TLSGD_MOVW_G0_NC";
  case R_AARCH64_TLSLD_ADR_PREL21: return "R_AARCH64_TLSLD_ADR_PREL21";
  case R_AARCH64_TLSLD_ADR_PAGE21: return "R_AARCH64_TLSLD_ADR_PAGE21";
  case R_AARCH64_TLSLD_ADD_LO12_NC: return "R_AARCH64_TLSLD_ADD_LO12_NC";
  case R_AARCH64_TLSLD_MOVW_G1: return "R_AARCH64_TLSLD_MOVW_G1";
  case R_AARCH64_TLSLD_MOVW_G0_NC: return "R_AARCH64_TLSLD_MOVW_G0_NC";
  case R_AARCH64_TLSLD_LD_PREL19: return "R_AARCH64_TLSLD_LD_PREL19";
  case R_AARCH64_TLSLD_MOVW_DTPREL_G2: return "R_AARCH64_TLSLD_MOVW_DTPREL_G2";
  case R_AARCH64_TLSLD_MOVW_DTPREL_G1: return "R_AARCH64_TLSLD_MOVW_DTPREL_G1";
  case R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC: return "R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC";
  case R_AARCH64_TLSLD_MOVW_DTPREL_G0: return "R_AARCH64_TLSLD_MOVW_DTPREL_G0";
  case R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC: return "R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC";
  case R_AARCH64_TLSLD_ADD_DTPREL_HI12: return "R_AARCH64_TLSLD_ADD_DTPREL_HI12";
  case R_AARCH64_TLSLD_ADD_DTPREL_LO12: return "R_AARCH64_TLSLD_ADD_DTPREL_LO12";
  case R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC: return "R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC";
  case R_AARCH64_TLSLD_LDST8_DTPREL_LO12: return "R_AARCH64_TLSLD_LDST8_DTPREL_LO12";
  case R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC: return "R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC";
  case R_AARCH64_TLSLD_LDST16_DTPREL_LO12: return "R_AARCH64_TLSLD_LDST16_DTPREL_LO12";
  case R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC: return "R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC";
  case R_AARCH64_TLSLD_LDST32_DTPREL_LO12: return "R_AARCH64_TLSLD_LDST32_DTPREL_LO12";
  case R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC: return "R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC";
  case R_AARCH64_TLSLD_LDST64_DTPREL_LO12: return "R_AARCH64_TLSLD_LDST64_DTPREL_LO12";
  case R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC: return "R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC";
  case R_AARCH64_TLSIE_MOVW_GOTTPREL_G1: return "R_AARCH64_TLSIE_MOVW_GOTTPREL_G1";
  case R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC: return "R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC";
  case R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21: return "R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21";
  case R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC: return "R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC";
  case R_AARCH64_TLSIE_LD_GOTTPREL_PREL19: return "R_AARCH64_TLSIE_LD_GOTTPREL_PREL19";
  case R_AARCH64_TLSLE_MOVW_TPREL_G2: return "R_AARCH64_TLSLE_MOVW_TPREL_G2";
  case R_AARCH64_TLSLE_MOVW_TPREL_G1: return "R_AARCH64_TLSLE_MOVW_TPREL_G1";
  case R_AARCH64_TLSLE_MOVW_TPREL_G1_NC: return "R_AARCH64_TLSLE_MOVW_TPREL_G1_NC";
  case R_AARCH64_TLSLE_MOVW_TPREL_G0: return "R_AARCH64_TLSLE_MOVW_TPREL_G0";
  case R_AARCH64_TLSLE_MOVW_TPREL_G0_NC: return "R_AARCH64_TLSLE_MOVW_TPREL_G0_NC";
  case R_AARCH64_TLSLE_ADD_TPREL_HI12: return "R_AARCH64_TLSLE_ADD_TPREL_HI12";
  case R_AARCH64_TLSLE_ADD_TPREL_LO12: return "R_AARCH64_TLSLE_ADD_TPREL_LO12";
  case R_AARCH64_TLSLE_ADD_TPREL_LO12_NC: return "R_AARCH64_TLSLE_ADD_TPREL_LO12_NC";
  case R_AARCH64_TLSLE_LDST8_TPREL_LO12: return "R_AARCH64_TLSLE_LDST8_TPREL_LO12";
  case R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC: return "R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC";
  case R_AARCH64_TLSLE_LDST16_TPREL_LO12: return "R_AARCH64_TLSLE_LDST16_TPREL_LO12";
  case R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC: return "R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC";
  case R_AARCH64_TLSLE_LDST32_TPREL_LO12: return "R_AARCH64_TLSLE_LDST32_TPREL_LO12";
  case R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC: return "R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC";
  case R_AARCH64_TLSLE_LDST64_TPREL_LO12: return "R_AARCH64_TLSLE_LDST64_TPREL_LO12";
  case R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC: return "R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC";
  case R_AARCH64_TLSDESC_ADR_PAGE21: return "R_AARCH64_TLSDESC_ADR_PAGE21";
  case R_AARCH64_TLSDESC_LD64_LO12: return "R_AARCH64_TLSDESC_LD64_LO12";
  case R_AARCH64_TLSDESC_ADD_LO12: return "R_AARCH64_TLSDESC_ADD_LO12";
  case R_AARCH64_TLSDESC_CALL: return "R_AARCH64_TLSDESC_CALL";
  case R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC: return "R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC";
  case R_AARCH64_COPY: return "R_AARCH64_COPY";
  case R_AARCH64_GLOB_DAT: return "R_AARCH64_GLOB_DAT";
  case R_AARCH64_JUMP_SLOT: return "R_AARCH64_JUMP_SLOT";
  case R_AARCH64_RELATIVE: return "R_AARCH64_RELATIVE";
  case R_AARCH64_TLS_DTPMOD64: return "R_AARCH64_TLS_DTPMOD64";
  case R_AARCH64_TLS_DTPREL64: return "R_AARCH64_TLS_DTPREL64";
  case R_AARCH64_TLS_TPREL64: return "R_AARCH64_TLS_TPREL64";
  case R_AARCH64_TLSDESC: return "R_AARCH64_TLSDESC";
  case R_AARCH64_IRELATIVE: return "R_AARCH64_IRELATIVE";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<ARM32>(u32 r_type) {
  switch (r_type) {
  case R_ARM_NONE: return "R_ARM_NONE";
  case R_ARM_PC24: return "R_ARM_PC24";
  case R_ARM_ABS32: return "R_ARM_ABS32";
  case R_ARM_REL32: return "R_ARM_REL32";
  case R_ARM_LDR_PC_G0: return "R_ARM_LDR_PC_G0";
  case R_ARM_ABS16: return "R_ARM_ABS16";
  case R_ARM_ABS12: return "R_ARM_ABS12";
  case R_ARM_THM_ABS5: return "R_ARM_THM_ABS5";
  case R_ARM_ABS8: return "R_ARM_ABS8";
  case R_ARM_SBREL32: return "R_ARM_SBREL32";
  case R_ARM_THM_CALL: return "R_ARM_THM_CALL";
  case R_ARM_THM_PC8: return "R_ARM_THM_PC8";
  case R_ARM_BREL_ADJ: return "R_ARM_BREL_ADJ";
  case R_ARM_TLS_DESC: return "R_ARM_TLS_DESC";
  case R_ARM_THM_SWI8: return "R_ARM_THM_SWI8";
  case R_ARM_XPC25: return "R_ARM_XPC25";
  case R_ARM_THM_XPC22: return "R_ARM_THM_XPC22";
  case R_ARM_TLS_DTPMOD32: return "R_ARM_TLS_DTPMOD32";
  case R_ARM_TLS_DTPOFF32: return "R_ARM_TLS_DTPOFF32";
  case R_ARM_TLS_TPOFF32: return "R_ARM_TLS_TPOFF32";
  case R_ARM_COPY: return "R_ARM_COPY";
  case R_ARM_GLOB_DAT: return "R_ARM_GLOB_DAT";
  case R_ARM_JUMP_SLOT: return "R_ARM_JUMP_SLOT";
  case R_ARM_RELATIVE: return "R_ARM_RELATIVE";
  case R_ARM_GOTOFF32: return "R_ARM_GOTOFF32";
  case R_ARM_BASE_PREL: return "R_ARM_BASE_PREL";
  case R_ARM_GOT_BREL: return "R_ARM_GOT_BREL";
  case R_ARM_PLT32: return "R_ARM_PLT32";
  case R_ARM_CALL: return "R_ARM_CALL";
  case R_ARM_JUMP24: return "R_ARM_JUMP24";
  case R_ARM_THM_JUMP24: return "R_ARM_THM_JUMP24";
  case R_ARM_BASE_ABS: return "R_ARM_BASE_ABS";
  case R_ARM_ALU_PCREL_7_0: return "R_ARM_ALU_PCREL_7_0";
  case R_ARM_ALU_PCREL_15_8: return "R_ARM_ALU_PCREL_15_8";
  case R_ARM_ALU_PCREL_23_15: return "R_ARM_ALU_PCREL_23_15";
  case R_ARM_LDR_SBREL_11_0_NC: return "R_ARM_LDR_SBREL_11_0_NC";
  case R_ARM_ALU_SBREL_19_12_NC: return "R_ARM_ALU_SBREL_19_12_NC";
  case R_ARM_ALU_SBREL_27_20_CK: return "R_ARM_ALU_SBREL_27_20_CK";
  case R_ARM_TARGET1: return "R_ARM_TARGET1";
  case R_ARM_SBREL31: return "R_ARM_SBREL31";
  case R_ARM_V4BX: return "R_ARM_V4BX";
  case R_ARM_TARGET2: return "R_ARM_TARGET2";
  case R_ARM_PREL31: return "R_ARM_PREL31";
  case R_ARM_MOVW_ABS_NC: return "R_ARM_MOVW_ABS_NC";
  case R_ARM_MOVT_ABS: return "R_ARM_MOVT_ABS";
  case R_ARM_MOVW_PREL_NC: return "R_ARM_MOVW_PREL_NC";
  case R_ARM_MOVT_PREL: return "R_ARM_MOVT_PREL";
  case R_ARM_THM_MOVW_ABS_NC: return "R_ARM_THM_MOVW_ABS_NC";
  case R_ARM_THM_MOVT_ABS: return "R_ARM_THM_MOVT_ABS";
  case R_ARM_THM_MOVW_PREL_NC: return "R_ARM_THM_MOVW_PREL_NC";
  case R_ARM_THM_MOVT_PREL: return "R_ARM_THM_MOVT_PREL";
  case R_ARM_THM_JUMP19: return "R_ARM_THM_JUMP19";
  case R_ARM_THM_JUMP6: return "R_ARM_THM_JUMP6";
  case R_ARM_THM_ALU_PREL_11_0: return "R_ARM_THM_ALU_PREL_11_0";
  case R_ARM_THM_PC12: return "R_ARM_THM_PC12";
  case R_ARM_ABS32_NOI: return "R_ARM_ABS32_NOI";
  case R_ARM_REL32_NOI: return "R_ARM_REL32_NOI";
  case R_ARM_ALU_PC_G0_NC: return "R_ARM_ALU_PC_G0_NC";
  case R_ARM_ALU_PC_G0: return "R_ARM_ALU_PC_G0";
  case R_ARM_ALU_PC_G1_NC: return "R_ARM_ALU_PC_G1_NC";
  case R_ARM_ALU_PC_G1: return "R_ARM_ALU_PC_G1";
  case R_ARM_ALU_PC_G2: return "R_ARM_ALU_PC_G2";
  case R_ARM_LDR_PC_G1: return "R_ARM_LDR_PC_G1";
  case R_ARM_LDR_PC_G2: return "R_ARM_LDR_PC_G2";
  case R_ARM_LDRS_PC_G0: return "R_ARM_LDRS_PC_G0";
  case R_ARM_LDRS_PC_G1: return "R_ARM_LDRS_PC_G1";
  case R_ARM_LDRS_PC_G2: return "R_ARM_LDRS_PC_G2";
  case R_ARM_LDC_PC_G0: return "R_ARM_LDC_PC_G0";
  case R_ARM_LDC_PC_G1: return "R_ARM_LDC_PC_G1";
  case R_ARM_LDC_PC_G2: return "R_ARM_LDC_PC_G2";
  case R_ARM_ALU_SB_G0_NC: return "R_ARM_ALU_SB_G0_NC";
  case R_ARM_ALU_SB_G0: return "R_ARM_ALU_SB_G0";
  case R_ARM_ALU_SB_G1_NC: return "R_ARM_ALU_SB_G1_NC";
  case R_ARM_ALU_SB_G1: return "R_ARM_ALU_SB_G1";
  case R_ARM_ALU_SB_G2: return "R_ARM_ALU_SB_G2";
  case R_ARM_LDR_SB_G0: return "R_ARM_LDR_SB_G0";
  case R_ARM_LDR_SB_G1: return "R_ARM_LDR_SB_G1";
  case R_ARM_LDR_SB_G2: return "R_ARM_LDR_SB_G2";
  case R_ARM_LDRS_SB_G0: return "R_ARM_LDRS_SB_G0";
  case R_ARM_LDRS_SB_G1: return "R_ARM_LDRS_SB_G1";
  case R_ARM_LDRS_SB_G2: return "R_ARM_LDRS_SB_G2";
  case R_ARM_LDC_SB_G0: return "R_ARM_LDC_SB_G0";
  case R_ARM_LDC_SB_G1: return "R_ARM_LDC_SB_G1";
  case R_ARM_LDC_SB_G2: return "R_ARM_LDC_SB_G2";
  case R_ARM_MOVW_BREL_NC: return "R_ARM_MOVW_BREL_NC";
  case R_ARM_MOVT_BREL: return "R_ARM_MOVT_BREL";
  case R_ARM_MOVW_BREL: return "R_ARM_MOVW_BREL";
  case R_ARM_THM_MOVW_BREL_NC: return "R_ARM_THM_MOVW_BREL_NC";
  case R_ARM_THM_MOVT_BREL: return "R_ARM_THM_MOVT_BREL";
  case R_ARM_THM_MOVW_BREL: return "R_ARM_THM_MOVW_BREL";
  case R_ARM_TLS_GOTDESC: return "R_ARM_TLS_GOTDESC";
  case R_ARM_TLS_CALL: return "R_ARM_TLS_CALL";
  case R_ARM_TLS_DESCSEQ: return "R_ARM_TLS_DESCSEQ";
  case R_ARM_THM_TLS_CALL: return "R_ARM_THM_TLS_CALL";
  case R_ARM_PLT32_ABS: return "R_ARM_PLT32_ABS";
  case R_ARM_GOT_ABS: return "R_ARM_GOT_ABS";
  case R_ARM_GOT_PREL: return "R_ARM_GOT_PREL";
  case R_ARM_GOT_BREL12: return "R_ARM_GOT_BREL12";
  case R_ARM_GOTOFF12: return "R_ARM_GOTOFF12";
  case R_ARM_GOTRELAX: return "R_ARM_GOTRELAX";
  case R_ARM_GNU_VTENTRY: return "R_ARM_GNU_VTENTRY";
  case R_ARM_GNU_VTINHERIT: return "R_ARM_GNU_VTINHERIT";
  case R_ARM_THM_JUMP11: return "R_ARM_THM_JUMP11";
  case R_ARM_THM_JUMP8: return "R_ARM_THM_JUMP8";
  case R_ARM_TLS_GD32: return "R_ARM_TLS_GD32";
  case R_ARM_TLS_LDM32: return "R_ARM_TLS_LDM32";
  case R_ARM_TLS_LDO32: return "R_ARM_TLS_LDO32";
  case R_ARM_TLS_IE32: return "R_ARM_TLS_IE32";
  case R_ARM_TLS_LE32: return "R_ARM_TLS_LE32";
  case R_ARM_TLS_LDO12: return "R_ARM_TLS_LDO12";
  case R_ARM_TLS_LE12: return "R_ARM_TLS_LE12";
  case R_ARM_TLS_IE12GP: return "R_ARM_TLS_IE12GP";
  case R_ARM_PRIVATE_0: return "R_ARM_PRIVATE_0";
  case R_ARM_PRIVATE_1: return "R_ARM_PRIVATE_1";
  case R_ARM_PRIVATE_2: return "R_ARM_PRIVATE_2";
  case R_ARM_PRIVATE_3: return "R_ARM_PRIVATE_3";
  case R_ARM_PRIVATE_4: return "R_ARM_PRIVATE_4";
  case R_ARM_PRIVATE_5: return "R_ARM_PRIVATE_5";
  case R_ARM_PRIVATE_6: return "R_ARM_PRIVATE_6";
  case R_ARM_PRIVATE_7: return "R_ARM_PRIVATE_7";
  case R_ARM_PRIVATE_8: return "R_ARM_PRIVATE_8";
  case R_ARM_PRIVATE_9: return "R_ARM_PRIVATE_9";
  case R_ARM_PRIVATE_10: return "R_ARM_PRIVATE_10";
  case R_ARM_PRIVATE_11: return "R_ARM_PRIVATE_11";
  case R_ARM_PRIVATE_12: return "R_ARM_PRIVATE_12";
  case R_ARM_PRIVATE_13: return "R_ARM_PRIVATE_13";
  case R_ARM_PRIVATE_14: return "R_ARM_PRIVATE_14";
  case R_ARM_PRIVATE_15: return "R_ARM_PRIVATE_15";
  case R_ARM_ME_TOO: return "R_ARM_ME_TOO";
  case R_ARM_THM_TLS_DESCSEQ16: return "R_ARM_THM_TLS_DESCSEQ16";
  case R_ARM_THM_TLS_DESCSEQ32: return "R_ARM_THM_TLS_DESCSEQ32";
  case R_ARM_THM_BF16: return "R_ARM_THM_BF16";
  case R_ARM_THM_BF12: return "R_ARM_THM_BF12";
  case R_ARM_THM_BF18: return "R_ARM_THM_BF18";
  case R_ARM_IRELATIVE: return "R_ARM_IRELATIVE";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<RV64LE>(u32 r_type) {
  switch (r_type) {
  case R_RISCV_NONE: return "R_RISCV_NONE";
  case R_RISCV_32: return "R_RISCV_32";
  case R_RISCV_64: return "R_RISCV_64";
  case R_RISCV_RELATIVE: return "R_RISCV_RELATIVE";
  case R_RISCV_COPY: return "R_RISCV_COPY";
  case R_RISCV_JUMP_SLOT: return "R_RISCV_JUMP_SLOT";
  case R_RISCV_TLS_DTPMOD32: return "R_RISCV_TLS_DTPMOD32";
  case R_RISCV_TLS_DTPMOD64: return "R_RISCV_TLS_DTPMOD64";
  case R_RISCV_TLS_DTPREL32: return "R_RISCV_TLS_DTPREL32";
  case R_RISCV_TLS_DTPREL64: return "R_RISCV_TLS_DTPREL64";
  case R_RISCV_TLS_TPREL32: return "R_RISCV_TLS_TPREL32";
  case R_RISCV_TLS_TPREL64: return "R_RISCV_TLS_TPREL64";
  case R_RISCV_BRANCH: return "R_RISCV_BRANCH";
  case R_RISCV_JAL: return "R_RISCV_JAL";
  case R_RISCV_CALL: return "R_RISCV_CALL";
  case R_RISCV_CALL_PLT: return "R_RISCV_CALL_PLT";
  case R_RISCV_GOT_HI20: return "R_RISCV_GOT_HI20";
  case R_RISCV_TLS_GOT_HI20: return "R_RISCV_TLS_GOT_HI20";
  case R_RISCV_TLS_GD_HI20: return "R_RISCV_TLS_GD_HI20";
  case R_RISCV_PCREL_HI20: return "R_RISCV_PCREL_HI20";
  case R_RISCV_PCREL_LO12_I: return "R_RISCV_PCREL_LO12_I";
  case R_RISCV_PCREL_LO12_S: return "R_RISCV_PCREL_LO12_S";
  case R_RISCV_HI20: return "R_RISCV_HI20";
  case R_RISCV_LO12_I: return "R_RISCV_LO12_I";
  case R_RISCV_LO12_S: return "R_RISCV_LO12_S";
  case R_RISCV_TPREL_HI20: return "R_RISCV_TPREL_HI20";
  case R_RISCV_TPREL_LO12_I: return "R_RISCV_TPREL_LO12_I";
  case R_RISCV_TPREL_LO12_S: return "R_RISCV_TPREL_LO12_S";
  case R_RISCV_TPREL_ADD: return "R_RISCV_TPREL_ADD";
  case R_RISCV_ADD8: return "R_RISCV_ADD8";
  case R_RISCV_ADD16: return "R_RISCV_ADD16";
  case R_RISCV_ADD32: return "R_RISCV_ADD32";
  case R_RISCV_ADD64: return "R_RISCV_ADD64";
  case R_RISCV_SUB8: return "R_RISCV_SUB8";
  case R_RISCV_SUB16: return "R_RISCV_SUB16";
  case R_RISCV_SUB32: return "R_RISCV_SUB32";
  case R_RISCV_SUB64: return "R_RISCV_SUB64";
  case R_RISCV_ALIGN: return "R_RISCV_ALIGN";
  case R_RISCV_RVC_BRANCH: return "R_RISCV_RVC_BRANCH";
  case R_RISCV_RVC_JUMP: return "R_RISCV_RVC_JUMP";
  case R_RISCV_RVC_LUI: return "R_RISCV_RVC_LUI";
  case R_RISCV_RELAX: return "R_RISCV_RELAX";
  case R_RISCV_SUB6: return "R_RISCV_SUB6";
  case R_RISCV_SET6: return "R_RISCV_SET6";
  case R_RISCV_SET8: return "R_RISCV_SET8";
  case R_RISCV_SET16: return "R_RISCV_SET16";
  case R_RISCV_SET32: return "R_RISCV_SET32";
  case R_RISCV_32_PCREL: return "R_RISCV_32_PCREL";
  case R_RISCV_IRELATIVE: return "R_RISCV_IRELATIVE";
  case R_RISCV_PLT32: return "R_RISCV_PLT32";
  case R_RISCV_SET_ULEB128: return "R_RISCV_SET_ULEB128";
  case R_RISCV_SUB_ULEB128: return "R_RISCV_SUB_ULEB128";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<RV64BE>(u32 r_type) {
  return rel_to_string<RV64LE>(r_type);
}

template <>
std::string rel_to_string<RV32LE>(u32 r_type) {
  return rel_to_string<RV64LE>(r_type);
}

template <>
std::string rel_to_string<RV32BE>(u32 r_type) {
  return rel_to_string<RV64LE>(r_type);
}

template <>
std::string rel_to_string<PPC32>(u32 r_type) {
  switch (r_type) {
  case R_PPC_NONE: return "R_PPC_NONE";
  case R_PPC_ADDR32: return "R_PPC_ADDR32";
  case R_PPC_ADDR24: return "R_PPC_ADDR24";
  case R_PPC_ADDR16: return "R_PPC_ADDR16";
  case R_PPC_ADDR16_LO: return "R_PPC_ADDR16_LO";
  case R_PPC_ADDR16_HI: return "R_PPC_ADDR16_HI";
  case R_PPC_ADDR16_HA: return "R_PPC_ADDR16_HA";
  case R_PPC_ADDR14: return "R_PPC_ADDR14";
  case R_PPC_ADDR14_BRTAKEN: return "R_PPC_ADDR14_BRTAKEN";
  case R_PPC_ADDR14_BRNTAKEN: return "R_PPC_ADDR14_BRNTAKEN";
  case R_PPC_REL24: return "R_PPC_REL24";
  case R_PPC_REL14: return "R_PPC_REL14";
  case R_PPC_REL14_BRTAKEN: return "R_PPC_REL14_BRTAKEN";
  case R_PPC_REL14_BRNTAKEN: return "R_PPC_REL14_BRNTAKEN";
  case R_PPC_GOT16: return "R_PPC_GOT16";
  case R_PPC_GOT16_LO: return "R_PPC_GOT16_LO";
  case R_PPC_GOT16_HI: return "R_PPC_GOT16_HI";
  case R_PPC_GOT16_HA: return "R_PPC_GOT16_HA";
  case R_PPC_PLTREL24: return "R_PPC_PLTREL24";
  case R_PPC_COPY: return "R_PPC_COPY";
  case R_PPC_GLOB_DAT: return "R_PPC_GLOB_DAT";
  case R_PPC_JMP_SLOT: return "R_PPC_JMP_SLOT";
  case R_PPC_RELATIVE: return "R_PPC_RELATIVE";
  case R_PPC_LOCAL24PC: return "R_PPC_LOCAL24PC";
  case R_PPC_UADDR32: return "R_PPC_UADDR32";
  case R_PPC_UADDR16: return "R_PPC_UADDR16";
  case R_PPC_REL32: return "R_PPC_REL32";
  case R_PPC_PLT32: return "R_PPC_PLT32";
  case R_PPC_PLTREL32: return "R_PPC_PLTREL32";
  case R_PPC_PLT16_LO: return "R_PPC_PLT16_LO";
  case R_PPC_PLT16_HI: return "R_PPC_PLT16_HI";
  case R_PPC_PLT16_HA: return "R_PPC_PLT16_HA";
  case R_PPC_SDAREL16: return "R_PPC_SDAREL16";
  case R_PPC_SECTOFF: return "R_PPC_SECTOFF";
  case R_PPC_SECTOFF_LO: return "R_PPC_SECTOFF_LO";
  case R_PPC_SECTOFF_HI: return "R_PPC_SECTOFF_HI";
  case R_PPC_SECTOFF_HA: return "R_PPC_SECTOFF_HA";
  case R_PPC_ADDR30: return "R_PPC_ADDR30";
  case R_PPC_TLS: return "R_PPC_TLS";
  case R_PPC_DTPMOD32: return "R_PPC_DTPMOD32";
  case R_PPC_TPREL16: return "R_PPC_TPREL16";
  case R_PPC_TPREL16_LO: return "R_PPC_TPREL16_LO";
  case R_PPC_TPREL16_HI: return "R_PPC_TPREL16_HI";
  case R_PPC_TPREL16_HA: return "R_PPC_TPREL16_HA";
  case R_PPC_TPREL32: return "R_PPC_TPREL32";
  case R_PPC_DTPREL16: return "R_PPC_DTPREL16";
  case R_PPC_DTPREL16_LO: return "R_PPC_DTPREL16_LO";
  case R_PPC_DTPREL16_HI: return "R_PPC_DTPREL16_HI";
  case R_PPC_DTPREL16_HA: return "R_PPC_DTPREL16_HA";
  case R_PPC_DTPREL32: return "R_PPC_DTPREL32";
  case R_PPC_GOT_TLSGD16: return "R_PPC_GOT_TLSGD16";
  case R_PPC_GOT_TLSGD16_LO: return "R_PPC_GOT_TLSGD16_LO";
  case R_PPC_GOT_TLSGD16_HI: return "R_PPC_GOT_TLSGD16_HI";
  case R_PPC_GOT_TLSGD16_HA: return "R_PPC_GOT_TLSGD16_HA";
  case R_PPC_GOT_TLSLD16: return "R_PPC_GOT_TLSLD16";
  case R_PPC_GOT_TLSLD16_LO: return "R_PPC_GOT_TLSLD16_LO";
  case R_PPC_GOT_TLSLD16_HI: return "R_PPC_GOT_TLSLD16_HI";
  case R_PPC_GOT_TLSLD16_HA: return "R_PPC_GOT_TLSLD16_HA";
  case R_PPC_GOT_TPREL16: return "R_PPC_GOT_TPREL16";
  case R_PPC_GOT_TPREL16_LO: return "R_PPC_GOT_TPREL16_LO";
  case R_PPC_GOT_TPREL16_HI: return "R_PPC_GOT_TPREL16_HI";
  case R_PPC_GOT_TPREL16_HA: return "R_PPC_GOT_TPREL16_HA";
  case R_PPC_GOT_DTPREL16: return "R_PPC_GOT_DTPREL16";
  case R_PPC_GOT_DTPREL16_LO: return "R_PPC_GOT_DTPREL16_LO";
  case R_PPC_GOT_DTPREL16_HI: return "R_PPC_GOT_DTPREL16_HI";
  case R_PPC_GOT_DTPREL16_HA: return "R_PPC_GOT_DTPREL16_HA";
  case R_PPC_TLSGD: return "R_PPC_TLSGD";
  case R_PPC_TLSLD: return "R_PPC_TLSLD";
  case R_PPC_PLTSEQ: return "R_PPC_PLTSEQ";
  case R_PPC_PLTCALL: return "R_PPC_PLTCALL";
  case R_PPC_IRELATIVE: return "R_PPC_IRELATIVE";
  case R_PPC_REL16: return "R_PPC_REL16";
  case R_PPC_REL16_LO: return "R_PPC_REL16_LO";
  case R_PPC_REL16_HI: return "R_PPC_REL16_HI";
  case R_PPC_REL16_HA: return "R_PPC_REL16_HA";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<PPC64V1>(u32 r_type) {
  switch (r_type) {
  case R_PPC64_NONE: return "R_PPC64_NONE";
  case R_PPC64_ADDR32: return "R_PPC64_ADDR32";
  case R_PPC64_ADDR24: return "R_PPC64_ADDR24";
  case R_PPC64_ADDR16: return "R_PPC64_ADDR16";
  case R_PPC64_ADDR16_LO: return "R_PPC64_ADDR16_LO";
  case R_PPC64_ADDR16_HI: return "R_PPC64_ADDR16_HI";
  case R_PPC64_ADDR16_HA: return "R_PPC64_ADDR16_HA";
  case R_PPC64_ADDR14: return "R_PPC64_ADDR14";
  case R_PPC64_ADDR14_BRTAKEN: return "R_PPC64_ADDR14_BRTAKEN";
  case R_PPC64_ADDR14_BRNTAKEN: return "R_PPC64_ADDR14_BRNTAKEN";
  case R_PPC64_REL24: return "R_PPC64_REL24";
  case R_PPC64_REL14: return "R_PPC64_REL14";
  case R_PPC64_REL14_BRTAKEN: return "R_PPC64_REL14_BRTAKEN";
  case R_PPC64_REL14_BRNTAKEN: return "R_PPC64_REL14_BRNTAKEN";
  case R_PPC64_GOT16: return "R_PPC64_GOT16";
  case R_PPC64_GOT16_LO: return "R_PPC64_GOT16_LO";
  case R_PPC64_GOT16_HI: return "R_PPC64_GOT16_HI";
  case R_PPC64_GOT16_HA: return "R_PPC64_GOT16_HA";
  case R_PPC64_COPY: return "R_PPC64_COPY";
  case R_PPC64_GLOB_DAT: return "R_PPC64_GLOB_DAT";
  case R_PPC64_JMP_SLOT: return "R_PPC64_JMP_SLOT";
  case R_PPC64_RELATIVE: return "R_PPC64_RELATIVE";
  case R_PPC64_REL32: return "R_PPC64_REL32";
  case R_PPC64_PLT16_LO: return "R_PPC64_PLT16_LO";
  case R_PPC64_PLT16_HI: return "R_PPC64_PLT16_HI";
  case R_PPC64_PLT16_HA: return "R_PPC64_PLT16_HA";
  case R_PPC64_ADDR64: return "R_PPC64_ADDR64";
  case R_PPC64_ADDR16_HIGHER: return "R_PPC64_ADDR16_HIGHER";
  case R_PPC64_ADDR16_HIGHERA: return "R_PPC64_ADDR16_HIGHERA";
  case R_PPC64_ADDR16_HIGHEST: return "R_PPC64_ADDR16_HIGHEST";
  case R_PPC64_ADDR16_HIGHESTA: return "R_PPC64_ADDR16_HIGHESTA";
  case R_PPC64_REL64: return "R_PPC64_REL64";
  case R_PPC64_TOC16: return "R_PPC64_TOC16";
  case R_PPC64_TOC16_LO: return "R_PPC64_TOC16_LO";
  case R_PPC64_TOC16_HI: return "R_PPC64_TOC16_HI";
  case R_PPC64_TOC16_HA: return "R_PPC64_TOC16_HA";
  case R_PPC64_TOC: return "R_PPC64_TOC";
  case R_PPC64_ADDR16_DS: return "R_PPC64_ADDR16_DS";
  case R_PPC64_ADDR16_LO_DS: return "R_PPC64_ADDR16_LO_DS";
  case R_PPC64_GOT16_DS: return "R_PPC64_GOT16_DS";
  case R_PPC64_GOT16_LO_DS: return "R_PPC64_GOT16_LO_DS";
  case R_PPC64_PLT16_LO_DS: return "R_PPC64_PLT16_LO_DS";
  case R_PPC64_TOC16_DS: return "R_PPC64_TOC16_DS";
  case R_PPC64_TOC16_LO_DS: return "R_PPC64_TOC16_LO_DS";
  case R_PPC64_TLS: return "R_PPC64_TLS";
  case R_PPC64_DTPMOD64: return "R_PPC64_DTPMOD64";
  case R_PPC64_TPREL16: return "R_PPC64_TPREL16";
  case R_PPC64_TPREL16_LO: return "R_PPC64_TPREL16_LO";
  case R_PPC64_TPREL16_HI: return "R_PPC64_TPREL16_HI";
  case R_PPC64_TPREL16_HA: return "R_PPC64_TPREL16_HA";
  case R_PPC64_TPREL64: return "R_PPC64_TPREL64";
  case R_PPC64_DTPREL16: return "R_PPC64_DTPREL16";
  case R_PPC64_DTPREL16_LO: return "R_PPC64_DTPREL16_LO";
  case R_PPC64_DTPREL16_HI: return "R_PPC64_DTPREL16_HI";
  case R_PPC64_DTPREL16_HA: return "R_PPC64_DTPREL16_HA";
  case R_PPC64_DTPREL64: return "R_PPC64_DTPREL64";
  case R_PPC64_GOT_TLSGD16: return "R_PPC64_GOT_TLSGD16";
  case R_PPC64_GOT_TLSGD16_LO: return "R_PPC64_GOT_TLSGD16_LO";
  case R_PPC64_GOT_TLSGD16_HI: return "R_PPC64_GOT_TLSGD16_HI";
  case R_PPC64_GOT_TLSGD16_HA: return "R_PPC64_GOT_TLSGD16_HA";
  case R_PPC64_GOT_TLSLD16: return "R_PPC64_GOT_TLSLD16";
  case R_PPC64_GOT_TLSLD16_LO: return "R_PPC64_GOT_TLSLD16_LO";
  case R_PPC64_GOT_TLSLD16_HI: return "R_PPC64_GOT_TLSLD16_HI";
  case R_PPC64_GOT_TLSLD16_HA: return "R_PPC64_GOT_TLSLD16_HA";
  case R_PPC64_GOT_TPREL16_DS: return "R_PPC64_GOT_TPREL16_DS";
  case R_PPC64_GOT_TPREL16_LO_DS: return "R_PPC64_GOT_TPREL16_LO_DS";
  case R_PPC64_GOT_TPREL16_HI: return "R_PPC64_GOT_TPREL16_HI";
  case R_PPC64_GOT_TPREL16_HA: return "R_PPC64_GOT_TPREL16_HA";
  case R_PPC64_GOT_DTPREL16_DS: return "R_PPC64_GOT_DTPREL16_DS";
  case R_PPC64_GOT_DTPREL16_LO_DS: return "R_PPC64_GOT_DTPREL16_LO_DS";
  case R_PPC64_GOT_DTPREL16_HI: return "R_PPC64_GOT_DTPREL16_HI";
  case R_PPC64_GOT_DTPREL16_HA: return "R_PPC64_GOT_DTPREL16_HA";
  case R_PPC64_TPREL16_DS: return "R_PPC64_TPREL16_DS";
  case R_PPC64_TPREL16_LO_DS: return "R_PPC64_TPREL16_LO_DS";
  case R_PPC64_TPREL16_HIGHER: return "R_PPC64_TPREL16_HIGHER";
  case R_PPC64_TPREL16_HIGHERA: return "R_PPC64_TPREL16_HIGHERA";
  case R_PPC64_TPREL16_HIGHEST: return "R_PPC64_TPREL16_HIGHEST";
  case R_PPC64_TPREL16_HIGHESTA: return "R_PPC64_TPREL16_HIGHESTA";
  case R_PPC64_DTPREL16_DS: return "R_PPC64_DTPREL16_DS";
  case R_PPC64_DTPREL16_LO_DS: return "R_PPC64_DTPREL16_LO_DS";
  case R_PPC64_DTPREL16_HIGHER: return "R_PPC64_DTPREL16_HIGHER";
  case R_PPC64_DTPREL16_HIGHERA: return "R_PPC64_DTPREL16_HIGHERA";
  case R_PPC64_DTPREL16_HIGHEST: return "R_PPC64_DTPREL16_HIGHEST";
  case R_PPC64_DTPREL16_HIGHESTA: return "R_PPC64_DTPREL16_HIGHESTA";
  case R_PPC64_TLSGD: return "R_PPC64_TLSGD";
  case R_PPC64_TLSLD: return "R_PPC64_TLSLD";
  case R_PPC64_ADDR16_HIGH: return "R_PPC64_ADDR16_HIGH";
  case R_PPC64_ADDR16_HIGHA: return "R_PPC64_ADDR16_HIGHA";
  case R_PPC64_TPREL16_HIGH: return "R_PPC64_TPREL16_HIGH";
  case R_PPC64_TPREL16_HIGHA: return "R_PPC64_TPREL16_HIGHA";
  case R_PPC64_DTPREL16_HIGH: return "R_PPC64_DTPREL16_HIGH";
  case R_PPC64_DTPREL16_HIGHA: return "R_PPC64_DTPREL16_HIGHA";
  case R_PPC64_REL24_NOTOC: return "R_PPC64_REL24_NOTOC";
  case R_PPC64_PLTSEQ: return "R_PPC64_PLTSEQ";
  case R_PPC64_PLTCALL: return "R_PPC64_PLTCALL";
  case R_PPC64_PLTSEQ_NOTOC: return "R_PPC64_PLTSEQ_NOTOC";
  case R_PPC64_PLTCALL_NOTOC: return "R_PPC64_PLTCALL_NOTOC";
  case R_PPC64_PCREL_OPT: return "R_PPC64_PCREL_OPT";
  case R_PPC64_PCREL34: return "R_PPC64_PCREL34";
  case R_PPC64_GOT_PCREL34: return "R_PPC64_GOT_PCREL34";
  case R_PPC64_PLT_PCREL34: return "R_PPC64_PLT_PCREL34";
  case R_PPC64_PLT_PCREL34_NOTOC: return "R_PPC64_PLT_PCREL34_NOTOC";
  case R_PPC64_TPREL34: return "R_PPC64_TPREL34";
  case R_PPC64_DTPREL34: return "R_PPC64_DTPREL34";
  case R_PPC64_GOT_TLSGD_PCREL34: return "R_PPC64_GOT_TLSGD_PCREL34";
  case R_PPC64_GOT_TLSLD_PCREL34: return "R_PPC64_GOT_TLSLD_PCREL34";
  case R_PPC64_GOT_TPREL_PCREL34: return "R_PPC64_GOT_TPREL_PCREL34";
  case R_PPC64_IRELATIVE: return "R_PPC64_IRELATIVE";
  case R_PPC64_REL16: return "R_PPC64_REL16";
  case R_PPC64_REL16_LO: return "R_PPC64_REL16_LO";
  case R_PPC64_REL16_HI: return "R_PPC64_REL16_HI";
  case R_PPC64_REL16_HA: return "R_PPC64_REL16_HA";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<PPC64V2>(u32 r_type) {
  return rel_to_string<PPC64V1>(r_type);
}

template <>
std::string rel_to_string<SPARC64>(u32 r_type) {
  switch (r_type) {
  case R_SPARC_NONE: return "R_SPARC_NONE";
  case R_SPARC_8: return "R_SPARC_8";
  case R_SPARC_16: return "R_SPARC_16";
  case R_SPARC_32: return "R_SPARC_32";
  case R_SPARC_DISP8: return "R_SPARC_DISP8";
  case R_SPARC_DISP16: return "R_SPARC_DISP16";
  case R_SPARC_DISP32: return "R_SPARC_DISP32";
  case R_SPARC_WDISP30: return "R_SPARC_WDISP30";
  case R_SPARC_WDISP22: return "R_SPARC_WDISP22";
  case R_SPARC_HI22: return "R_SPARC_HI22";
  case R_SPARC_22: return "R_SPARC_22";
  case R_SPARC_13: return "R_SPARC_13";
  case R_SPARC_LO10: return "R_SPARC_LO10";
  case R_SPARC_GOT10: return "R_SPARC_GOT10";
  case R_SPARC_GOT13: return "R_SPARC_GOT13";
  case R_SPARC_GOT22: return "R_SPARC_GOT22";
  case R_SPARC_PC10: return "R_SPARC_PC10";
  case R_SPARC_PC22: return "R_SPARC_PC22";
  case R_SPARC_WPLT30: return "R_SPARC_WPLT30";
  case R_SPARC_COPY: return "R_SPARC_COPY";
  case R_SPARC_GLOB_DAT: return "R_SPARC_GLOB_DAT";
  case R_SPARC_JMP_SLOT: return "R_SPARC_JMP_SLOT";
  case R_SPARC_RELATIVE: return "R_SPARC_RELATIVE";
  case R_SPARC_UA32: return "R_SPARC_UA32";
  case R_SPARC_PLT32: return "R_SPARC_PLT32";
  case R_SPARC_HIPLT22: return "R_SPARC_HIPLT22";
  case R_SPARC_LOPLT10: return "R_SPARC_LOPLT10";
  case R_SPARC_PCPLT32: return "R_SPARC_PCPLT32";
  case R_SPARC_PCPLT22: return "R_SPARC_PCPLT22";
  case R_SPARC_PCPLT10: return "R_SPARC_PCPLT10";
  case R_SPARC_10: return "R_SPARC_10";
  case R_SPARC_11: return "R_SPARC_11";
  case R_SPARC_64: return "R_SPARC_64";
  case R_SPARC_OLO10: return "R_SPARC_OLO10";
  case R_SPARC_HH22: return "R_SPARC_HH22";
  case R_SPARC_HM10: return "R_SPARC_HM10";
  case R_SPARC_LM22: return "R_SPARC_LM22";
  case R_SPARC_PC_HH22: return "R_SPARC_PC_HH22";
  case R_SPARC_PC_HM10: return "R_SPARC_PC_HM10";
  case R_SPARC_PC_LM22: return "R_SPARC_PC_LM22";
  case R_SPARC_WDISP16: return "R_SPARC_WDISP16";
  case R_SPARC_WDISP19: return "R_SPARC_WDISP19";
  case R_SPARC_7: return "R_SPARC_7";
  case R_SPARC_5: return "R_SPARC_5";
  case R_SPARC_6: return "R_SPARC_6";
  case R_SPARC_DISP64: return "R_SPARC_DISP64";
  case R_SPARC_PLT64: return "R_SPARC_PLT64";
  case R_SPARC_HIX22: return "R_SPARC_HIX22";
  case R_SPARC_LOX10: return "R_SPARC_LOX10";
  case R_SPARC_H44: return "R_SPARC_H44";
  case R_SPARC_M44: return "R_SPARC_M44";
  case R_SPARC_L44: return "R_SPARC_L44";
  case R_SPARC_REGISTER: return "R_SPARC_REGISTER";
  case R_SPARC_UA64: return "R_SPARC_UA64";
  case R_SPARC_UA16: return "R_SPARC_UA16";
  case R_SPARC_TLS_GD_HI22: return "R_SPARC_TLS_GD_HI22";
  case R_SPARC_TLS_GD_LO10: return "R_SPARC_TLS_GD_LO10";
  case R_SPARC_TLS_GD_ADD: return "R_SPARC_TLS_GD_ADD";
  case R_SPARC_TLS_GD_CALL: return "R_SPARC_TLS_GD_CALL";
  case R_SPARC_TLS_LDM_HI22: return "R_SPARC_TLS_LDM_HI22";
  case R_SPARC_TLS_LDM_LO10: return "R_SPARC_TLS_LDM_LO10";
  case R_SPARC_TLS_LDM_ADD: return "R_SPARC_TLS_LDM_ADD";
  case R_SPARC_TLS_LDM_CALL: return "R_SPARC_TLS_LDM_CALL";
  case R_SPARC_TLS_LDO_HIX22: return "R_SPARC_TLS_LDO_HIX22";
  case R_SPARC_TLS_LDO_LOX10: return "R_SPARC_TLS_LDO_LOX10";
  case R_SPARC_TLS_LDO_ADD: return "R_SPARC_TLS_LDO_ADD";
  case R_SPARC_TLS_IE_HI22: return "R_SPARC_TLS_IE_HI22";
  case R_SPARC_TLS_IE_LO10: return "R_SPARC_TLS_IE_LO10";
  case R_SPARC_TLS_IE_LD: return "R_SPARC_TLS_IE_LD";
  case R_SPARC_TLS_IE_LDX: return "R_SPARC_TLS_IE_LDX";
  case R_SPARC_TLS_IE_ADD: return "R_SPARC_TLS_IE_ADD";
  case R_SPARC_TLS_LE_HIX22: return "R_SPARC_TLS_LE_HIX22";
  case R_SPARC_TLS_LE_LOX10: return "R_SPARC_TLS_LE_LOX10";
  case R_SPARC_TLS_DTPMOD32: return "R_SPARC_TLS_DTPMOD32";
  case R_SPARC_TLS_DTPMOD64: return "R_SPARC_TLS_DTPMOD64";
  case R_SPARC_TLS_DTPOFF32: return "R_SPARC_TLS_DTPOFF32";
  case R_SPARC_TLS_DTPOFF64: return "R_SPARC_TLS_DTPOFF64";
  case R_SPARC_TLS_TPOFF32: return "R_SPARC_TLS_TPOFF32";
  case R_SPARC_TLS_TPOFF64: return "R_SPARC_TLS_TPOFF64";
  case R_SPARC_GOTDATA_HIX22: return "R_SPARC_GOTDATA_HIX22";
  case R_SPARC_GOTDATA_LOX10: return "R_SPARC_GOTDATA_LOX10";
  case R_SPARC_GOTDATA_OP_HIX22: return "R_SPARC_GOTDATA_OP_HIX22";
  case R_SPARC_GOTDATA_OP_LOX10: return "R_SPARC_GOTDATA_OP_LOX10";
  case R_SPARC_GOTDATA_OP: return "R_SPARC_GOTDATA_OP";
  case R_SPARC_IRELATIVE: return "R_SPARC_IRELATIVE";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<S390X>(u32 r_type) {
  switch (r_type) {
  case R_390_NONE: return "R_390_NONE";
  case R_390_8: return "R_390_8";
  case R_390_12: return "R_390_12";
  case R_390_16: return "R_390_16";
  case R_390_32: return "R_390_32";
  case R_390_PC32: return "R_390_PC32";
  case R_390_GOT12: return "R_390_GOT12";
  case R_390_GOT32: return "R_390_GOT32";
  case R_390_PLT32: return "R_390_PLT32";
  case R_390_COPY: return "R_390_COPY";
  case R_390_GLOB_DAT: return "R_390_GLOB_DAT";
  case R_390_JMP_SLOT: return "R_390_JMP_SLOT";
  case R_390_RELATIVE: return "R_390_RELATIVE";
  case R_390_GOTOFF32: return "R_390_GOTOFF32";
  case R_390_GOTPC: return "R_390_GOTPC";
  case R_390_GOT16: return "R_390_GOT16";
  case R_390_PC16: return "R_390_PC16";
  case R_390_PC16DBL: return "R_390_PC16DBL";
  case R_390_PLT16DBL: return "R_390_PLT16DBL";
  case R_390_PC32DBL: return "R_390_PC32DBL";
  case R_390_PLT32DBL: return "R_390_PLT32DBL";
  case R_390_GOTPCDBL: return "R_390_GOTPCDBL";
  case R_390_64: return "R_390_64";
  case R_390_PC64: return "R_390_PC64";
  case R_390_GOT64: return "R_390_GOT64";
  case R_390_PLT64: return "R_390_PLT64";
  case R_390_GOTENT: return "R_390_GOTENT";
  case R_390_GOTOFF16: return "R_390_GOTOFF16";
  case R_390_GOTOFF64: return "R_390_GOTOFF64";
  case R_390_GOTPLT12: return "R_390_GOTPLT12";
  case R_390_GOTPLT16: return "R_390_GOTPLT16";
  case R_390_GOTPLT32: return "R_390_GOTPLT32";
  case R_390_GOTPLT64: return "R_390_GOTPLT64";
  case R_390_GOTPLTENT: return "R_390_GOTPLTENT";
  case R_390_PLTOFF16: return "R_390_PLTOFF16";
  case R_390_PLTOFF32: return "R_390_PLTOFF32";
  case R_390_PLTOFF64: return "R_390_PLTOFF64";
  case R_390_TLS_LOAD: return "R_390_TLS_LOAD";
  case R_390_TLS_GDCALL: return "R_390_TLS_GDCALL";
  case R_390_TLS_LDCALL: return "R_390_TLS_LDCALL";
  case R_390_TLS_GD32: return "R_390_TLS_GD32";
  case R_390_TLS_GD64: return "R_390_TLS_GD64";
  case R_390_TLS_GOTIE12: return "R_390_TLS_GOTIE12";
  case R_390_TLS_GOTIE32: return "R_390_TLS_GOTIE32";
  case R_390_TLS_GOTIE64: return "R_390_TLS_GOTIE64";
  case R_390_TLS_LDM32: return "R_390_TLS_LDM32";
  case R_390_TLS_LDM64: return "R_390_TLS_LDM64";
  case R_390_TLS_IE32: return "R_390_TLS_IE32";
  case R_390_TLS_IE64: return "R_390_TLS_IE64";
  case R_390_TLS_IEENT: return "R_390_TLS_IEENT";
  case R_390_TLS_LE32: return "R_390_TLS_LE32";
  case R_390_TLS_LE64: return "R_390_TLS_LE64";
  case R_390_TLS_LDO32: return "R_390_TLS_LDO32";
  case R_390_TLS_LDO64: return "R_390_TLS_LDO64";
  case R_390_TLS_DTPMOD: return "R_390_TLS_DTPMOD";
  case R_390_TLS_DTPOFF: return "R_390_TLS_DTPOFF";
  case R_390_TLS_TPOFF: return "R_390_TLS_TPOFF";
  case R_390_20: return "R_390_20";
  case R_390_GOT20: return "R_390_GOT20";
  case R_390_GOTPLT20: return "R_390_GOTPLT20";
  case R_390_TLS_GOTIE20: return "R_390_TLS_GOTIE20";
  case R_390_IRELATIVE: return "R_390_IRELATIVE";
  case R_390_PC12DBL: return "R_390_PC12DBL";
  case R_390_PLT12DBL: return "R_390_PLT12DBL";
  case R_390_PC24DBL: return "R_390_PC24DBL";
  case R_390_PLT24DBL: return "R_390_PLT24DBL";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<M68K>(u32 r_type) {
  switch (r_type) {
  case R_68K_NONE: return "R_68K_NONE";
  case R_68K_32: return "R_68K_32";
  case R_68K_16: return "R_68K_16";
  case R_68K_8: return "R_68K_8";
  case R_68K_PC32: return "R_68K_PC32";
  case R_68K_PC16: return "R_68K_PC16";
  case R_68K_PC8: return "R_68K_PC8";
  case R_68K_GOTPCREL32: return "R_68K_GOTPCREL32";
  case R_68K_GOTPCREL16: return "R_68K_GOTPCREL16";
  case R_68K_GOTPCREL8: return "R_68K_GOTPCREL8";
  case R_68K_GOTOFF32: return "R_68K_GOTOFF32";
  case R_68K_GOTOFF16: return "R_68K_GOTOFF16";
  case R_68K_GOTOFF8: return "R_68K_GOTOFF8";
  case R_68K_PLT32: return "R_68K_PLT32";
  case R_68K_PLT16: return "R_68K_PLT16";
  case R_68K_PLT8: return "R_68K_PLT8";
  case R_68K_PLTOFF32: return "R_68K_PLTOFF32";
  case R_68K_PLTOFF16: return "R_68K_PLTOFF16";
  case R_68K_PLTOFF8: return "R_68K_PLTOFF8";
  case R_68K_COPY: return "R_68K_COPY";
  case R_68K_GLOB_DAT: return "R_68K_GLOB_DAT";
  case R_68K_JMP_SLOT: return "R_68K_JMP_SLOT";
  case R_68K_RELATIVE: return "R_68K_RELATIVE";
  case R_68K_TLS_GD32: return "R_68K_TLS_GD32";
  case R_68K_TLS_GD16: return "R_68K_TLS_GD16";
  case R_68K_TLS_GD8: return "R_68K_TLS_GD8";
  case R_68K_TLS_LDM32: return "R_68K_TLS_LDM32";
  case R_68K_TLS_LDM16: return "R_68K_TLS_LDM16";
  case R_68K_TLS_LDM8: return "R_68K_TLS_LDM8";
  case R_68K_TLS_LDO32: return "R_68K_TLS_LDO32";
  case R_68K_TLS_LDO16: return "R_68K_TLS_LDO16";
  case R_68K_TLS_LDO8: return "R_68K_TLS_LDO8";
  case R_68K_TLS_IE32: return "R_68K_TLS_IE32";
  case R_68K_TLS_IE16: return "R_68K_TLS_IE16";
  case R_68K_TLS_IE8: return "R_68K_TLS_IE8";
  case R_68K_TLS_LE32: return "R_68K_TLS_LE32";
  case R_68K_TLS_LE16: return "R_68K_TLS_LE16";
  case R_68K_TLS_LE8: return "R_68K_TLS_LE8";
  case R_68K_TLS_DTPMOD32: return "R_68K_TLS_DTPMOD32";
  case R_68K_TLS_DTPREL32: return "R_68K_TLS_DTPREL32";
  case R_68K_TLS_TPREL32: return "R_68K_TLS_TPREL32";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<SH4>(u32 r_type) {
  switch (r_type) {
  case R_SH_NONE: return "R_SH_NONE";
  case R_SH_DIR32: return "R_SH_DIR32";
  case R_SH_REL32: return "R_SH_REL32";
  case R_SH_DIR8WPN: return "R_SH_DIR8WPN";
  case R_SH_IND12W: return "R_SH_IND12W";
  case R_SH_DIR8WPL: return "R_SH_DIR8WPL";
  case R_SH_DIR8WPZ: return "R_SH_DIR8WPZ";
  case R_SH_DIR8BP: return "R_SH_DIR8BP";
  case R_SH_DIR8W: return "R_SH_DIR8W";
  case R_SH_DIR8L: return "R_SH_DIR8L";
  case R_SH_TLS_GD_32: return "R_SH_TLS_GD_32";
  case R_SH_TLS_LD_32: return "R_SH_TLS_LD_32";
  case R_SH_TLS_LDO_32: return "R_SH_TLS_LDO_32";
  case R_SH_TLS_IE_32: return "R_SH_TLS_IE_32";
  case R_SH_TLS_LE_32: return "R_SH_TLS_LE_32";
  case R_SH_TLS_DTPMOD32: return "R_SH_TLS_DTPMOD32";
  case R_SH_TLS_DTPOFF32: return "R_SH_TLS_DTPOFF32";
  case R_SH_TLS_TPOFF32: return "R_SH_TLS_TPOFF32";
  case R_SH_GOT32: return "R_SH_GOT32";
  case R_SH_PLT32: return "R_SH_PLT32";
  case R_SH_COPY: return "R_SH_COPY";
  case R_SH_GLOB_DAT: return "R_SH_GLOB_DAT";
  case R_SH_JMP_SLOT: return "R_SH_JMP_SLOT";
  case R_SH_RELATIVE: return "R_SH_RELATIVE";
  case R_SH_GOTOFF: return "R_SH_GOTOFF";
  case R_SH_GOTPC: return "R_SH_GOTPC";
  case R_SH_GOTPLT32: return "R_SH_GOTPLT32";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

template <>
std::string rel_to_string<ALPHA>(u32 r_type) {
  switch (r_type) {
  case R_ALPHA_NONE: return "R_ALPHA_NONE";
  case R_ALPHA_REFLONG: return "R_ALPHA_REFLONG";
  case R_ALPHA_REFQUAD: return "R_ALPHA_REFQUAD";
  case R_ALPHA_GPREL32: return "R_ALPHA_GPREL32";
  case R_ALPHA_LITERAL: return "R_ALPHA_LITERAL";
  case R_ALPHA_LITUSE: return "R_ALPHA_LITUSE";
  case R_ALPHA_GPDISP: return "R_ALPHA_GPDISP";
  case R_ALPHA_BRADDR: return "R_ALPHA_BRADDR";
  case R_ALPHA_HINT: return "R_ALPHA_HINT";
  case R_ALPHA_SREL16: return "R_ALPHA_SREL16";
  case R_ALPHA_SREL32: return "R_ALPHA_SREL32";
  case R_ALPHA_SREL64: return "R_ALPHA_SREL64";
  case R_ALPHA_GPRELHIGH: return "R_ALPHA_GPRELHIGH";
  case R_ALPHA_GPRELLOW: return "R_ALPHA_GPRELLOW";
  case R_ALPHA_GPREL16: return "R_ALPHA_GPREL16";
  case R_ALPHA_COPY: return "R_ALPHA_COPY";
  case R_ALPHA_GLOB_DAT: return "R_ALPHA_GLOB_DAT";
  case R_ALPHA_JMP_SLOT: return "R_ALPHA_JMP_SLOT";
  case R_ALPHA_RELATIVE: return "R_ALPHA_RELATIVE";
  case R_ALPHA_BRSGP: return "R_ALPHA_BRSGP";
  case R_ALPHA_TLSGD: return "R_ALPHA_TLSGD";
  case R_ALPHA_TLSLDM: return "R_ALPHA_TLSLDM";
  case R_ALPHA_DTPMOD64: return "R_ALPHA_DTPMOD64";
  case R_ALPHA_GOTDTPREL: return "R_ALPHA_GOTDTPREL";
  case R_ALPHA_DTPREL64: return "R_ALPHA_DTPREL64";
  case R_ALPHA_DTPRELHI: return "R_ALPHA_DTPRELHI";
  case R_ALPHA_DTPRELLO: return "R_ALPHA_DTPRELLO";
  case R_ALPHA_DTPREL16: return "R_ALPHA_DTPREL16";
  case R_ALPHA_GOTTPREL: return "R_ALPHA_GOTTPREL";
  case R_ALPHA_TPREL64: return "R_ALPHA_TPREL64";
  case R_ALPHA_TPRELHI: return "R_ALPHA_TPRELHI";
  case R_ALPHA_TPRELLO: return "R_ALPHA_TPRELLO";
  case R_ALPHA_TPREL16: return "R_ALPHA_TPREL16";
  }
  return "unknown (" + std::to_string(r_type) + ")";
}

} // namespace mold::elf
