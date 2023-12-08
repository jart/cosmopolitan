/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/descrobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pystrhex.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("_sha3");
PYTHON_PROVIDE("_sha3.__doc__");
PYTHON_PROVIDE("_sha3.__loader__");
PYTHON_PROVIDE("_sha3.__name__");
PYTHON_PROVIDE("_sha3.__package__");
PYTHON_PROVIDE("_sha3.__spec__");
PYTHON_PROVIDE("_sha3.implementation");
PYTHON_PROVIDE("_sha3.keccakopt");
PYTHON_PROVIDE("_sha3.sha3_224");
PYTHON_PROVIDE("_sha3.sha3_256");
PYTHON_PROVIDE("_sha3.sha3_384");
PYTHON_PROVIDE("_sha3.sha3_512");
PYTHON_PROVIDE("_sha3.shake_128");
PYTHON_PROVIDE("_sha3.shake_256");

typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHLEN = 2 } HashReturn;

typedef struct KeccakWidth1600_SpongeInstanceStruct {
  unsigned char state[200] forcealign(8);
  unsigned int rate;
  unsigned int byteIOIndex;
  int squeezing;
} KeccakWidth1600_SpongeInstance;

typedef struct {
  KeccakWidth1600_SpongeInstance sponge;
  unsigned int fixedOutputLength;
  unsigned char delimitedSuffix;
} Keccak_HashInstance;

static const uint64_t kKeccakf[24] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
};

static void _PySHA3_KeccakP1600_Initialize(void *state) {
  bzero(state, 200);
  ((uint64_t *)state)[1] = ~(uint64_t)0;
  ((uint64_t *)state)[2] = ~(uint64_t)0;
  ((uint64_t *)state)[8] = ~(uint64_t)0;
  ((uint64_t *)state)[12] = ~(uint64_t)0;
  ((uint64_t *)state)[17] = ~(uint64_t)0;
  ((uint64_t *)state)[20] = ~(uint64_t)0;
}

static void _PySHA3_KeccakP1600_AddBytesInLane(void *state,
                                               unsigned int lanePosition,
                                               const unsigned char *data,
                                               unsigned int offset,
                                               unsigned int length) {
  uint64_t lane;
  if (length == 0) return;
  if (length == 1)
    lane = data[0];
  else {
    lane = 0;
    memcpy(&lane, data, length);
  }
  lane <<= offset * 8;
  ((uint64_t *)state)[lanePosition] ^= lane;
}

static void _PySHA3_KeccakP1600_AddLanes(void *state, const unsigned char *data,
                                         unsigned int laneCount) {
  unsigned int i = 0;
  for (; (i + 8) <= laneCount; i += 8) {
    ((uint64_t *)state)[i + 0] ^= ((uint64_t *)data)[i + 0];
    ((uint64_t *)state)[i + 1] ^= ((uint64_t *)data)[i + 1];
    ((uint64_t *)state)[i + 2] ^= ((uint64_t *)data)[i + 2];
    ((uint64_t *)state)[i + 3] ^= ((uint64_t *)data)[i + 3];
    ((uint64_t *)state)[i + 4] ^= ((uint64_t *)data)[i + 4];
    ((uint64_t *)state)[i + 5] ^= ((uint64_t *)data)[i + 5];
    ((uint64_t *)state)[i + 6] ^= ((uint64_t *)data)[i + 6];
    ((uint64_t *)state)[i + 7] ^= ((uint64_t *)data)[i + 7];
  }
  for (; (i + 4) <= laneCount; i += 4) {
    ((uint64_t *)state)[i + 0] ^= ((uint64_t *)data)[i + 0];
    ((uint64_t *)state)[i + 1] ^= ((uint64_t *)data)[i + 1];
    ((uint64_t *)state)[i + 2] ^= ((uint64_t *)data)[i + 2];
    ((uint64_t *)state)[i + 3] ^= ((uint64_t *)data)[i + 3];
  }
  for (; (i + 2) <= laneCount; i += 2) {
    ((uint64_t *)state)[i + 0] ^= ((uint64_t *)data)[i + 0];
    ((uint64_t *)state)[i + 1] ^= ((uint64_t *)data)[i + 1];
  }
  if (i < laneCount) {
    ((uint64_t *)state)[i + 0] ^= ((uint64_t *)data)[i + 0];
  }
}

static void _PySHA3_KeccakP1600_AddBytes(void *state, const unsigned char *data,
                                         unsigned int offset,
                                         unsigned int length) {
  if ((offset) == 0) {
    _PySHA3_KeccakP1600_AddLanes(state, data, (length) / 8);
    _PySHA3_KeccakP1600_AddBytesInLane(
        state, (length) / 8, (data) + ((length) / 8) * 8, 0, (length) % 8);
  } else {
    unsigned int _sizeLeft = (length);
    unsigned int _lanePosition = (offset) / 8;
    unsigned int _offsetInLane = (offset) % 8;
    const unsigned char *_curData = (data);
    while (_sizeLeft > 0) {
      unsigned int _bytesInLane = 8 - _offsetInLane;
      if (_bytesInLane > _sizeLeft) _bytesInLane = _sizeLeft;
      _PySHA3_KeccakP1600_AddBytesInLane(state, _lanePosition, _curData,
                                         _offsetInLane, _bytesInLane);
      _sizeLeft -= _bytesInLane;
      _lanePosition++;
      _offsetInLane = 0;
      _curData += _bytesInLane;
    }
  }
}

static void _PySHA3_KeccakP1600_OverwriteBytesInLane(void *state,
                                                     unsigned int lanePosition,
                                                     const unsigned char *data,
                                                     unsigned int offset,
                                                     unsigned int length) {
  if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
      (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20)) {
    unsigned int i;
    for (i = 0; i < length; i++)
      ((unsigned char *)state)[lanePosition * 8 + offset + i] = ~data[i];
  } else {
    memcpy((unsigned char *)state + lanePosition * 8 + offset, data, length);
  }
}

static void _PySHA3_KeccakP1600_OverwriteLanes(void *state,
                                               const unsigned char *data,
                                               unsigned int laneCount) {
  unsigned int lanePosition;
  for (lanePosition = 0; lanePosition < laneCount; lanePosition++)
    if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
        (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20))
      ((uint64_t *)state)[lanePosition] =
          ~((const uint64_t *)data)[lanePosition];
    else
      ((uint64_t *)state)[lanePosition] =
          ((const uint64_t *)data)[lanePosition];
}

static void _PySHA3_KeccakP1600_OverwriteBytes(void *state,
                                               const unsigned char *data,
                                               unsigned int offset,
                                               unsigned int length) {
  {
    if ((offset) == 0) {
      _PySHA3_KeccakP1600_OverwriteLanes(state, data, (length) / 8);
      _PySHA3_KeccakP1600_OverwriteBytesInLane(
          state, (length) / 8, (data) + ((length) / 8) * 8, 0, (length) % 8);
    } else {
      unsigned int _sizeLeft = (length);
      unsigned int _lanePosition = (offset) / 8;
      unsigned int _offsetInLane = (offset) % 8;
      const unsigned char *_curData = (data);
      while (_sizeLeft > 0) {
        unsigned int _bytesInLane = 8 - _offsetInLane;
        if (_bytesInLane > _sizeLeft) _bytesInLane = _sizeLeft;
        _PySHA3_KeccakP1600_OverwriteBytesInLane(state, _lanePosition, _curData,
                                                 _offsetInLane, _bytesInLane);
        _sizeLeft -= _bytesInLane;
        _lanePosition++;
        _offsetInLane = 0;
        _curData += _bytesInLane;
      }
    }
  };
}

static void _PySHA3_KeccakP1600_OverwriteWithZeroes(void *state,
                                                    unsigned int byteCount) {
  unsigned int lanePosition;
  for (lanePosition = 0; lanePosition < byteCount / 8; lanePosition++)
    if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
        (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20))
      ((uint64_t *)state)[lanePosition] = ~0;
    else
      ((uint64_t *)state)[lanePosition] = 0;
  if (byteCount % 8 != 0) {
    lanePosition = byteCount / 8;
    if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
        (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20))
      memset((unsigned char *)state + lanePosition * 8, 0xFF, byteCount % 8);
    else
      bzero((unsigned char *)state + lanePosition * 8, byteCount % 8);
  }
}

static void _PySHA3_KeccakP1600_Permute_24rounds(void *state) {
  uint64_t Ca, Ce, Ci, Co, Cu;
  uint64_t Da, De, Di, Do, Du;
  uint64_t Aba, Abe, Abi, Abo, Abu;
  uint64_t Aga, Age, Agi, Ago, Agu;
  uint64_t Aka, Ake, Aki, Ako, Aku;
  uint64_t Ama, Ame, Ami, Amo, Amu;
  uint64_t Asa, Ase, Asi, Aso, Asu;
  uint64_t Bba, Bbe, Bbi, Bbo, Bbu;
  uint64_t Bga, Bge, Bgi, Bgo, Bgu;
  uint64_t Bka, Bke, Bki, Bko, Bku;
  uint64_t Bma, Bme, Bmi, Bmo, Bmu;
  uint64_t Bsa, Bse, Bsi, Bso, Bsu;
  uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
  uint64_t Ega, Ege, Egi, Ego, Egu;
  uint64_t Eka, Eke, Eki, Eko, Eku;
  uint64_t Ema, Eme, Emi, Emo, Emu;
  uint64_t Esa, Ese, Esi, Eso, Esu;
  uint64_t *stateAsLanes = (uint64_t *)state;
  Aba = stateAsLanes[0];
  Abe = stateAsLanes[1];
  Abi = stateAsLanes[2];
  Abo = stateAsLanes[3];
  Abu = stateAsLanes[4];
  Aga = stateAsLanes[5];
  Age = stateAsLanes[6];
  Agi = stateAsLanes[7];
  Ago = stateAsLanes[8];
  Agu = stateAsLanes[9];
  Aka = stateAsLanes[10];
  Ake = stateAsLanes[11];
  Aki = stateAsLanes[12];
  Ako = stateAsLanes[13];
  Aku = stateAsLanes[14];
  Ama = stateAsLanes[15];
  Ame = stateAsLanes[16];
  Ami = stateAsLanes[17];
  Amo = stateAsLanes[18];
  Amu = stateAsLanes[19];
  Asa = stateAsLanes[20];
  Ase = stateAsLanes[21];
  Asi = stateAsLanes[22];
  Aso = stateAsLanes[23];
  Asu = stateAsLanes[24];
  Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
  Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
  Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
  Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
  Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[0];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[1];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[2];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[3];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[4];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[5];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[6];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[7];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[8];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[9];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[10];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[11];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[12];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[13];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[14];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[15];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[16];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[17];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[18];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[19];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[20];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[21];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[22];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[23];
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Abi = Bbi ^ (Bbo & Bbu);
  Abo = Bbo ^ (Bbu | Bba);
  Abu = Bbu ^ (Bba & Bbe);
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Age = Bge ^ (Bgi & Bgo);
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ago = Bgo ^ (Bgu | Bga);
  Agu = Bgu ^ (Bga & Bge);
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ake = Bke ^ (Bki & Bko);
  Aki = Bki ^ ((~Bko) & Bku);
  Ako = (~Bko) ^ (Bku | Bka);
  Aku = Bku ^ (Bka & Bke);
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ame = Bme ^ (Bmi | Bmo);
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Amo = (~Bmo) ^ (Bmu & Bma);
  Amu = Bmu ^ (Bma | Bme);
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ase = (~Bse) ^ (Bsi | Bso);
  Asi = Bsi ^ (Bso & Bsu);
  Aso = Bso ^ (Bsu | Bsa);
  Asu = Bsu ^ (Bsa & Bse);
  stateAsLanes[0] = Aba;
  stateAsLanes[1] = Abe;
  stateAsLanes[2] = Abi;
  stateAsLanes[3] = Abo;
  stateAsLanes[4] = Abu;
  stateAsLanes[5] = Aga;
  stateAsLanes[6] = Age;
  stateAsLanes[7] = Agi;
  stateAsLanes[8] = Ago;
  stateAsLanes[9] = Agu;
  stateAsLanes[10] = Aka;
  stateAsLanes[11] = Ake;
  stateAsLanes[12] = Aki;
  stateAsLanes[13] = Ako;
  stateAsLanes[14] = Aku;
  stateAsLanes[15] = Ama;
  stateAsLanes[16] = Ame;
  stateAsLanes[17] = Ami;
  stateAsLanes[18] = Amo;
  stateAsLanes[19] = Amu;
  stateAsLanes[20] = Asa;
  stateAsLanes[21] = Ase;
  stateAsLanes[22] = Asi;
  stateAsLanes[23] = Aso;
  stateAsLanes[24] = Asu;
}

static void _PySHA3_KeccakP1600_Permute_12rounds(void *state) {
  uint64_t Aba, Abe, Abi, Abo, Abu;
  uint64_t Aga, Age, Agi, Ago, Agu;
  uint64_t Aka, Ake, Aki, Ako, Aku;
  uint64_t Ama, Ame, Ami, Amo, Amu;
  uint64_t Asa, Ase, Asi, Aso, Asu;
  uint64_t Bba, Bbe, Bbi, Bbo, Bbu;
  uint64_t Bga, Bge, Bgi, Bgo, Bgu;
  uint64_t Bka, Bke, Bki, Bko, Bku;
  uint64_t Bma, Bme, Bmi, Bmo, Bmu;
  uint64_t Bsa, Bse, Bsi, Bso, Bsu;
  uint64_t Ca, Ce, Ci, Co, Cu;
  uint64_t Da, De, Di, Do, Du;
  uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
  uint64_t Ega, Ege, Egi, Ego, Egu;
  uint64_t Eka, Eke, Eki, Eko, Eku;
  uint64_t Ema, Eme, Emi, Emo, Emu;
  uint64_t Esa, Ese, Esi, Eso, Esu;
  uint64_t *stateAsLanes = (uint64_t *)state;
  Aba = stateAsLanes[0];
  Abe = stateAsLanes[1];
  Abi = stateAsLanes[2];
  Abo = stateAsLanes[3];
  Abu = stateAsLanes[4];
  Aga = stateAsLanes[5];
  Age = stateAsLanes[6];
  Agi = stateAsLanes[7];
  Ago = stateAsLanes[8];
  Agu = stateAsLanes[9];
  Aka = stateAsLanes[10];
  Ake = stateAsLanes[11];
  Aki = stateAsLanes[12];
  Ako = stateAsLanes[13];
  Aku = stateAsLanes[14];
  Ama = stateAsLanes[15];
  Ame = stateAsLanes[16];
  Ami = stateAsLanes[17];
  Amo = stateAsLanes[18];
  Amu = stateAsLanes[19];
  Asa = stateAsLanes[20];
  Ase = stateAsLanes[21];
  Asi = stateAsLanes[22];
  Aso = stateAsLanes[23];
  Asu = stateAsLanes[24];
  Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
  Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
  Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
  Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
  Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[12];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[13];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[14];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[15];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[16];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[17];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[18];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[19];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[20];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[21];
  Ca = Aba;
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Abe;
  Abi = Bbi ^ (Bbo & Bbu);
  Ci = Abi;
  Abo = Bbo ^ (Bbu | Bba);
  Co = Abo;
  Abu = Bbu ^ (Bba & Bbe);
  Cu = Abu;
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Ca ^= Aga;
  Age = Bge ^ (Bgi & Bgo);
  Ce ^= Age;
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Agi;
  Ago = Bgo ^ (Bgu | Bga);
  Co ^= Ago;
  Agu = Bgu ^ (Bga & Bge);
  Cu ^= Agu;
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ca ^= Aka;
  Ake = Bke ^ (Bki & Bko);
  Ce ^= Ake;
  Aki = Bki ^ ((~Bko) & Bku);
  Ci ^= Aki;
  Ako = (~Bko) ^ (Bku | Bka);
  Co ^= Ako;
  Aku = Bku ^ (Bka & Bke);
  Cu ^= Aku;
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ca ^= Ama;
  Ame = Bme ^ (Bmi | Bmo);
  Ce ^= Ame;
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Ami;
  Amo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Amo;
  Amu = Bmu ^ (Bma | Bme);
  Cu ^= Amu;
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Asa;
  Ase = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ase;
  Asi = Bsi ^ (Bso & Bsu);
  Ci ^= Asi;
  Aso = Bso ^ (Bsu | Bsa);
  Co ^= Aso;
  Asu = Bsu ^ (Bsa & Bse);
  Cu ^= Asu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Aba ^= Da;
  Bba = Aba;
  Age ^= De;
  Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
  Aki ^= Di;
  Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
  Amo ^= Do;
  Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
  Asu ^= Du;
  Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
  Eba = Bba ^ (Bbe | Bbi);
  Eba ^= kKeccakf[22];
  Ca = Eba;
  Ebe = Bbe ^ ((~Bbi) | Bbo);
  Ce = Ebe;
  Ebi = Bbi ^ (Bbo & Bbu);
  Ci = Ebi;
  Ebo = Bbo ^ (Bbu | Bba);
  Co = Ebo;
  Ebu = Bbu ^ (Bba & Bbe);
  Cu = Ebu;
  Abo ^= Do;
  Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
  Agu ^= Du;
  Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
  Aka ^= Da;
  Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
  Ame ^= De;
  Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
  Asi ^= Di;
  Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
  Ega = Bga ^ (Bge | Bgi);
  Ca ^= Ega;
  Ege = Bge ^ (Bgi & Bgo);
  Ce ^= Ege;
  Egi = Bgi ^ (Bgo | (~Bgu));
  Ci ^= Egi;
  Ego = Bgo ^ (Bgu | Bga);
  Co ^= Ego;
  Egu = Bgu ^ (Bga & Bge);
  Cu ^= Egu;
  Abe ^= De;
  Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
  Agi ^= Di;
  Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
  Ako ^= Do;
  Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
  Amu ^= Du;
  Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
  Asa ^= Da;
  Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
  Eka = Bka ^ (Bke | Bki);
  Ca ^= Eka;
  Eke = Bke ^ (Bki & Bko);
  Ce ^= Eke;
  Eki = Bki ^ ((~Bko) & Bku);
  Ci ^= Eki;
  Eko = (~Bko) ^ (Bku | Bka);
  Co ^= Eko;
  Eku = Bku ^ (Bka & Bke);
  Cu ^= Eku;
  Abu ^= Du;
  Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
  Aga ^= Da;
  Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
  Ake ^= De;
  Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
  Ami ^= Di;
  Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
  Aso ^= Do;
  Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
  Ema = Bma ^ (Bme & Bmi);
  Ca ^= Ema;
  Eme = Bme ^ (Bmi | Bmo);
  Ce ^= Eme;
  Emi = Bmi ^ ((~Bmo) | Bmu);
  Ci ^= Emi;
  Emo = (~Bmo) ^ (Bmu & Bma);
  Co ^= Emo;
  Emu = Bmu ^ (Bma | Bme);
  Cu ^= Emu;
  Abi ^= Di;
  Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
  Ago ^= Do;
  Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
  Aku ^= Du;
  Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
  Ama ^= Da;
  Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
  Ase ^= De;
  Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
  Esa = Bsa ^ ((~Bse) & Bsi);
  Ca ^= Esa;
  Ese = (~Bse) ^ (Bsi | Bso);
  Ce ^= Ese;
  Esi = Bsi ^ (Bso & Bsu);
  Ci ^= Esi;
  Eso = Bso ^ (Bsu | Bsa);
  Co ^= Eso;
  Esu = Bsu ^ (Bsa & Bse);
  Cu ^= Esu;
  Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
  De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
  Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
  Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
  Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
  Eba ^= Da;
  Bba = Eba;
  Ege ^= De;
  Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
  Eki ^= Di;
  Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
  Emo ^= Do;
  Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
  Esu ^= Du;
  Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
  Aba = Bba ^ (Bbe | Bbi);
  Aba ^= kKeccakf[23];
  Abe = Bbe ^ ((~Bbi) | Bbo);
  Abi = Bbi ^ (Bbo & Bbu);
  Abo = Bbo ^ (Bbu | Bba);
  Abu = Bbu ^ (Bba & Bbe);
  Ebo ^= Do;
  Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
  Egu ^= Du;
  Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
  Eka ^= Da;
  Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
  Eme ^= De;
  Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
  Esi ^= Di;
  Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
  Aga = Bga ^ (Bge | Bgi);
  Age = Bge ^ (Bgi & Bgo);
  Agi = Bgi ^ (Bgo | (~Bgu));
  Ago = Bgo ^ (Bgu | Bga);
  Agu = Bgu ^ (Bga & Bge);
  Ebe ^= De;
  Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
  Egi ^= Di;
  Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
  Eko ^= Do;
  Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
  Emu ^= Du;
  Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
  Esa ^= Da;
  Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
  Aka = Bka ^ (Bke | Bki);
  Ake = Bke ^ (Bki & Bko);
  Aki = Bki ^ ((~Bko) & Bku);
  Ako = (~Bko) ^ (Bku | Bka);
  Aku = Bku ^ (Bka & Bke);
  Ebu ^= Du;
  Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
  Ega ^= Da;
  Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
  Eke ^= De;
  Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
  Emi ^= Di;
  Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
  Eso ^= Do;
  Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
  Ama = Bma ^ (Bme & Bmi);
  Ame = Bme ^ (Bmi | Bmo);
  Ami = Bmi ^ ((~Bmo) | Bmu);
  Amo = (~Bmo) ^ (Bmu & Bma);
  Amu = Bmu ^ (Bma | Bme);
  Ebi ^= Di;
  Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
  Ego ^= Do;
  Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
  Eku ^= Du;
  Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
  Ema ^= Da;
  Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
  Ese ^= De;
  Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
  Asa = Bsa ^ ((~Bse) & Bsi);
  Ase = (~Bse) ^ (Bsi | Bso);
  Asi = Bsi ^ (Bso & Bsu);
  Aso = Bso ^ (Bsu | Bsa);
  Asu = Bsu ^ (Bsa & Bse);
  stateAsLanes[0] = Aba;
  stateAsLanes[1] = Abe;
  stateAsLanes[2] = Abi;
  stateAsLanes[3] = Abo;
  stateAsLanes[4] = Abu;
  stateAsLanes[5] = Aga;
  stateAsLanes[6] = Age;
  stateAsLanes[7] = Agi;
  stateAsLanes[8] = Ago;
  stateAsLanes[9] = Agu;
  stateAsLanes[10] = Aka;
  stateAsLanes[11] = Ake;
  stateAsLanes[12] = Aki;
  stateAsLanes[13] = Ako;
  stateAsLanes[14] = Aku;
  stateAsLanes[15] = Ama;
  stateAsLanes[16] = Ame;
  stateAsLanes[17] = Ami;
  stateAsLanes[18] = Amo;
  stateAsLanes[19] = Amu;
  stateAsLanes[20] = Asa;
  stateAsLanes[21] = Ase;
  stateAsLanes[22] = Asi;
  stateAsLanes[23] = Aso;
  stateAsLanes[24] = Asu;
}

static void _PySHA3_KeccakP1600_ExtractBytesInLane(const void *state,
                                                   unsigned int lanePosition,
                                                   unsigned char *data,
                                                   unsigned int offset,
                                                   unsigned int length) {
  uint64_t lane = ((uint64_t *)state)[lanePosition];
  if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
      (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20))
    lane = ~lane;
  {
    uint64_t lane1[1];
    lane1[0] = lane;
    memcpy(data, (uint8_t *)lane1 + offset, length);
  }
}

static void _PySHA3_KeccakP1600_ExtractLanes(const void *state,
                                             unsigned char *data,
                                             unsigned int laneCount) {
  memcpy(data, state, laneCount * 8);
  if (laneCount > 1) {
    ((uint64_t *)data)[1] = ~((uint64_t *)data)[1];
    if (laneCount > 2) {
      ((uint64_t *)data)[2] = ~((uint64_t *)data)[2];
      if (laneCount > 8) {
        ((uint64_t *)data)[8] = ~((uint64_t *)data)[8];
        if (laneCount > 12) {
          ((uint64_t *)data)[12] = ~((uint64_t *)data)[12];
          if (laneCount > 17) {
            ((uint64_t *)data)[17] = ~((uint64_t *)data)[17];
            if (laneCount > 20) {
              ((uint64_t *)data)[20] = ~((uint64_t *)data)[20];
            }
          }
        }
      }
    }
  }
}

static void _PySHA3_KeccakP1600_ExtractBytes(const void *state,
                                             unsigned char *data,
                                             unsigned int offset,
                                             unsigned int length) {
  if ((offset) == 0) {
    _PySHA3_KeccakP1600_ExtractLanes(state, data, (length) / 8);
    _PySHA3_KeccakP1600_ExtractBytesInLane(
        state, (length) / 8, (data) + ((length) / 8) * 8, 0, (length) % 8);
  } else {
    unsigned int _sizeLeft = (length);
    unsigned int _lanePosition = (offset) / 8;
    unsigned int _offsetInLane = (offset) % 8;
    unsigned char *_curData = (data);
    while (_sizeLeft > 0) {
      unsigned int _bytesInLane = 8 - _offsetInLane;
      if (_bytesInLane > _sizeLeft) _bytesInLane = _sizeLeft;
      _PySHA3_KeccakP1600_ExtractBytesInLane(state, _lanePosition, _curData,
                                             _offsetInLane, _bytesInLane);
      _sizeLeft -= _bytesInLane;
      _lanePosition++;
      _offsetInLane = 0;
      _curData += _bytesInLane;
    }
  }
}

static size_t _PySHA3_KeccakF1600_FastLoop_Absorb(void *state,
                                                  unsigned int laneCount,
                                                  const unsigned char *data,
                                                  size_t dataByteLen) {
  size_t originalDataByteLen = dataByteLen;
  uint64_t Aba, Abe, Abi, Abo, Abu;
  uint64_t Aga, Age, Agi, Ago, Agu;
  uint64_t Aka, Ake, Aki, Ako, Aku;
  uint64_t Ama, Ame, Ami, Amo, Amu;
  uint64_t Asa, Ase, Asi, Aso, Asu;
  uint64_t Bba, Bbe, Bbi, Bbo, Bbu;
  uint64_t Bga, Bge, Bgi, Bgo, Bgu;
  uint64_t Bka, Bke, Bki, Bko, Bku;
  uint64_t Bma, Bme, Bmi, Bmo, Bmu;
  uint64_t Bsa, Bse, Bsi, Bso, Bsu;
  uint64_t Ca, Ce, Ci, Co, Cu;
  uint64_t Da, De, Di, Do, Du;
  uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
  uint64_t Ega, Ege, Egi, Ego, Egu;
  uint64_t Eka, Eke, Eki, Eko, Eku;
  uint64_t Ema, Eme, Emi, Emo, Emu;
  uint64_t Esa, Ese, Esi, Eso, Esu;
  uint64_t *stateAsLanes = (uint64_t *)state;
  uint64_t *inDataAsLanes = (uint64_t *)data;
  Aba = stateAsLanes[0];
  Abe = stateAsLanes[1];
  Abi = stateAsLanes[2];
  Abo = stateAsLanes[3];
  Abu = stateAsLanes[4];
  Aga = stateAsLanes[5];
  Age = stateAsLanes[6];
  Agi = stateAsLanes[7];
  Ago = stateAsLanes[8];
  Agu = stateAsLanes[9];
  Aka = stateAsLanes[10];
  Ake = stateAsLanes[11];
  Aki = stateAsLanes[12];
  Ako = stateAsLanes[13];
  Aku = stateAsLanes[14];
  Ama = stateAsLanes[15];
  Ame = stateAsLanes[16];
  Ami = stateAsLanes[17];
  Amo = stateAsLanes[18];
  Amu = stateAsLanes[19];
  Asa = stateAsLanes[20];
  Ase = stateAsLanes[21];
  Asi = stateAsLanes[22];
  Aso = stateAsLanes[23];
  Asu = stateAsLanes[24];
  while (dataByteLen >= laneCount * 8) {
    if (laneCount == 21) {
      Aba ^= inDataAsLanes[0];
      Abe ^= inDataAsLanes[1];
      Abi ^= inDataAsLanes[2];
      Abo ^= inDataAsLanes[3];
      Abu ^= inDataAsLanes[4];
      Aga ^= inDataAsLanes[5];
      Age ^= inDataAsLanes[6];
      Agi ^= inDataAsLanes[7];
      Ago ^= inDataAsLanes[8];
      Agu ^= inDataAsLanes[9];
      Aka ^= inDataAsLanes[10];
      Ake ^= inDataAsLanes[11];
      Aki ^= inDataAsLanes[12];
      Ako ^= inDataAsLanes[13];
      Aku ^= inDataAsLanes[14];
      Ama ^= inDataAsLanes[15];
      Ame ^= inDataAsLanes[16];
      Ami ^= inDataAsLanes[17];
      Amo ^= inDataAsLanes[18];
      Amu ^= inDataAsLanes[19];
      Asa ^= inDataAsLanes[20];
    } else if (laneCount < 16) {
      if (laneCount < 8) {
        if (laneCount < 4) {
          if (laneCount < 2) {
            if (laneCount < 1) {
            } else {
              Aba ^= inDataAsLanes[0];
            }
          } else {
            Aba ^= inDataAsLanes[0];
            Abe ^= inDataAsLanes[1];
            if (laneCount < 3) {
            } else {
              Abi ^= inDataAsLanes[2];
            }
          }
        } else {
          Aba ^= inDataAsLanes[0];
          Abe ^= inDataAsLanes[1];
          Abi ^= inDataAsLanes[2];
          Abo ^= inDataAsLanes[3];
          if (laneCount < 6) {
            if (laneCount < 5) {
            } else {
              Abu ^= inDataAsLanes[4];
            }
          } else {
            Abu ^= inDataAsLanes[4];
            Aga ^= inDataAsLanes[5];
            if (laneCount < 7) {
            } else {
              Age ^= inDataAsLanes[6];
            }
          }
        }
      } else {
        Aba ^= inDataAsLanes[0];
        Abe ^= inDataAsLanes[1];
        Abi ^= inDataAsLanes[2];
        Abo ^= inDataAsLanes[3];
        Abu ^= inDataAsLanes[4];
        Aga ^= inDataAsLanes[5];
        Age ^= inDataAsLanes[6];
        Agi ^= inDataAsLanes[7];
        if (laneCount < 12) {
          if (laneCount < 10) {
            if (laneCount < 9) {
            } else {
              Ago ^= inDataAsLanes[8];
            }
          } else {
            Ago ^= inDataAsLanes[8];
            Agu ^= inDataAsLanes[9];
            if (laneCount < 11) {
            } else {
              Aka ^= inDataAsLanes[10];
            }
          }
        } else {
          Ago ^= inDataAsLanes[8];
          Agu ^= inDataAsLanes[9];
          Aka ^= inDataAsLanes[10];
          Ake ^= inDataAsLanes[11];
          if (laneCount < 14) {
            if (laneCount < 13) {
            } else {
              Aki ^= inDataAsLanes[12];
            }
          } else {
            Aki ^= inDataAsLanes[12];
            Ako ^= inDataAsLanes[13];
            if (laneCount < 15) {
            } else {
              Aku ^= inDataAsLanes[14];
            }
          }
        }
      }
    } else {
      Aba ^= inDataAsLanes[0];
      Abe ^= inDataAsLanes[1];
      Abi ^= inDataAsLanes[2];
      Abo ^= inDataAsLanes[3];
      Abu ^= inDataAsLanes[4];
      Aga ^= inDataAsLanes[5];
      Age ^= inDataAsLanes[6];
      Agi ^= inDataAsLanes[7];
      Ago ^= inDataAsLanes[8];
      Agu ^= inDataAsLanes[9];
      Aka ^= inDataAsLanes[10];
      Ake ^= inDataAsLanes[11];
      Aki ^= inDataAsLanes[12];
      Ako ^= inDataAsLanes[13];
      Aku ^= inDataAsLanes[14];
      Ama ^= inDataAsLanes[15];
      if (laneCount < 24) {
        if (laneCount < 20) {
          if (laneCount < 18) {
            if (laneCount < 17) {
            } else {
              Ame ^= inDataAsLanes[16];
            }
          } else {
            Ame ^= inDataAsLanes[16];
            Ami ^= inDataAsLanes[17];
            if (laneCount < 19) {
            } else {
              Amo ^= inDataAsLanes[18];
            }
          }
        } else {
          Ame ^= inDataAsLanes[16];
          Ami ^= inDataAsLanes[17];
          Amo ^= inDataAsLanes[18];
          Amu ^= inDataAsLanes[19];
          if (laneCount < 22) {
            if (laneCount < 21) {
            } else {
              Asa ^= inDataAsLanes[20];
            }
          } else {
            Asa ^= inDataAsLanes[20];
            Ase ^= inDataAsLanes[21];
            if (laneCount < 23) {
            } else {
              Asi ^= inDataAsLanes[22];
            }
          }
        }
      } else {
        Ame ^= inDataAsLanes[16];
        Ami ^= inDataAsLanes[17];
        Amo ^= inDataAsLanes[18];
        Amu ^= inDataAsLanes[19];
        Asa ^= inDataAsLanes[20];
        Ase ^= inDataAsLanes[21];
        Asi ^= inDataAsLanes[22];
        Aso ^= inDataAsLanes[23];
        if (laneCount < 25) {
        } else {
          Asu ^= inDataAsLanes[24];
        }
      }
    }
    Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
    Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
    Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
    Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
    Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[0];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[1];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[2];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[3];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[4];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[5];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[6];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[7];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[8];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[9];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[10];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[11];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[12];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[13];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[14];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[15];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[16];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[17];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[18];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[19];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[20];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[21];
    Ca = Aba;
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Abe;
    Abi = Bbi ^ (Bbo & Bbu);
    Ci = Abi;
    Abo = Bbo ^ (Bbu | Bba);
    Co = Abo;
    Abu = Bbu ^ (Bba & Bbe);
    Cu = Abu;
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Ca ^= Aga;
    Age = Bge ^ (Bgi & Bgo);
    Ce ^= Age;
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Agi;
    Ago = Bgo ^ (Bgu | Bga);
    Co ^= Ago;
    Agu = Bgu ^ (Bga & Bge);
    Cu ^= Agu;
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ca ^= Aka;
    Ake = Bke ^ (Bki & Bko);
    Ce ^= Ake;
    Aki = Bki ^ ((~Bko) & Bku);
    Ci ^= Aki;
    Ako = (~Bko) ^ (Bku | Bka);
    Co ^= Ako;
    Aku = Bku ^ (Bka & Bke);
    Cu ^= Aku;
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ca ^= Ama;
    Ame = Bme ^ (Bmi | Bmo);
    Ce ^= Ame;
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Ami;
    Amo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Amo;
    Amu = Bmu ^ (Bma | Bme);
    Cu ^= Amu;
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Asa;
    Ase = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ase;
    Asi = Bsi ^ (Bso & Bsu);
    Ci ^= Asi;
    Aso = Bso ^ (Bsu | Bsa);
    Co ^= Aso;
    Asu = Bsu ^ (Bsa & Bse);
    Cu ^= Asu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Aba ^= Da;
    Bba = Aba;
    Age ^= De;
    Bbe = ((((uint64_t)Age) << 44) ^ (((uint64_t)Age) >> (64 - 44)));
    Aki ^= Di;
    Bbi = ((((uint64_t)Aki) << 43) ^ (((uint64_t)Aki) >> (64 - 43)));
    Amo ^= Do;
    Bbo = ((((uint64_t)Amo) << 21) ^ (((uint64_t)Amo) >> (64 - 21)));
    Asu ^= Du;
    Bbu = ((((uint64_t)Asu) << 14) ^ (((uint64_t)Asu) >> (64 - 14)));
    Eba = Bba ^ (Bbe | Bbi);
    Eba ^= kKeccakf[22];
    Ca = Eba;
    Ebe = Bbe ^ ((~Bbi) | Bbo);
    Ce = Ebe;
    Ebi = Bbi ^ (Bbo & Bbu);
    Ci = Ebi;
    Ebo = Bbo ^ (Bbu | Bba);
    Co = Ebo;
    Ebu = Bbu ^ (Bba & Bbe);
    Cu = Ebu;
    Abo ^= Do;
    Bga = ((((uint64_t)Abo) << 28) ^ (((uint64_t)Abo) >> (64 - 28)));
    Agu ^= Du;
    Bge = ((((uint64_t)Agu) << 20) ^ (((uint64_t)Agu) >> (64 - 20)));
    Aka ^= Da;
    Bgi = ((((uint64_t)Aka) << 3) ^ (((uint64_t)Aka) >> (64 - 3)));
    Ame ^= De;
    Bgo = ((((uint64_t)Ame) << 45) ^ (((uint64_t)Ame) >> (64 - 45)));
    Asi ^= Di;
    Bgu = ((((uint64_t)Asi) << 61) ^ (((uint64_t)Asi) >> (64 - 61)));
    Ega = Bga ^ (Bge | Bgi);
    Ca ^= Ega;
    Ege = Bge ^ (Bgi & Bgo);
    Ce ^= Ege;
    Egi = Bgi ^ (Bgo | (~Bgu));
    Ci ^= Egi;
    Ego = Bgo ^ (Bgu | Bga);
    Co ^= Ego;
    Egu = Bgu ^ (Bga & Bge);
    Cu ^= Egu;
    Abe ^= De;
    Bka = ((((uint64_t)Abe) << 1) ^ (((uint64_t)Abe) >> (64 - 1)));
    Agi ^= Di;
    Bke = ((((uint64_t)Agi) << 6) ^ (((uint64_t)Agi) >> (64 - 6)));
    Ako ^= Do;
    Bki = ((((uint64_t)Ako) << 25) ^ (((uint64_t)Ako) >> (64 - 25)));
    Amu ^= Du;
    Bko = ((((uint64_t)Amu) << 8) ^ (((uint64_t)Amu) >> (64 - 8)));
    Asa ^= Da;
    Bku = ((((uint64_t)Asa) << 18) ^ (((uint64_t)Asa) >> (64 - 18)));
    Eka = Bka ^ (Bke | Bki);
    Ca ^= Eka;
    Eke = Bke ^ (Bki & Bko);
    Ce ^= Eke;
    Eki = Bki ^ ((~Bko) & Bku);
    Ci ^= Eki;
    Eko = (~Bko) ^ (Bku | Bka);
    Co ^= Eko;
    Eku = Bku ^ (Bka & Bke);
    Cu ^= Eku;
    Abu ^= Du;
    Bma = ((((uint64_t)Abu) << 27) ^ (((uint64_t)Abu) >> (64 - 27)));
    Aga ^= Da;
    Bme = ((((uint64_t)Aga) << 36) ^ (((uint64_t)Aga) >> (64 - 36)));
    Ake ^= De;
    Bmi = ((((uint64_t)Ake) << 10) ^ (((uint64_t)Ake) >> (64 - 10)));
    Ami ^= Di;
    Bmo = ((((uint64_t)Ami) << 15) ^ (((uint64_t)Ami) >> (64 - 15)));
    Aso ^= Do;
    Bmu = ((((uint64_t)Aso) << 56) ^ (((uint64_t)Aso) >> (64 - 56)));
    Ema = Bma ^ (Bme & Bmi);
    Ca ^= Ema;
    Eme = Bme ^ (Bmi | Bmo);
    Ce ^= Eme;
    Emi = Bmi ^ ((~Bmo) | Bmu);
    Ci ^= Emi;
    Emo = (~Bmo) ^ (Bmu & Bma);
    Co ^= Emo;
    Emu = Bmu ^ (Bma | Bme);
    Cu ^= Emu;
    Abi ^= Di;
    Bsa = ((((uint64_t)Abi) << 62) ^ (((uint64_t)Abi) >> (64 - 62)));
    Ago ^= Do;
    Bse = ((((uint64_t)Ago) << 55) ^ (((uint64_t)Ago) >> (64 - 55)));
    Aku ^= Du;
    Bsi = ((((uint64_t)Aku) << 39) ^ (((uint64_t)Aku) >> (64 - 39)));
    Ama ^= Da;
    Bso = ((((uint64_t)Ama) << 41) ^ (((uint64_t)Ama) >> (64 - 41)));
    Ase ^= De;
    Bsu = ((((uint64_t)Ase) << 2) ^ (((uint64_t)Ase) >> (64 - 2)));
    Esa = Bsa ^ ((~Bse) & Bsi);
    Ca ^= Esa;
    Ese = (~Bse) ^ (Bsi | Bso);
    Ce ^= Ese;
    Esi = Bsi ^ (Bso & Bsu);
    Ci ^= Esi;
    Eso = Bso ^ (Bsu | Bsa);
    Co ^= Eso;
    Esu = Bsu ^ (Bsa & Bse);
    Cu ^= Esu;
    Da = Cu ^ ((((uint64_t)Ce) << 1) ^ (((uint64_t)Ce) >> (64 - 1)));
    De = Ca ^ ((((uint64_t)Ci) << 1) ^ (((uint64_t)Ci) >> (64 - 1)));
    Di = Ce ^ ((((uint64_t)Co) << 1) ^ (((uint64_t)Co) >> (64 - 1)));
    Do = Ci ^ ((((uint64_t)Cu) << 1) ^ (((uint64_t)Cu) >> (64 - 1)));
    Du = Co ^ ((((uint64_t)Ca) << 1) ^ (((uint64_t)Ca) >> (64 - 1)));
    Eba ^= Da;
    Bba = Eba;
    Ege ^= De;
    Bbe = ((((uint64_t)Ege) << 44) ^ (((uint64_t)Ege) >> (64 - 44)));
    Eki ^= Di;
    Bbi = ((((uint64_t)Eki) << 43) ^ (((uint64_t)Eki) >> (64 - 43)));
    Emo ^= Do;
    Bbo = ((((uint64_t)Emo) << 21) ^ (((uint64_t)Emo) >> (64 - 21)));
    Esu ^= Du;
    Bbu = ((((uint64_t)Esu) << 14) ^ (((uint64_t)Esu) >> (64 - 14)));
    Aba = Bba ^ (Bbe | Bbi);
    Aba ^= kKeccakf[23];
    Abe = Bbe ^ ((~Bbi) | Bbo);
    Abi = Bbi ^ (Bbo & Bbu);
    Abo = Bbo ^ (Bbu | Bba);
    Abu = Bbu ^ (Bba & Bbe);
    Ebo ^= Do;
    Bga = ((((uint64_t)Ebo) << 28) ^ (((uint64_t)Ebo) >> (64 - 28)));
    Egu ^= Du;
    Bge = ((((uint64_t)Egu) << 20) ^ (((uint64_t)Egu) >> (64 - 20)));
    Eka ^= Da;
    Bgi = ((((uint64_t)Eka) << 3) ^ (((uint64_t)Eka) >> (64 - 3)));
    Eme ^= De;
    Bgo = ((((uint64_t)Eme) << 45) ^ (((uint64_t)Eme) >> (64 - 45)));
    Esi ^= Di;
    Bgu = ((((uint64_t)Esi) << 61) ^ (((uint64_t)Esi) >> (64 - 61)));
    Aga = Bga ^ (Bge | Bgi);
    Age = Bge ^ (Bgi & Bgo);
    Agi = Bgi ^ (Bgo | (~Bgu));
    Ago = Bgo ^ (Bgu | Bga);
    Agu = Bgu ^ (Bga & Bge);
    Ebe ^= De;
    Bka = ((((uint64_t)Ebe) << 1) ^ (((uint64_t)Ebe) >> (64 - 1)));
    Egi ^= Di;
    Bke = ((((uint64_t)Egi) << 6) ^ (((uint64_t)Egi) >> (64 - 6)));
    Eko ^= Do;
    Bki = ((((uint64_t)Eko) << 25) ^ (((uint64_t)Eko) >> (64 - 25)));
    Emu ^= Du;
    Bko = ((((uint64_t)Emu) << 8) ^ (((uint64_t)Emu) >> (64 - 8)));
    Esa ^= Da;
    Bku = ((((uint64_t)Esa) << 18) ^ (((uint64_t)Esa) >> (64 - 18)));
    Aka = Bka ^ (Bke | Bki);
    Ake = Bke ^ (Bki & Bko);
    Aki = Bki ^ ((~Bko) & Bku);
    Ako = (~Bko) ^ (Bku | Bka);
    Aku = Bku ^ (Bka & Bke);
    Ebu ^= Du;
    Bma = ((((uint64_t)Ebu) << 27) ^ (((uint64_t)Ebu) >> (64 - 27)));
    Ega ^= Da;
    Bme = ((((uint64_t)Ega) << 36) ^ (((uint64_t)Ega) >> (64 - 36)));
    Eke ^= De;
    Bmi = ((((uint64_t)Eke) << 10) ^ (((uint64_t)Eke) >> (64 - 10)));
    Emi ^= Di;
    Bmo = ((((uint64_t)Emi) << 15) ^ (((uint64_t)Emi) >> (64 - 15)));
    Eso ^= Do;
    Bmu = ((((uint64_t)Eso) << 56) ^ (((uint64_t)Eso) >> (64 - 56)));
    Ama = Bma ^ (Bme & Bmi);
    Ame = Bme ^ (Bmi | Bmo);
    Ami = Bmi ^ ((~Bmo) | Bmu);
    Amo = (~Bmo) ^ (Bmu & Bma);
    Amu = Bmu ^ (Bma | Bme);
    Ebi ^= Di;
    Bsa = ((((uint64_t)Ebi) << 62) ^ (((uint64_t)Ebi) >> (64 - 62)));
    Ego ^= Do;
    Bse = ((((uint64_t)Ego) << 55) ^ (((uint64_t)Ego) >> (64 - 55)));
    Eku ^= Du;
    Bsi = ((((uint64_t)Eku) << 39) ^ (((uint64_t)Eku) >> (64 - 39)));
    Ema ^= Da;
    Bso = ((((uint64_t)Ema) << 41) ^ (((uint64_t)Ema) >> (64 - 41)));
    Ese ^= De;
    Bsu = ((((uint64_t)Ese) << 2) ^ (((uint64_t)Ese) >> (64 - 2)));
    Asa = Bsa ^ ((~Bse) & Bsi);
    Ase = (~Bse) ^ (Bsi | Bso);
    Asi = Bsi ^ (Bso & Bsu);
    Aso = Bso ^ (Bsu | Bsa);
    Asu = Bsu ^ (Bsa & Bse);
    inDataAsLanes += laneCount;
    dataByteLen -= laneCount * 8;
  }
  stateAsLanes[0] = Aba;
  stateAsLanes[1] = Abe;
  stateAsLanes[2] = Abi;
  stateAsLanes[3] = Abo;
  stateAsLanes[4] = Abu;
  stateAsLanes[5] = Aga;
  stateAsLanes[6] = Age;
  stateAsLanes[7] = Agi;
  stateAsLanes[8] = Ago;
  stateAsLanes[9] = Agu;
  stateAsLanes[10] = Aka;
  stateAsLanes[11] = Ake;
  stateAsLanes[12] = Aki;
  stateAsLanes[13] = Ako;
  stateAsLanes[14] = Aku;
  stateAsLanes[15] = Ama;
  stateAsLanes[16] = Ame;
  stateAsLanes[17] = Ami;
  stateAsLanes[18] = Amo;
  stateAsLanes[19] = Amu;
  stateAsLanes[20] = Asa;
  stateAsLanes[21] = Ase;
  stateAsLanes[22] = Asi;
  stateAsLanes[23] = Aso;
  stateAsLanes[24] = Asu;
  return originalDataByteLen - dataByteLen;
}

static int _PySHA3_KeccakWidth1600_Sponge(
    unsigned int rate, unsigned int capacity, const unsigned char *input,
    size_t inputByteLen, unsigned char suffix, unsigned char *output,
    size_t outputByteLen) {
  forcealign(8) unsigned char state[200];
  unsigned int partialBlock;
  const unsigned char *curInput = input;
  unsigned char *curOutput = output;
  unsigned int rateInBytes = rate / 8;
  if (rate + capacity != 1600) return 1;
  if ((rate <= 0) || (rate > 1600) || ((rate % 8) != 0)) return 1;
  if (suffix == 0) return 1;
  _PySHA3_KeccakP1600_Initialize(state);
  if (((rateInBytes % (1600 / 200)) == 0) && (inputByteLen >= rateInBytes)) {
    size_t j;
    j = _PySHA3_KeccakF1600_FastLoop_Absorb(state, rateInBytes / (1600 / 200),
                                            curInput, inputByteLen);
    curInput += j;
    inputByteLen -= j;
  }
  while (inputByteLen >= (size_t)rateInBytes) {
    _PySHA3_KeccakP1600_AddBytes(state, curInput, 0, rateInBytes);
    _PySHA3_KeccakP1600_Permute_24rounds(state);
    curInput += rateInBytes;
    inputByteLen -= rateInBytes;
  }
  partialBlock = (unsigned int)inputByteLen;
  _PySHA3_KeccakP1600_AddBytes(state, curInput, 0, partialBlock);
  ((unsigned char *)(state))[(partialBlock)] ^= (suffix);
  if ((suffix >= 0x80) && (partialBlock == (rateInBytes - 1)))
    _PySHA3_KeccakP1600_Permute_24rounds(state);
  ((unsigned char *)(state))[(rateInBytes - 1)] ^= (0x80);
  _PySHA3_KeccakP1600_Permute_24rounds(state);
  while (outputByteLen > (size_t)rateInBytes) {
    _PySHA3_KeccakP1600_ExtractBytes(state, curOutput, 0, rateInBytes);
    _PySHA3_KeccakP1600_Permute_24rounds(state);
    curOutput += rateInBytes;
    outputByteLen -= rateInBytes;
  }
  partialBlock = (unsigned int)outputByteLen;
  _PySHA3_KeccakP1600_ExtractBytes(state, curOutput, 0, partialBlock);
  return 0;
}

static int _PySHA3_KeccakWidth1600_SpongeAbsorb(
    KeccakWidth1600_SpongeInstance *instance, const unsigned char *data,
    size_t dataByteLen) {
  size_t i, j;
  unsigned int partialBlock;
  const unsigned char *curData;
  unsigned int rateInBytes = instance->rate / 8;
  if (instance->squeezing) return 1;
  i = 0;
  curData = data;
  while (i < dataByteLen) {
    if ((instance->byteIOIndex == 0) && (dataByteLen >= (i + rateInBytes))) {
      if ((rateInBytes % (1600 / 200)) == 0) {
        j = _PySHA3_KeccakF1600_FastLoop_Absorb(instance->state,
                                                rateInBytes / (1600 / 200),
                                                curData, dataByteLen - i);
        i += j;
        curData += j;
      } else {
        for (j = dataByteLen - i; j >= rateInBytes; j -= rateInBytes) {
          _PySHA3_KeccakP1600_AddBytes(instance->state, curData, 0,
                                       rateInBytes);
          _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
          curData += rateInBytes;
        }
        i = dataByteLen - j;
      }
    } else {
      partialBlock = (unsigned int)(dataByteLen - i);
      if (partialBlock + instance->byteIOIndex > rateInBytes)
        partialBlock = rateInBytes - instance->byteIOIndex;
      i += partialBlock;
      _PySHA3_KeccakP1600_AddBytes(instance->state, curData,
                                   instance->byteIOIndex, partialBlock);
      curData += partialBlock;
      instance->byteIOIndex += partialBlock;
      if (instance->byteIOIndex == rateInBytes) {
        _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
        instance->byteIOIndex = 0;
      }
    }
  }
  return 0;
}

static int _PySHA3_KeccakWidth1600_SpongeAbsorbLastFewBits(
    KeccakWidth1600_SpongeInstance *instance, unsigned char delimitedData) {
  unsigned int rateInBytes = instance->rate / 8;
  if (delimitedData == 0) return 1;
  if (instance->squeezing) return 1;
  ((unsigned char *)(instance->state))[(instance->byteIOIndex)] ^=
      (delimitedData);
  if ((delimitedData >= 0x80) && (instance->byteIOIndex == (rateInBytes - 1)))
    _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
  ((unsigned char *)(instance->state))[(rateInBytes - 1)] ^= (0x80);
  _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
  instance->byteIOIndex = 0;
  instance->squeezing = 1;
  return 0;
}

static int _PySHA3_KeccakWidth1600_SpongeSqueeze(
    KeccakWidth1600_SpongeInstance *instance, unsigned char *data,
    size_t dataByteLen) {
  size_t i, j;
  unsigned int partialBlock;
  unsigned int rateInBytes = instance->rate / 8;
  unsigned char *curData;
  if (!instance->squeezing)
    _PySHA3_KeccakWidth1600_SpongeAbsorbLastFewBits(instance, 0x01);
  i = 0;
  curData = data;
  while (i < dataByteLen) {
    if ((instance->byteIOIndex == rateInBytes) &&
        (dataByteLen >= (i + rateInBytes))) {
      for (j = dataByteLen - i; j >= rateInBytes; j -= rateInBytes) {
        _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
        _PySHA3_KeccakP1600_ExtractBytes(instance->state, curData, 0,
                                         rateInBytes);
        curData += rateInBytes;
      }
      i = dataByteLen - j;
    } else {
      if (instance->byteIOIndex == rateInBytes) {
        _PySHA3_KeccakP1600_Permute_24rounds(instance->state);
        instance->byteIOIndex = 0;
      }
      partialBlock = (unsigned int)(dataByteLen - i);
      if (partialBlock + instance->byteIOIndex > rateInBytes)
        partialBlock = rateInBytes - instance->byteIOIndex;
      i += partialBlock;
      _PySHA3_KeccakP1600_ExtractBytes(instance->state, curData,
                                       instance->byteIOIndex, partialBlock);
      curData += partialBlock;
      instance->byteIOIndex += partialBlock;
    }
  }
  return 0;
}

static int _PySHA3_KeccakWidth1600_SpongeInitialize(
    KeccakWidth1600_SpongeInstance *instance, unsigned int rate,
    unsigned int capacity) {
  if (rate + capacity != 1600) return 1;
  if ((rate <= 0) || (rate > 1600) || ((rate % 8) != 0)) return 1;
  _PySHA3_KeccakP1600_Initialize(instance->state);
  instance->rate = rate;
  instance->byteIOIndex = 0;
  instance->squeezing = 0;
  return 0;
}

static HashReturn _PySHA3_Keccak_HashInitialize(Keccak_HashInstance *instance,
                                                unsigned int rate,
                                                unsigned int capacity,
                                                unsigned int hashbitlen,
                                                unsigned char delimitedSuffix) {
  HashReturn result;
  if (delimitedSuffix == 0) return FAIL;
  result = (HashReturn)_PySHA3_KeccakWidth1600_SpongeInitialize(
      &instance->sponge, rate, capacity);
  if (result != SUCCESS) return result;
  instance->fixedOutputLength = hashbitlen;
  instance->delimitedSuffix = delimitedSuffix;
  return SUCCESS;
}

static HashReturn _PySHA3_Keccak_HashUpdate(Keccak_HashInstance *instance,
                                            const unsigned char *data,
                                            size_t databitlen) {
  if ((databitlen % 8) == 0)
    return (HashReturn)_PySHA3_KeccakWidth1600_SpongeAbsorb(
        &instance->sponge, data, databitlen / 8);
  else {
    HashReturn ret = (HashReturn)_PySHA3_KeccakWidth1600_SpongeAbsorb(
        &instance->sponge, data, databitlen / 8);
    if (ret == SUCCESS) {
      unsigned char lastByte = data[databitlen / 8];
      unsigned short delimitedLastBytes =
          (unsigned short)((unsigned short)lastByte |
                           ((unsigned short)instance->delimitedSuffix
                            << (databitlen % 8)));
      if ((delimitedLastBytes & 0xFF00) == 0x0000) {
        instance->delimitedSuffix = delimitedLastBytes & 0xFF;
      } else {
        unsigned char oneByte[1];
        oneByte[0] = delimitedLastBytes & 0xFF;
        ret = (HashReturn)_PySHA3_KeccakWidth1600_SpongeAbsorb(
            &instance->sponge, oneByte, 1);
        instance->delimitedSuffix = (delimitedLastBytes >> 8) & 0xFF;
      }
    }
    return ret;
  }
}

static HashReturn _PySHA3_Keccak_HashFinal(Keccak_HashInstance *instance,
                                           unsigned char *hashval) {
  HashReturn ret = (HashReturn)_PySHA3_KeccakWidth1600_SpongeAbsorbLastFewBits(
      &instance->sponge, instance->delimitedSuffix);
  if (ret == SUCCESS)
    return (HashReturn)_PySHA3_KeccakWidth1600_SpongeSqueeze(
        &instance->sponge, hashval, instance->fixedOutputLength / 8);
  else
    return ret;
}

static HashReturn _PySHA3_Keccak_HashSqueeze(Keccak_HashInstance *instance,
                                             unsigned char *data,
                                             size_t databitlen) {
  if ((databitlen % 8) != 0) return FAIL;
  return (HashReturn)_PySHA3_KeccakWidth1600_SpongeSqueeze(
      &instance->sponge, data, databitlen / 8);
}

static void _PySHA3_KeccakP1600_ExtractAndAddBytesInLane(
    const void *state, unsigned int lanePosition, const unsigned char *input,
    unsigned char *output, unsigned int offset, unsigned int length) {
  uint64_t lane = ((uint64_t *)state)[lanePosition];
  if ((lanePosition == 1) || (lanePosition == 2) || (lanePosition == 8) ||
      (lanePosition == 12) || (lanePosition == 17) || (lanePosition == 20))
    lane = ~lane;
  {
    unsigned int i;
    uint64_t lane1[1];
    lane1[0] = lane;
    for (i = 0; i < length; i++)
      output[i] = input[i] ^ ((uint8_t *)lane1)[offset + i];
  }
}

static void _PySHA3_KeccakP1600_ExtractAndAddLanes(const void *state,
                                                   const unsigned char *input,
                                                   unsigned char *output,
                                                   unsigned int laneCount) {
  unsigned int i;
  for (i = 0; i < laneCount; i++) {
    ((uint64_t *)output)[i] =
        ((uint64_t *)input)[i] ^ ((const uint64_t *)state)[i];
  }
  if (laneCount > 1) {
    ((uint64_t *)output)[1] = ~((uint64_t *)output)[1];
    if (laneCount > 2) {
      ((uint64_t *)output)[2] = ~((uint64_t *)output)[2];
      if (laneCount > 8) {
        ((uint64_t *)output)[8] = ~((uint64_t *)output)[8];
        if (laneCount > 12) {
          ((uint64_t *)output)[12] = ~((uint64_t *)output)[12];
          if (laneCount > 17) {
            ((uint64_t *)output)[17] = ~((uint64_t *)output)[17];
            if (laneCount > 20) {
              ((uint64_t *)output)[20] = ~((uint64_t *)output)[20];
            }
          }
        }
      }
    }
  }
}

static void _PySHA3_KeccakP1600_ExtractAndAddBytes(const void *state,
                                                   const unsigned char *input,
                                                   unsigned char *output,
                                                   unsigned int offset,
                                                   unsigned int length) {
  if ((offset) == 0) {
    _PySHA3_KeccakP1600_ExtractAndAddLanes(state, input, output, (length) / 8);
    _PySHA3_KeccakP1600_ExtractAndAddBytesInLane(
        state, (length) / 8, (input) + ((length) / 8) * 8,
        (output) + ((length) / 8) * 8, 0, (length) % 8);
  } else {
    unsigned int _sizeLeft = (length);
    unsigned int _lanePosition = (offset) / 8;
    unsigned int _offsetInLane = (offset) % 8;
    const unsigned char *_curInput = (input);
    unsigned char *_curOutput = (output);
    while (_sizeLeft > 0) {
      unsigned int _bytesInLane = 8 - _offsetInLane;
      if (_bytesInLane > _sizeLeft) _bytesInLane = _sizeLeft;
      _PySHA3_KeccakP1600_ExtractAndAddBytesInLane(state, _lanePosition,
                                                   _curInput, _curOutput,
                                                   _offsetInLane, _bytesInLane);
      _sizeLeft -= _bytesInLane;
      _lanePosition++;
      _offsetInLane = 0;
      _curInput += _bytesInLane;
      _curOutput += _bytesInLane;
    }
  }
}

typedef struct {
  PyObject ob_base;
  Keccak_HashInstance hash_state;
} SHA3object;

static PyTypeObject SHA3_224type;
static PyTypeObject SHA3_256type;
static PyTypeObject SHA3_384type;
static PyTypeObject SHA3_512type;
static PyTypeObject SHAKE128type;
static PyTypeObject SHAKE256type;

static char _sha3_sha3_224_copy__doc__[] = "copy($self, /)\n"
                                           "--\n"
                                           "\n"
                                           "Return a copy of the hash object.";

static PyObject *_sha3_sha3_224_copy_impl(SHA3object *);

static PyObject *_sha3_sha3_224_copy(SHA3object *self, PyObject *_unused_ignored
                                     __attribute__((unused))) {
  return _sha3_sha3_224_copy_impl(self);
}

static char _sha3_sha3_224_digest__doc__[] =
    "digest($self, /)\n"
    "--\n"
    "\n"
    "Return the digest value as a bytes object.";

static PyObject *_sha3_sha3_224_digest_impl(SHA3object *);

static PyObject *_sha3_sha3_224_digest(SHA3object *self,
                                       PyObject *_unused_ignored
                                       __attribute__((unused))) {
  return _sha3_sha3_224_digest_impl(self);
}

static char _sha3_sha3_224_hexdigest__doc__[] =
    "hexdigest($self, /)\n"
    "--\n"
    "\n"
    "Return the digest value as a string of hexadecimal digits.";

static PyObject *_sha3_sha3_224_hexdigest_impl(SHA3object *);

static PyObject *_sha3_sha3_224_hexdigest(SHA3object *self,
                                          PyObject *_unused_ignored
                                          __attribute__((unused))) {
  return _sha3_sha3_224_hexdigest_impl(self);
}

static char _sha3_sha3_224_update__doc__[] =
    "update($self, data, /)\n"
    "--\n"
    "\n"
    "Update this hash object\'s state with the provided bytes-like object.";

static char _sha3_shake_128_digest__doc__[] =
    "digest($self, length, /)\n"
    "--\n"
    "\n"
    "Return the digest value as a bytes object.";

static char _sha3_shake_128_hexdigest__doc__[] =
    "hexdigest($self, length, /)\n"
    "--\n"
    "\n"
    "Return the digest value as a string of hexadecimal digits.";

static SHA3object *newSHA3object(PyTypeObject *type) {
  SHA3object *newobj;
  newobj = PyObject_New(SHA3object, type);
  if (!newobj) return 0;
  return newobj;
}

static PyObject *py_sha3_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwargs) {
  SHA3object *self = 0;
  Py_buffer buf = {0, 0};
  HashReturn res;
  PyObject *data = 0;
  if (!_PyArg_NoKeywords(type->tp_name, kwargs)) {
    return 0;
  }
  if (!PyArg_UnpackTuple(args, type->tp_name, 0, 1, &data)) {
    return 0;
  }
  self = newSHA3object(type);
  if (self == 0) {
    goto error;
  }
  if (type == &SHA3_224type) {
    res =
        _PySHA3_Keccak_HashInitialize(&self->hash_state, 1152, 448, 224, 0x06);
  } else if (type == &SHA3_256type) {
    res =
        _PySHA3_Keccak_HashInitialize(&self->hash_state, 1088, 512, 256, 0x06);
  } else if (type == &SHA3_384type) {
    res = _PySHA3_Keccak_HashInitialize(&self->hash_state, 832, 768, 384, 0x06);
  } else if (type == &SHA3_512type) {
    res =
        _PySHA3_Keccak_HashInitialize(&self->hash_state, 576, 1024, 512, 0x06);
  } else if (type == &SHAKE128type) {
    res = _PySHA3_Keccak_HashInitialize(&self->hash_state, 1344, 256, 0, 0x1F);
  } else if (type == &SHAKE256type) {
    res = _PySHA3_Keccak_HashInitialize(&self->hash_state, 1088, 512, 0, 0x1F);
  } else {
    PyErr_BadInternalCall();
    goto error;
  }
  if (data) {
    do {
      if (((((((PyObject *)((data)))->ob_type))->tp_flags & ((1UL << 28))) !=
           0)) {
        PyErr_SetString(PyExc_TypeError,
                        "Unicode-objects must be encoded before hashing");
        goto error;
      }
      if (!((((data))->ob_type->tp_as_buffer != 0) &&
            (((data))->ob_type->tp_as_buffer->bf_getbuffer != 0))) {
        PyErr_SetString(PyExc_TypeError,
                        "object supporting the buffer API required");
        goto error;
      }
      if (PyObject_GetBuffer((data), (&buf), 0) == -1) {
        goto error;
      }
      if ((&buf)->ndim > 1) {
        PyErr_SetString(PyExc_BufferError, "Buffer must be single dimension");
        PyBuffer_Release((&buf));
        goto error;
      }
    } while (0);
    res = _PySHA3_Keccak_HashUpdate(&self->hash_state, buf.buf, buf.len * 8);
    if (res != SUCCESS) {
      PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Update()");
      goto error;
    }
    PyBuffer_Release(&buf);
  }
  return (PyObject *)self;
error:
  if (self) {
    do {
      PyObject *_py_decref_tmp = (PyObject *)(self);
      if (--(_py_decref_tmp)->ob_refcnt != 0)
        ;
      else
        ((*(((PyObject *)(_py_decref_tmp))->ob_type)->tp_dealloc)(
            (PyObject *)(_py_decref_tmp)));
    } while (0);
  }
  if (data && buf.obj) {
    PyBuffer_Release(&buf);
  }
  return 0;
}

static void SHA3_dealloc(SHA3object *self) {
  PyObject_Free(self);
}

static PyObject *_sha3_sha3_224_copy_impl(SHA3object *self) {
  SHA3object *newobj;
  if ((newobj = newSHA3object((((PyObject *)(self))->ob_type))) == 0) {
    return 0;
  }
  memcpy(&(newobj->hash_state), &(self->hash_state),
         sizeof(Keccak_HashInstance));
  return (PyObject *)newobj;
}

static PyObject *_sha3_sha3_224_digest_impl(SHA3object *self) {
  unsigned char digest[64 + (20 * 8)];
  Keccak_HashInstance temp;
  HashReturn res;
  memcpy(&(temp), &(self->hash_state), sizeof(Keccak_HashInstance));
  res = _PySHA3_Keccak_HashFinal(&temp, digest);
  if (res != SUCCESS) {
    PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Final()");
    return 0;
  }
  return PyBytes_FromStringAndSize((const char *)digest,
                                   self->hash_state.fixedOutputLength / 8);
}

static PyObject *_sha3_sha3_224_hexdigest_impl(SHA3object *self) {
  unsigned char digest[64 + (20 * 8)];
  Keccak_HashInstance temp;
  HashReturn res;
  memcpy(&(temp), &(self->hash_state), sizeof(Keccak_HashInstance));
  res = _PySHA3_Keccak_HashFinal(&temp, digest);
  if (res != SUCCESS) {
    PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Final()");
    return 0;
  }
  return _Py_strhex((const char *)digest,
                    self->hash_state.fixedOutputLength / 8);
}

static PyObject *_sha3_sha3_224_update(SHA3object *self, PyObject *data) {
  Py_buffer buf;
  HashReturn res;
  do {
    if (((((((PyObject *)((data)))->ob_type))->tp_flags & ((1UL << 28))) !=
         0)) {
      PyErr_SetString(PyExc_TypeError,
                      "Unicode-objects must be encoded before hashing");
      return 0;
    }
    if (!((((data))->ob_type->tp_as_buffer != 0) &&
          (((data))->ob_type->tp_as_buffer->bf_getbuffer != 0))) {
      PyErr_SetString(PyExc_TypeError,
                      "object supporting the buffer API required");
      return 0;
    }
    if (PyObject_GetBuffer((data), (&buf), 0) == -1) {
      return 0;
    }
    if ((&buf)->ndim > 1) {
      PyErr_SetString(PyExc_BufferError, "Buffer must be single dimension");
      PyBuffer_Release((&buf));
      return 0;
    }
  } while (0);
  res = _PySHA3_Keccak_HashUpdate(&self->hash_state, buf.buf, buf.len * 8);
  if (res != SUCCESS) {
    PyBuffer_Release(&buf);
    PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Update()");
    return 0;
  }
  PyBuffer_Release(&buf);
  (((PyObject *)((&_Py_NoneStruct)))->ob_refcnt++);
  return (&_Py_NoneStruct);
}

static PyMethodDef SHA3_methods[] = {
    {"copy", (PyCFunction)_sha3_sha3_224_copy, 0x0004,
     _sha3_sha3_224_copy__doc__},
    {"digest", (PyCFunction)_sha3_sha3_224_digest, 0x0004,
     _sha3_sha3_224_digest__doc__},
    {"hexdigest", (PyCFunction)_sha3_sha3_224_hexdigest, 0x0004,
     _sha3_sha3_224_hexdigest__doc__},
    {"update", (PyCFunction)_sha3_sha3_224_update, 0x0008,
     _sha3_sha3_224_update__doc__},
    {0},
};

static PyObject *SHA3_get_block_size(SHA3object *self, void *closure) {
  int rate = self->hash_state.sponge.rate;
  return PyLong_FromLong(rate / 8);
}

static PyObject *SHA3_get_name(SHA3object *self, void *closure) {
  PyTypeObject *type = (((PyObject *)(self))->ob_type);
  if (type == &SHA3_224type) {
    return PyUnicode_FromString("sha3_224");
  } else if (type == &SHA3_256type) {
    return PyUnicode_FromString("sha3_256");
  } else if (type == &SHA3_384type) {
    return PyUnicode_FromString("sha3_384");
  } else if (type == &SHA3_512type) {
    return PyUnicode_FromString("sha3_512");
  } else if (type == &SHAKE128type) {
    return PyUnicode_FromString("shake_128");
  } else if (type == &SHAKE256type) {
    return PyUnicode_FromString("shake_256");
  } else {
    PyErr_BadInternalCall();
    return 0;
  }
}

static PyObject *SHA3_get_digest_size(SHA3object *self, void *closure) {
  return PyLong_FromLong(self->hash_state.fixedOutputLength / 8);
}

static PyObject *SHA3_get_capacity_bits(SHA3object *self, void *closure) {
  int capacity = 1600 - self->hash_state.sponge.rate;
  return PyLong_FromLong(capacity);
}

static PyObject *SHA3_get_rate_bits(SHA3object *self, void *closure) {
  unsigned int rate = self->hash_state.sponge.rate;
  return PyLong_FromLong(rate);
}

static PyObject *SHA3_get_suffix(SHA3object *self, void *closure) {
  unsigned char suffix[2];
  suffix[0] = self->hash_state.delimitedSuffix;
  suffix[1] = 0;
  return PyBytes_FromStringAndSize((const char *)suffix, 1);
}

static PyGetSetDef SHA3_getseters[] = {
    {"block_size", (getter)SHA3_get_block_size},
    {"name", (getter)SHA3_get_name},
    {"digest_size", (getter)SHA3_get_digest_size},
    {"_capacity_bits", (getter)SHA3_get_capacity_bits},
    {"_rate_bits", (getter)SHA3_get_rate_bits},
    {"_suffix", (getter)SHA3_get_suffix},
    {0},
};

static char sha3_224__doc__[] =
    "sha3_224([data]) -> SHA3 object\n\nReturn a new SHA3 hash object with a "
    "hashbit length of 28 bytes.";

static char sha3_256__doc__[] =
    "sha3_256([data]) -> SHA3 object\n\nReturn a new SHA3 hash object with a "
    "hashbit length of 32 bytes.";

static char sha3_384__doc__[] =
    "sha3_384([data]) -> SHA3 object\n\nReturn a new SHA3 hash object with a "
    "hashbit length of 48 bytes.";

static char sha3_512__doc__[] =
    "sha3_512([data]) -> SHA3 object\n\nReturn a new SHA3 hash object with a "
    "hashbit length of 64 bytes.";

static PyTypeObject SHA3_224type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.sha3_224",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    sha3_224__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHA3_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};

static PyTypeObject SHA3_256type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.sha3_256",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    sha3_256__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHA3_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};

static PyTypeObject SHA3_384type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.sha3_384",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    sha3_384__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHA3_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};

static PyTypeObject SHA3_512type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.sha3_512",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    sha3_512__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHA3_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};

static PyObject *_SHAKE_digest(SHA3object *self, PyObject *digestlen_obj,
                               int hex) {
  unsigned long digestlen;
  unsigned char *digest = 0;
  Keccak_HashInstance temp;
  int res;
  PyObject *result = 0;
  digestlen = PyLong_AsUnsignedLong(digestlen_obj);
  if (digestlen == (unsigned long)-1 && PyErr_Occurred()) {
    return 0;
  }
  if (digestlen >= (1 << 29)) {
    PyErr_SetString(PyExc_ValueError, "length is too large");
    return 0;
  }
  digest = (unsigned char *)PyMem_Malloc(digestlen + (20 * 8));
  if (digest == 0) {
    return PyErr_NoMemory();
  }
  memcpy(&(temp), &(self->hash_state), sizeof(Keccak_HashInstance));
  res = _PySHA3_Keccak_HashFinal(&temp, 0);
  if (res != SUCCESS) {
    PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 done()");
    goto error;
  }
  res = _PySHA3_Keccak_HashSqueeze(&temp, digest, digestlen * 8);
  if (res != SUCCESS) {
    PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Squeeze()");
    return 0;
  }
  if (hex) {
    result = _Py_strhex((const char *)digest, digestlen);
  } else {
    result = PyBytes_FromStringAndSize((const char *)digest, digestlen);
  }
error:
  if (digest != 0) {
    PyMem_Free(digest);
  }
  return result;
}

static PyObject *_sha3_shake_128_digest(SHA3object *self, PyObject *length) {
  return _SHAKE_digest(self, length, 0);
}

static PyObject *_sha3_shake_128_hexdigest(SHA3object *self, PyObject *length) {
  return _SHAKE_digest(self, length, 1);
}

static PyMethodDef SHAKE_methods[] = {
    {"copy", (PyCFunction)_sha3_sha3_224_copy, 0x0004,
     _sha3_sha3_224_copy__doc__},
    {"digest", (PyCFunction)_sha3_shake_128_digest, 0x0008,
     _sha3_shake_128_digest__doc__},
    {"hexdigest", (PyCFunction)_sha3_shake_128_hexdigest, 0x0008,
     _sha3_shake_128_hexdigest__doc__},
    {"update", (PyCFunction)_sha3_sha3_224_update, 0x0008,
     _sha3_sha3_224_update__doc__},
    {0, 0}};

static char shake_128__doc__[] =
    "shake_128([data]) -> SHAKE object\n\nReturn a new SHAKE hash object.";

static char shake_256__doc__[] =
    "shake_256([data]) -> SHAKE object\n\nReturn a new SHAKE hash object.";

static PyTypeObject SHAKE128type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.shake_128",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    shake_128__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHAKE_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};
static PyTypeObject SHAKE256type = {
    PyVarObject_HEAD_INIT(NULL, 0) "_sha3.shake_256",
    sizeof(SHA3object),
    0,
    (destructor)SHA3_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (0 | (1UL << 18) | 0),
    shake_256__doc__,
    0,
    0,
    0,
    0,
    0,
    0,
    SHAKE_methods,
    0,
    SHA3_getseters,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    py_sha3_new,
};

static struct PyModuleDef _SHA3module = {PyModuleDef_HEAD_INIT, "_sha3", 0, -1};

PyObject *PyInit__sha3(void) {
  PyObject *m = 0;
  if ((m = PyModule_Create2(&_SHA3module, 1013)) == 0) {
    return 0;
  }
  do {
    (((PyObject *)(&SHA3_224type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHA3_224type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHA3_224type))->ob_refcnt++);
    if (PyModule_AddObject(m, "sha3_224", (PyObject *)&SHA3_224type) < 0) {
      goto error;
    }
  } while (0);
  do {
    (((PyObject *)(&SHA3_256type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHA3_256type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHA3_256type))->ob_refcnt++);
    if (PyModule_AddObject(m, "sha3_256", (PyObject *)&SHA3_256type) < 0) {
      goto error;
    }
  } while (0);
  do {
    (((PyObject *)(&SHA3_384type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHA3_384type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHA3_384type))->ob_refcnt++);
    if (PyModule_AddObject(m, "sha3_384", (PyObject *)&SHA3_384type) < 0) {
      goto error;
    }
  } while (0);
  do {
    (((PyObject *)(&SHA3_512type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHA3_512type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHA3_512type))->ob_refcnt++);
    if (PyModule_AddObject(m, "sha3_512", (PyObject *)&SHA3_512type) < 0) {
      goto error;
    }
  } while (0);
  do {
    (((PyObject *)(&SHAKE128type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHAKE128type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHAKE128type))->ob_refcnt++);
    if (PyModule_AddObject(m, "shake_128", (PyObject *)&SHAKE128type) < 0) {
      goto error;
    }
  } while (0);
  do {
    (((PyObject *)(&SHAKE256type))->ob_type) = &PyType_Type;
    if (PyType_Ready(&SHAKE256type) < 0) {
      goto error;
    }
    (((PyObject *)((PyObject *)&SHAKE256type))->ob_refcnt++);
    if (PyModule_AddObject(m, "shake_256", (PyObject *)&SHAKE256type) < 0) {
      goto error;
    }
  } while (0);
  if (PyModule_AddIntConstant(m, "keccakopt", 64) < 0) {
    goto error;
  }
  if (PyModule_AddStringConstant(m, "implementation",
                                 "generic 64-bit optimized implementation ("
                                 "lane complementing, all rounds unrolled"
                                 ")") < 0) {
    goto error;
  }
  return m;
error:
  do {
    PyObject *_py_decref_tmp = (PyObject *)(m);
    if (--(_py_decref_tmp)->ob_refcnt != 0)
      ;
    else
      ((*(((PyObject *)(_py_decref_tmp))->ob_type)->tp_dealloc)(
          (PyObject *)(_py_decref_tmp)));
  } while (0);
  return 0;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
    const struct _inittab _PyImport_Inittab__sha3 = {
        "_sha3",
        PyInit__sha3,
};
