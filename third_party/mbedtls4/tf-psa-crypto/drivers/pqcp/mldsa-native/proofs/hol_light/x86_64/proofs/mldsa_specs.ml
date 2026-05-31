(*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT-0
 *)

(* ========================================================================= *)
(* Common specifications and tactics for ML-DSA, mainly related to the NTT.  *)
(* ========================================================================= *)

needs "Library/words.ml";;
needs "Library/isum.ml";;

(* ------------------------------------------------------------------------- *)
(* The pure forms of forward and inverse NTT with no reordering.             *)
(* ------------------------------------------------------------------------- *)

let pure_forward_ntt_mldsa = define
 `pure_forward_ntt_mldsa f k =
    isum (0..127) (\j. f(2 * j + k MOD 2) *
                       &1753 pow ((2 * k DIV 2 + 1) * j))
    rem &8380417`;;

(* ------------------------------------------------------------------------- *)
(* Bit-reversing order as used in the standard/default order.                *)
(* ------------------------------------------------------------------------- *)

let bitreverse8 = define
 `bitreverse8(n) = val(word_reversefields 1 (word n:8 word))`;;

(* ------------------------------------------------------------------------- *)
(* AVX2-optimized ordering for ML-DSA NTT (swaps bit fields then reverses)   *)
(* ------------------------------------------------------------------------- *)

let mldsa_avx2_ntt_order = define
 `mldsa_avx2_ntt_order i =
    bitreverse8(64 * (i DIV 64) + ((i MOD 64) DIV 8) + 8 * (i MOD 8))`;;

(* ------------------------------------------------------------------------- *)
(* Conversion of each element of an array to Montgomery form with B = 2^16.  *)
(* ------------------------------------------------------------------------- *)

let tomont_8380417 = define
 `tomont_8380417 (a:num->int) = \i. (&2 pow 32 * a i) rem &8380417`;;

(* ------------------------------------------------------------------------- *)
(* The precise specs of the actual x86 code.                                 *)
(* ------------------------------------------------------------------------- *)

let mldsa_forward_ntt = define
 `mldsa_forward_ntt f k =
    isum (0..255) (\j. f j * &1753 pow ((2 * mldsa_avx2_ntt_order k + 1) * j))
    rem &8380417`;;

(* ------------------------------------------------------------------------- *)
(* Show how these relate to the "pure" ones.                                 *)
(* ------------------------------------------------------------------------- *)

let MLDSA_FORWARD_NTT = prove
 (`mldsa_forward_ntt f k =
   isum (0..255) (\j. f j * &1753 pow ((2 * mldsa_avx2_ntt_order k + 1) * j)) rem &8380417`,
  REWRITE_TAC[mldsa_forward_ntt]);;

(* ------------------------------------------------------------------------- *)
(* Explicit computation rules to evaluate mod-8380417 powers less naively.   *)
(* ------------------------------------------------------------------------- *)

let BITREVERSE8_CLAUSES = end_itlist CONJ (map
 (GEN_REWRITE_CONV I [bitreverse8] THENC DEPTH_CONV WORD_NUM_RED_CONV)
 (map (curry mk_comb `bitreverse8` o mk_small_numeral) (0--255)));;

let MLDSA_AVX2_NTT_ORDER_CLAUSES = end_itlist CONJ (map
 (GEN_REWRITE_CONV I [mldsa_avx2_ntt_order] THENC DEPTH_CONV WORD_NUM_RED_CONV THENC
  GEN_REWRITE_CONV I [BITREVERSE8_CLAUSES])
 (map (curry mk_comb `mldsa_avx2_ntt_order` o mk_small_numeral) (0--255)));;

let MLDSA_FORWARD_NTT_ALT = prove
 (`mldsa_forward_ntt f k =
   isum (0..255)
        (\j. f j *
             (&1753 pow ((2 * mldsa_avx2_ntt_order k + 1) * j)) rem &8380417)
    rem &8380417`,
  REWRITE_TAC[mldsa_forward_ntt] THEN MATCH_MP_TAC
   (REWRITE_RULE[] (ISPEC
      `(\x y. x rem &8380417 = y rem &8380417)` ISUM_RELATED)) THEN
  REWRITE_TAC[INT_REM_EQ; FINITE_NUMSEG; INT_CONG_ADD] THEN
  X_GEN_TAC `i:num` THEN DISCH_TAC THEN
  REWRITE_TAC[GSYM INT_OF_NUM_REM; GSYM INT_OF_NUM_CLAUSES;
              GSYM INT_REM_EQ] THEN
  CONV_TAC INT_REM_DOWN_CONV THEN
  AP_THM_TAC THEN AP_TERM_TAC THEN CONV_TAC INT_ARITH);;

let MLDSA_FORWARD_NTT_CONV =
  GEN_REWRITE_CONV I [MLDSA_FORWARD_NTT_ALT] THENC
  LAND_CONV EXPAND_ISUM_CONV THENC
  DEPTH_CONV NUM_RED_CONV THENC
  GEN_REWRITE_CONV ONCE_DEPTH_CONV [MLDSA_AVX2_NTT_ORDER_CLAUSES] THENC
  DEPTH_CONV NUM_RED_CONV THENC
  GEN_REWRITE_CONV DEPTH_CONV [INT_OF_NUM_POW; INT_OF_NUM_REM] THENC
  ONCE_DEPTH_CONV EXP_MOD_CONV THENC INT_REDUCE_CONV;;

(* ------------------------------------------------------------------------- *)
(* Abbreviate the Barrett reduction and multiplication and Montgomery        *)
(* reduction patterns in the code.                                           *)
(* ------------------------------------------------------------------------- *)

let mldsa_barred = define
 `(mldsa_barred:int32->int32) x =
  word_sub x
   (word_mul
    (word_ishr (word_add x (word 4194304)) 23)
    (word 8380417))`;;

let mldsa_montred = define
   `(mldsa_montred:int64->int32) x =
    word_subword
     (word_add
       (word_mul ((word_sx:int32->int64)
                    (word_mul (word_subword x (0,32)) (word 4236238847)))
                 (word 8380417))
       x)
     (32,32)`;;

let mldsa_montmul = define
 `mldsa_montmul (a:int64,b:int64) (x:int32) =
  word_sub
  (word_subword (word_mul (word_sx (x:int32)) a:int64) (32,32):int32)
  (word_subword
    (word_mul (word_sx
      (word_subword (word_mul (word_sx (x:int32)) b:int64) (0,32):int32))
      (word 8380417:int64))
    (32,32))`;;

let WORD_ADD_MLDSA_MONTMUL = prove
 (`word_add y (mldsa_montmul (a,b) x) =
   word_sub (word_add
    (word_subword (word_mul (word_sx (x:int32)) a:int64) (32,32):int32)
    y)
  (word_subword
    (word_mul (word_sx
      (word_subword (word_mul (word_sx (x:int32)) b:int64) (0,32):int32))
      (word 8380417:int64))
    (32,32))`,
  REWRITE_TAC[mldsa_montmul] THEN CONV_TAC WORD_RULE);;

let WORD_SUB_MLDSA_MONTMUL = prove
 (`word_sub y (mldsa_montmul (a,b) x) =
  word_add (word_sub y
        (word_subword (word_mul (word_sx (x:int32)) a:int64) (32,32):int32))
    (word_subword
    (word_mul (word_sx
      (word_subword (word_mul (word_sx (x:int32)) b:int64) (0,32):int32))
      (word 8380417:int64))
    (32,32))`,
  REWRITE_TAC[mldsa_montmul] THEN CONV_TAC WORD_RULE);;

let WORD_ADD_MLDSA_MONTMUL_ALT = prove
 (`word_add y (mldsa_montmul (a,b) x) =
   word_sub (word_add y
    (word_subword (word_mul (word_sx (x:int32)) a:int64) (32,32):int32))
  (word_subword
    (word_mul (word_sx
      (word_subword (word_mul (word_sx (x:int32)) b:int64) (0,32):int32))
      (word 8380417:int64))
    (32,32))`,
  REWRITE_TAC[mldsa_montmul] THEN CONV_TAC WORD_RULE);;

(* ------------------------------------------------------------------------- *)
(* From |- (x == y) (mod m) /\ P   to   |- (x == y) (mod n) /\ P             *)
(* ------------------------------------------------------------------------- *)

let WEAKEN_INTCONG_RULE =
  let prule = (MATCH_MP o prove)
   (`(x:int == y) (mod m) ==> !n. m rem n = &0 ==> (x == y) (mod n)`,
    REWRITE_TAC[INT_REM_EQ_0] THEN INTEGER_TAC)
  and conv = GEN_REWRITE_CONV I [INT_REM_ZERO; INT_REM_REFL] ORELSEC
             INT_REM_CONV
  and zth = REFL `&0:int` in
  let lrule n th =
    let th1 = SPEC (mk_intconst n) (prule th) in
    let th2 = LAND_CONV conv (lhand(concl th1)) in
    MP th1 (EQ_MP (SYM th2) zth) in
  fun n th ->
    let th1,th2 = CONJ_PAIR th in
    CONJ (lrule n th1) th2;;

(* ------------------------------------------------------------------------- *)
(* Unify modulus and conjoin a pair of (x == y) (mod m) /\ P thoerems.       *)
(* ------------------------------------------------------------------------- *)

let UNIFY_INTCONG_RULE th1 th2 =
  let p1 = dest_intconst (rand(rand(lhand(concl th1))))
  and p2 = dest_intconst (rand(rand(lhand(concl th2)))) in
  let d = gcd_num p1 p2 in
  CONJ (WEAKEN_INTCONG_RULE d th1) (WEAKEN_INTCONG_RULE d th2);;

(* ------------------------------------------------------------------------- *)
(* Process list of inequality into standard congbounds for atomic terms.     *)
(* ------------------------------------------------------------------------- *)

let DIMINDEX_INT_REDUCE_CONV =
  DEPTH_CONV(WORD_NUM_RED_CONV ORELSEC DIMINDEX_CONV) THENC
  INT_REDUCE_CONV;;

let PROCESS_BOUND_ASSUMPTIONS =
  let cth = prove
   (`(ival x <= b <=>
      --(&2 pow (dimindex(:N) - 1)) <= ival x /\ ival x <= b) /\
     (b <= ival x <=>
      b <= ival x /\ ival x <= &2 pow (dimindex(:N) - 1) - &1) /\
     (ival(x:N word) > b <=>
      b + &1 <= ival x /\ ival x <= &2 pow (dimindex(:N) - 1) - &1) /\
     (b > ival(x:N word) <=>
      --(&2 pow (dimindex(:N) - 1)) <= ival x /\ ival x <= b - &1) /\
     (ival(x:N word) >= b <=>
      b <= ival x /\ ival x <= &2 pow (dimindex(:N) - 1) - &1) /\
     (b >= ival(x:N word) <=>
      --(&2 pow (dimindex(:N) - 1)) <= ival x /\ ival x <= b) /\
     (ival(x:N word) < b <=>
      --(&2 pow (dimindex(:N) - 1)) <= ival x /\ ival x <= b - &1) /\
     (b < ival(x:N word) <=>
     b + &1 <= ival x /\ ival x <= &2 pow (dimindex(:N) - 1) - &1) /\
     (abs(ival(x:N word)) <= b <=>
      --b <= ival x /\ ival x <= b) /\
     (abs(ival(x:N word)) < b <=>
      &1 - b <= ival x /\ ival x <= b - &1)`,
    REWRITE_TAC[IVAL_BOUND; INT_ARITH `x:int <= y - &1 <=> x < y`] THEN
    INT_ARITH_TAC)
  and pth = prove
   (`!l u (x:N word).
          l <= ival x /\ ival x <= u
          ==> (ival x == ival x) (mod &0) /\ l <= ival x /\ ival x <= u`,
    REPEAT STRIP_TAC THEN ASM_REWRITE_TAC[] THEN INTEGER_TAC) in
  let rule =
    MATCH_MP pth o
    CONV_RULE (BINOP2_CONV (LAND_CONV DIMINDEX_INT_REDUCE_CONV)
                           (RAND_CONV DIMINDEX_INT_REDUCE_CONV)) o
    GEN_REWRITE_RULE I [cth] in
  let rec process lfn ths =
    match ths with
      [] -> lfn
    | th::oths ->
          let lfn' =
            try let th' = rule th in
                let tm = rand(concl th') in
                if is_intconst (rand(rand tm)) && is_intconst (lhand(lhand tm))
                then (rand(lhand(rand tm)) |-> th') lfn
                else lfn
            with Failure _ -> lfn in
          process lfn' oths in
  process undefined;;

(* ------------------------------------------------------------------------- *)
(* Congruence-and-bound propagation, just recursion on the expression tree.  *)
(* ------------------------------------------------------------------------- *)

let CONGBOUND_CONST = prove
 (`!(x:N word) n.
        ival x = n
        ==> (ival x == n) (mod &0) /\ n <= ival x /\ ival x <= n`,
  REPEAT STRIP_TAC THEN ASM_REWRITE_TAC[INT_LE_REFL] THEN INTEGER_TAC);;

let CONGBOUND_ATOM = prove
 (`!x:N word. (ival x == ival x) (mod &0) /\
              --(&2 pow (dimindex(:N) - 1)) <= ival x /\
              ival x <= &2 pow (dimindex(:N) - 1) - &1`,
  GEN_TAC THEN REWRITE_TAC[INT_ARITH `x:int <= y - &1 <=> x < y`] THEN
  REWRITE_TAC[IVAL_BOUND] THEN INTEGER_TAC);;

let CONGBOUND_ATOM_GEN = prove
 (`!x:N word. abs(ival x) <= n
              ==> (ival x == ival x) (mod &0) /\
                  --n <= ival x /\ ival x <= n`,
  REWRITE_TAC[INTEGER_RULE `(x:int == x) (mod n)`] THEN INT_ARITH_TAC);;

let CONGBOUND_IWORD = prove
 (`!x. ((x == x') (mod p) /\ l <= x /\ x <= u)
       ==> --(&2 pow (dimindex(:N) - 1)) <= l /\
           u <= &2 pow (dimindex(:N) - 1) - &1
           ==> (ival(iword x:N word) == x') (mod p) /\
               l <= ival(iword x:N word) /\ ival(iword x:N word) <= u`,
  GEN_TAC THEN STRIP_TAC THEN STRIP_TAC THEN REWRITE_TAC[word_sx] THEN
  W(MP_TAC o PART_MATCH (lhand o rand) IVAL_IWORD o
    lhand o rand o rand o snd) THEN
  ANTS_TAC THENL [ASM_INT_ARITH_TAC; DISCH_THEN SUBST1_TAC] THEN
  ASM_REWRITE_TAC[]);;

let CONGBOUND_WORD_SX = prove
 (`!x:M word.
        ((ival x == x') (mod p) /\ l <= ival x /\ ival x <= u)
        ==> --(&2 pow (dimindex(:N) - 1)) <= l /\
            u <= &2 pow (dimindex(:N) - 1) - &1
            ==> (ival(word_sx x:N word) == x') (mod p) /\
                l <= ival(word_sx x:N word) /\ ival(word_sx x:N word) <= u`,
  REWRITE_TAC[word_sx; CONGBOUND_IWORD]);;

let CONGBOUND_WORD_NEG = prove
 (`!x:N word.
        ((ival x == x') (mod p) /\ lx <= ival x /\ ival x <= ux)
        ==> --lx <= &2 pow (dimindex(:N) - 1) - &1
            ==> (ival(word_neg x) == --x') (mod p) /\
                --ux <= ival(word_neg x) /\
                ival(word_neg x) <= --lx`,
  GEN_TAC THEN STRIP_TAC THEN STRIP_TAC THEN
  SUBGOAL_THEN `ival(word_neg x:N word) = --(ival x)` SUBST1_TAC THENL
   [REPEAT(POP_ASSUM MP_TAC) THEN WORD_ARITH_TAC;
    ASM_SIMP_TAC[INTEGER_RULE
     `(x:int == x') (mod p) ==> (--x == --x') (mod p)`] THEN
    ASM_ARITH_TAC]);;

let CONGBOUND_WORD_ADD = prove
 (`!x y:N word.
        ((ival x == x') (mod p) /\ lx <= ival x /\ ival x <= ux) /\
        ((ival y == y') (mod p) /\ ly <= ival y /\ ival y <= uy)
        ==> --(&2 pow (dimindex(:N) - 1)) <= lx + ly /\
            ux + uy <= &2 pow (dimindex(:N) - 1) - &1
            ==> (ival(word_add x y) == x' + y') (mod p) /\
                lx + ly <= ival(word_add x y) /\
                ival(word_add x y) <= ux + uy`,
  REPEAT GEN_TAC THEN REWRITE_TAC[WORD_ADD_IMODULAR; imodular] THEN
  STRIP_TAC THEN STRIP_TAC THEN
  MATCH_MP_TAC(REWRITE_RULE[IMP_IMP] CONGBOUND_IWORD) THEN
  ASM_SIMP_TAC[INT_CONG_ADD] THEN ASM_INT_ARITH_TAC);;

let CONGBOUND_WORD_SUB = prove
 (`!x y:N word.
        ((ival x == x') (mod p) /\ lx <= ival x /\ ival x <= ux) /\
        ((ival y == y') (mod p) /\ ly <= ival y /\ ival y <= uy)
        ==> --(&2 pow (dimindex(:N) - 1)) <= lx - uy /\
            ux - ly <= &2 pow (dimindex(:N) - 1) - &1
            ==> (ival(word_sub x y) == x' - y') (mod p) /\
                lx - uy <= ival(word_sub x y) /\
                ival(word_sub x y) <= ux - ly`,
  REPEAT GEN_TAC THEN REWRITE_TAC[WORD_SUB_IMODULAR; imodular] THEN
  STRIP_TAC THEN STRIP_TAC THEN
  MATCH_MP_TAC(REWRITE_RULE[IMP_IMP] CONGBOUND_IWORD) THEN
  ASM_SIMP_TAC[INT_CONG_SUB] THEN ASM_INT_ARITH_TAC);;

let CONGBOUND_WORD_MUL = prove
 (`!x y:N word.
        ((ival x == x') (mod p) /\ lx <= ival x /\ ival x <= ux) /\
        ((ival y == y') (mod p) /\ ly <= ival y /\ ival y <= uy)
        ==> --(&2 pow (dimindex(:N) - 1))
            <= min (lx * ly) (min (lx * uy) (min (ux * ly) (ux * uy))) /\
            max (lx * ly) (max (lx * uy) (max (ux * ly) (ux * uy)))
            <= &2 pow (dimindex(:N) - 1) - &1
            ==> (ival(word_mul x y) == x' * y') (mod p) /\
                min (lx * ly) (min (lx * uy) (min (ux * ly) (ux * uy)))
                <= ival(word_mul x y) /\
                ival(word_mul x y)
                <= max (lx * ly) (max (lx * uy) (max (ux * ly) (ux * uy)))`,
  let lemma = prove
     (`l:int <= x /\ x <= u
       ==> !a. a * l <= a * x /\ a * x <= a * u \/
               a * u <= a * x /\ a * x <= a * l`,
      MESON_TAC[INT_LE_NEGTOTAL; INT_LE_LMUL;
                INT_ARITH `a * x:int <= a * y <=> --a * y <= --a * x`]) in
  REPEAT GEN_TAC THEN
  DISCH_THEN(CONJUNCTS_THEN(CONJUNCTS_THEN2 ASSUME_TAC MP_TAC)) THEN
  DISCH_THEN(ASSUME_TAC o SPEC `ival(x:N word)` o MATCH_MP lemma) THEN
  DISCH_THEN(MP_TAC o MATCH_MP lemma) THEN DISCH_THEN(fun th ->
        ASSUME_TAC(SPEC `ly:int` th) THEN ASSUME_TAC(SPEC `uy:int` th)) THEN
  REWRITE_TAC[WORD_MUL_IMODULAR; imodular] THEN STRIP_TAC THEN
  MATCH_MP_TAC(REWRITE_RULE[IMP_IMP] CONGBOUND_IWORD) THEN
  ASM_SIMP_TAC[INT_CONG_MUL] THEN ASM_INT_ARITH_TAC);;

let MONTRED_MLDSA_LEMMA = prove
 (`!x. &2 pow 32 * ival(mldsa_montred x) =
       ival(word_add
         (word_mul (word_sx(iword(ival x * &4236238847):int32)) (word 8380417)) x)`,
  GEN_TAC THEN REWRITE_TAC[mldsa_montred] THEN REWRITE_TAC[WORD_BLAST
   `word_subword (x:int64) (0,32):int32 = word_sx x`] THEN
  REWRITE_TAC[IWORD_INT_MUL; GSYM word_sx; GSYM WORD_IWORD] THEN
  REWRITE_TAC[WORD_BLAST `(word_sx:int64->int32) x = word_zx x`] THEN
  CONV_TAC INT_REDUCE_CONV THEN MATCH_MP_TAC(BITBLAST_RULE
   `word_and x (word 4294967295):int64 = word 0
    ==> &4294967296 * ival(word_subword x (32,32):int32) = ival x`) THEN
  REWRITE_TAC[BITBLAST_RULE
   `word_and x (word 4294967295):int64 = word 0 <=> word_zx x:int32 = word 0`] THEN
  W(MP_TAC o PART_MATCH (lhand o rand) WORD_ZX_ADD o lhand o snd) THEN
  REWRITE_TAC[DIMINDEX_32; DIMINDEX_64; ARITH] THEN DISCH_THEN SUBST1_TAC THEN
  W(MP_TAC o PART_MATCH (lhand o rand) WORD_ZX_MUL o lhand o lhand o snd) THEN
  REWRITE_TAC[DIMINDEX_32; DIMINDEX_64; ARITH] THEN DISCH_THEN SUBST1_TAC THEN
  REWRITE_TAC[WORD_BLAST `word_zx(word_sx (x:int32):int64) = x`] THEN
  REWRITE_TAC[GSYM VAL_EQ_0; VAL_WORD_ADD; VAL_WORD_MUL; VAL_WORD] THEN
  CONV_TAC MOD_DOWN_CONV THEN REWRITE_TAC[GSYM DIVIDES_MOD; DIMINDEX_32] THEN
  CONV_TAC WORD_REDUCE_CONV THEN MATCH_MP_TAC(NUMBER_RULE
   `(a * b + 1 == 0) (mod d) ==> d divides ((x * a) * b + x)`) THEN
  REWRITE_TAC[CONG] THEN ARITH_TAC);;

let CONGBOUND_MLDSA_MONTRED = prove
 (`!a a' l u.
      (ival a == a') (mod &8380417) /\ l <= ival a /\ ival a <= u
      ==> --(&9205375228383854592) <= l /\ u <= &9205375228392235008
          ==> (ival(mldsa_montred a) == &(inverse_mod 8380417 4294967296) * a')
              (mod &8380417) /\
              (l - &17996808470921216) div &2 pow 32 <= ival(mldsa_montred a) /\
              ival(mldsa_montred a) <= &1 + (u + &17996808462540799) div &2 pow 32`,
  REPEAT GEN_TAC THEN STRIP_TAC THEN STRIP_TAC THEN
  CONV_TAC NUM_REDUCE_CONV THEN CONV_TAC(ONCE_DEPTH_CONV INVERSE_MOD_CONV) THEN
  MP_TAC(SPECL [`&(inverse_mod 8380417 4294967296):int`; `(&2:int) pow 32`; `&8380417:int`] (INTEGER_RULE
   `!d e n:int. (e * d == &1) (mod n)
                ==> !x y. ((x == d * y) (mod n) <=> (e * x == y) (mod n))`)) THEN
  CONV_TAC(ONCE_DEPTH_CONV INVERSE_MOD_CONV) THEN
  ANTS_TAC THENL
   [REWRITE_TAC[GSYM INT_REM_EQ] THEN CONV_TAC INT_REDUCE_CONV;
    DISCH_THEN(fun th -> REWRITE_TAC[th])] THEN
  ONCE_REWRITE_TAC[INT_ARITH
   `l:int <= x <=> &2 pow 32 * l <= &2 pow 32 * x`] THEN
  REWRITE_TAC[MONTRED_MLDSA_LEMMA] THEN
  REWRITE_TAC[WORD_RULE
   `word_add (word_mul a b) c = iword(ival a * ival b + ival c)`] THEN
  ASM_SIMP_TAC[IVAL_WORD_SX; DIMINDEX_32; DIMINDEX_64; ARITH] THEN
  W(MP_TAC o PART_MATCH (lhand o rand) IVAL_IWORD o
   lhand o rator o lhand o snd) THEN
  REWRITE_TAC[DIMINDEX_64] THEN CONV_TAC(DEPTH_CONV WORD_NUM_RED_CONV) THEN
  ANTS_TAC THENL
   [SUBGOAL_THEN
     `--(&9205375228383854592) <= ival(a:int64) /\
      ival(a:int64) <= &9205375228392235008`
    MP_TAC THENL [ASM_INT_ARITH_TAC; ALL_TAC] THEN
    POP_ASSUM_LIST(K ALL_TAC) THEN STRIP_TAC THEN
    ASM BOUNDER_TAC[];
    DISCH_THEN SUBST1_TAC] THEN
  ASM_REWRITE_TAC[INTEGER_RULE
   `(a * p + x:int == y) (mod p) <=> (x == y) (mod p)`] THEN
  CONJ_TAC THENL
   [FIRST_X_ASSUM(MATCH_MP_TAC o MATCH_MP (INT_ARITH
     `l:int <= a ==> x - l <= p ==> x <= p + a`)) THEN
    TRANS_TAC INT_LE_TRANS `--(&2 pow 31) *  &8380417:int` THEN
    CONJ_TAC THENL [ASM_INT_ARITH_TAC; BOUNDER_TAC[]];
    FIRST_X_ASSUM(MATCH_MP_TAC o MATCH_MP (INT_ARITH
     `a:int <= u ==> x <= p - u ==> x + a <= p`)) THEN
    TRANS_TAC INT_LE_TRANS `(&2 pow 31 - &1) *  &8380417:int` THEN
    CONJ_TAC THENL [BOUNDER_TAC[]; ASM_INT_ARITH_TAC]]);;

let CONGBOUND_MLDSA_BARRED = prove
 (`!a a' l u.
        ((ival a == a') (mod &8380417) /\ l <= ival a /\ ival a <= u)
        ==> u <= &0x7fbfffff
            ==> (ival(mldsa_barred a) == a') (mod &8380417) /\
                --(&6283009) <= ival(mldsa_barred a) /\
                ival(mldsa_barred a) <= &6283008`,
  REPEAT GEN_TAC THEN STRIP_TAC THEN STRIP_TAC THEN
  MP_TAC(ISPEC `a:int32` (BITBLAST_RULE
     `!a:int32.
        let ML_DSA_Q = &8380417 in
        let t = word_ishr (word_add a (word 4194304)) 23 in
        let r = word_sub a (word_mul t (word 8380417)) in
        ival(a) < &0x7fc00000
        ==> ival(a) - ML_DSA_Q * ival t = ival r /\
            --(&6283009) <= ival r /\ ival r <= &6283008`)) THEN
  CONV_TAC(TOP_DEPTH_CONV let_CONV) THEN
  ASM_REWRITE_TAC[GSYM mldsa_barred] THEN
  ANTS_TAC THENL [ASM_INT_ARITH_TAC; ALL_TAC] THEN
  DISCH_THEN(fun th -> REWRITE_TAC[GSYM th]) THEN
  ASM_REWRITE_TAC[INTEGER_RULE
   `(x - p * q:int == y) (mod p) <=> (x == y) (mod p)`]);;

let CONGBOUND_MLDSA_MONTMUL = prove
 (`!x x' lx ux.
       ((ival x == x') (mod &8380417) /\ lx <= ival x /\ ival x <= ux)
       ==> !a b. --(&2147483648) <= ival a /\
                 ival a <= &2147483647 /\
                 (&8380417 * ival b) rem &4294967296 = ival a rem &4294967296
                 ==> (ival(mldsa_montmul (a,b) x) ==
                     &(inverse_mod 8380417 4294967296) * ival a * x')
                     (mod &8380417) /\
                     (min (ival a * lx) (ival a * ux) - &17996808462540799)
                     div &4294967296 <= ival(mldsa_montmul (a,b) x) /\
                     ival(mldsa_montmul (a,b) x) <=
                     (max (ival a * lx) (ival a * ux) + &17996812765888511)
                     div &2 pow 32`,
  let lemma = prove
   (`l:int <= x /\ x <= u
     ==> !a. a * l <= a * x /\ a * x <= a * u \/
             a * u <= a * x /\ a * x <= a * l`,
    MESON_TAC[INT_LE_NEGTOTAL; INT_LE_LMUL;
              INT_ARITH `a * x:int <= a * y <=> --a * y <= --a * x`])
  and ilemma = prove
   (`!x:int64. ival(word_subword x (32,32):int32) = ival x div &2 pow 32`,
    REWRITE_TAC[GSYM DIMINDEX_16; GSYM IVAL_WORD_ISHR] THEN
    GEN_TAC THEN REWRITE_TAC[DIMINDEX_16] THEN BITBLAST_TAC) in
  let mainlemma = prove
   (`!x:int64 y:int64.
          (ival x == ival y) (mod (&2 pow 32))
          ==> &2 pow 32 *
              ival(word_sub (word_subword x (32,32))
                            (word_subword y (32,32)):int32) =
              ival(word_sub x y)`,
    REPEAT STRIP_TAC THEN MATCH_MP_TAC(INT_ARITH
     `b rem &2 pow 32 = &0 /\ a = &2 pow 32 * b div &2 pow 32 ==> a = b`) THEN
    CONJ_TAC THENL
     [REWRITE_TAC[WORD_SUB_IMODULAR; imodular; INT_REM_EQ_0] THEN
      SIMP_TAC[INT_DIVIDES_IVAL_IWORD; DIMINDEX_64; ARITH] THEN
      POP_ASSUM MP_TAC THEN CONV_TAC INTEGER_RULE;
      AP_TERM_TAC THEN REWRITE_TAC[GSYM ilemma] THEN AP_TERM_TAC] THEN
    FIRST_X_ASSUM(MP_TAC o GEN_REWRITE_RULE I [GSYM INT_REM_EQ]) THEN
    SIMP_TAC[INT_REM_IVAL; DIMINDEX_16; DIMINDEX_64; ARITH] THEN
    BITBLAST_TAC) in
  REPEAT GEN_TAC THEN DISCH_TAC THEN REPEAT GEN_TAC THEN STRIP_TAC THEN
  CONV_TAC NUM_REDUCE_CONV THEN CONV_TAC(ONCE_DEPTH_CONV INVERSE_MOD_CONV) THEN
  MP_TAC(SPECL [`&8265825:int`; `(&2:int) pow 32`; `&8380417:int`] (INTEGER_RULE
 `!d e n:int. (e * d == &1) (mod n)
              ==> !x y. ((x == d * y) (mod n) <=> (e * x == y) (mod n))`)) THEN
  ANTS_TAC THENL
   [REWRITE_TAC[GSYM INT_REM_EQ] THEN INT_ARITH_TAC;
    DISCH_THEN(fun th -> REWRITE_TAC[th])] THEN
  ONCE_REWRITE_TAC[INT_ARITH
   `l:int <= x <=> &2 pow 32 * l <= &2 pow 32 * x`] THEN
  REWRITE_TAC[mldsa_montmul] THEN
  REWRITE_TAC[WORD_MUL_IMODULAR; imodular] THEN
  SIMP_TAC[IVAL_WORD_SX; DIMINDEX_32; DIMINDEX_64; ARITH] THEN
  CONV_TAC WORD_REDUCE_CONV THEN
  W(MP_TAC o PART_MATCH (lhand o rand) mainlemma o
   lhand o rator o lhand o snd) THEN
  ANTS_TAC THENL
   [SIMP_TAC[GSYM INT_REM_EQ; INT_REM_IVAL_IWORD; DIMINDEX_64; ARITH] THEN
    ONCE_REWRITE_TAC[GSYM INT_MUL_REM] THEN
    SIMP_TAC[GSYM VAL_IVAL_REM; GSYM DIMINDEX_32] THEN
    SIMP_TAC[WORD_SUBWORD_EQUAL_WORD_ZX_POS0; DIMINDEX_32; DIMINDEX_64;
             VAL_WORD_ZX_GEN; ARITH_LE; ARITH_LT] THEN
    ONCE_REWRITE_TAC[ARITH_RULE `32 = MIN 64 32`] THEN
    REWRITE_TAC[GSYM MOD_MOD_EXP_MIN] THEN
    REWRITE_TAC[GSYM INT_OF_NUM_REM; GSYM INT_OF_NUM_CLAUSES] THEN
    REWRITE_TAC[REWRITE_RULE[GSYM INT_REM_EQ; DIMINDEX_64]
     (INST_TYPE [`:64`,`:N`] VAL_IWORD_CONG)] THEN
    REWRITE_TAC[INT_REM_REM_POW_MIN] THEN CONV_TAC NUM_REDUCE_CONV THEN
    CONV_TAC INT_REM_DOWN_CONV THEN
    REWRITE_TAC[GSYM INT_MUL_ASSOC] THEN
    ONCE_REWRITE_TAC[GSYM INT_MUL_REM] THEN
    SIMP_TAC[GSYM VAL_IVAL_REM; GSYM DIMINDEX_32] THEN
    REWRITE_TAC[DIMINDEX_32] THEN CONV_TAC INT_REM_DOWN_CONV THEN
    ONCE_REWRITE_TAC[GSYM INT_MUL_REM] THEN
    AP_THM_TAC THEN AP_TERM_TAC THEN AP_TERM_TAC THEN
    CONV_TAC INT_REDUCE_CONV THEN ASM_REWRITE_TAC[INT_MUL_SYM];
    DISCH_THEN SUBST1_TAC THEN REWRITE_TAC[GSYM IWORD_INT_SUB]] THEN
  W(MP_TAC o PART_MATCH (lhand o rand) IVAL_IWORD o
    lhand o rator o lhand o snd) THEN
  ANTS_TAC THENL
   [REWRITE_TAC[DIMINDEX_64; ARITH] THEN ASM BOUNDER_TAC[];
    DISCH_THEN SUBST1_TAC] THEN
  ASM_SIMP_TAC[INTEGER_RULE
   `(x:int == x') (mod p) ==> (x * a - q * p == a * x') (mod p)`] THEN
  REWRITE_TAC[GSYM(INT_REDUCE_CONV `(&2:int) pow 32`)] THEN
  MATCH_MP_TAC(INT_ARITH
  `(l <= p /\ p <= u) /\ (&4294967295 - c <= q /\ q <= b)
   ==> &2 pow 32 * (l - b) div &2 pow 32 <= p - q /\
       p - q <= &2 pow 32 * (u + c) div &2 pow 32`) THEN
  CONJ_TAC THENL [ALL_TAC; BOUNDER_TAC[]] THEN
  FIRST_X_ASSUM(MP_TAC o SPEC `ival(a:int64)` o
    MATCH_MP lemma o CONJUNCT2) THEN
  INT_ARITH_TAC);;

let CONCL_BOUNDS_RULE =
  CONV_RULE(BINOP2_CONV
          (LAND_CONV(RAND_CONV DIMINDEX_INT_REDUCE_CONV))
          (BINOP2_CONV
           (LAND_CONV DIMINDEX_INT_REDUCE_CONV)
           (RAND_CONV DIMINDEX_INT_REDUCE_CONV)));;

let SIDE_ELIM_RULE th =
  MP th (EQT_ELIM(DIMINDEX_INT_REDUCE_CONV(lhand(concl th))));;

let rec ASM_CONGBOUND_RULE lfn tm =
    try apply lfn tm with Failure _ ->
    match tm with
      Comb(Const("word",_),n) when is_numeral n ->
        let th1 = ISPEC tm CONGBOUND_CONST in
        let th2 = WORD_RED_CONV(lhand(lhand(snd(strip_forall(concl th1))))) in
        MATCH_MP th1 th2
    | Comb(Const("iword",_),n) when is_intconst n ->
        let th0 = WORD_IWORD_CONV tm in
        let th1 = ISPEC (rand(concl th0)) CONGBOUND_CONST in
        let th2 = WORD_RED_CONV(lhand(lhand(snd(strip_forall(concl th1))))) in
        SUBS[SYM th0] (MATCH_MP th1 th2)
    | Comb(Const("mldsa_montred",_),t) ->
        let th1 = WEAKEN_INTCONG_RULE (num 8380417)
                   (ASM_CONGBOUND_RULE lfn t) in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE(MATCH_MP CONGBOUND_MLDSA_MONTRED th1))
    | Comb(Const("mldsa_barred",_),t) ->
        let th1 = WEAKEN_INTCONG_RULE (num 8380417)
                     (ASM_CONGBOUND_RULE lfn t) in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE(MATCH_MP CONGBOUND_MLDSA_BARRED th1))
    | Comb(Comb(Const("mldsa_montmul",_),ab),t) ->
        let atm,btm = dest_pair ab and th0 = ASM_CONGBOUND_RULE lfn t in
        let th0' = WEAKEN_INTCONG_RULE (num 8380417) th0 in
        let th1 = SPECL [atm;btm] (MATCH_MP CONGBOUND_MLDSA_MONTMUL th0') in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | Comb(Const("word_sx",_),t) ->
        let th0 = ASM_CONGBOUND_RULE lfn t in
        let tyin = type_match
         (type_of(rator(rand(lhand(funpow 4 rand (snd(dest_forall
            (concl CONGBOUND_WORD_SX)))))))) (type_of(rator tm)) [] in
        let th1 = MATCH_MP (INST_TYPE tyin CONGBOUND_WORD_SX) th0 in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | Comb(Const("word_neg",_),t) ->
        let th0 = ASM_CONGBOUND_RULE lfn t in
        let th1 = MATCH_MP CONGBOUND_WORD_NEG th0 in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | Comb(Comb(Const("word_add",_),ltm),rtm) ->
        let lth = ASM_CONGBOUND_RULE lfn ltm
        and rth = ASM_CONGBOUND_RULE lfn rtm in
        let th1 = MATCH_MP CONGBOUND_WORD_ADD (UNIFY_INTCONG_RULE lth rth) in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | Comb(Comb(Const("word_sub",_),ltm),rtm) ->
        let lth = ASM_CONGBOUND_RULE lfn ltm
        and rth = ASM_CONGBOUND_RULE lfn rtm in
        let th1 = MATCH_MP CONGBOUND_WORD_SUB (UNIFY_INTCONG_RULE lth rth) in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | Comb(Comb(Const("word_mul",_),ltm),rtm) ->
        let lth = ASM_CONGBOUND_RULE lfn ltm
        and rth = ASM_CONGBOUND_RULE lfn rtm in
        let th1 = MATCH_MP CONGBOUND_WORD_MUL (UNIFY_INTCONG_RULE lth rth) in
        CONCL_BOUNDS_RULE(SIDE_ELIM_RULE th1)
    | _ -> CONCL_BOUNDS_RULE(ISPEC tm CONGBOUND_ATOM);;

let GEN_CONGBOUND_RULE aboths =
  ASM_CONGBOUND_RULE (PROCESS_BOUND_ASSUMPTIONS aboths);;

let CONGBOUND_RULE = GEN_CONGBOUND_RULE [];;

let rec LOCAL_CONGBOUND_RULE lfn asms =
  match asms with
    [] -> lfn
  | th::ths ->
      let bod,var = dest_eq (concl th) in
      let th1 = ASM_CONGBOUND_RULE lfn bod in
      let th2 = SUBS[th] th1 in
      let lfn' = (var |-> th2) lfn in
      LOCAL_CONGBOUND_RULE lfn' ths;;

(* ------------------------------------------------------------------------- *)
(* Simplify SIMD cruft and fold relevant definitions when encountered.       *)
(* The ABBREV form also introduces abbreviations for relevant subterms.      *)
(* ------------------------------------------------------------------------- *)

let SIMD_SIMPLIFY_CONV unfold_defs =
  TOP_DEPTH_CONV
   (REWR_CONV WORD_SUBWORD_AND ORELSEC WORD_SIMPLE_SUBWORD_CONV) THENC
  DEPTH_CONV WORD_NUM_RED_CONV THENC
  REWRITE_CONV (map GSYM unfold_defs);;

let SIMD_SIMPLIFY_TAC unfold_defs =
  let arm_simdable = can (term_match [] `read X (s:armstate):int128 = whatever`) in
  let x86_simdable = can (term_match [] `read X (s:x86state):int256 = whatever`) in
  let simdable tm = arm_simdable tm || x86_simdable tm in
  TRY(FIRST_X_ASSUM
   (ASSUME_TAC o
    CONV_RULE(RAND_CONV (SIMD_SIMPLIFY_CONV unfold_defs)) o
    check (simdable o concl)));;

let is_local_definition unfold_defs =
  let pats = map (lhand o snd o strip_forall o concl) unfold_defs in
  let pam t = exists (fun p -> can(term_match [] p) t) pats in
  fun tm -> is_eq tm && is_var(rand tm) && pam(lhand tm);;

let AUTO_ABBREV_TAC tm =
  let gv = genvar(type_of tm) in
  ABBREV_TAC(mk_eq(gv,tm));;

let SIMD_SIMPLIFY_ABBREV_TAC =
  let arm_simdable =
    can (term_match [] `read X (s:armstate):int128 = whatever`)
  and x86_simdable =
    can (term_match [] `read X (s:x86state):int256 = whatever`) in
  let simdable tm = arm_simdable tm || x86_simdable tm in
  fun unfold_defs unfold_aux ->
    let pats = map (lhand o snd o strip_forall o concl) unfold_defs in
    let pam t = exists (fun p -> can(term_match [] p) t) pats in
    let ttac th (asl,w) =
      let th' = CONV_RULE(RAND_CONV
                 (SIMD_SIMPLIFY_CONV (unfold_defs @ unfold_aux))) th in
      let asms =
        map snd (filter (is_local_definition unfold_defs o concl o snd) asl) in
      let th'' = GEN_REWRITE_RULE (RAND_CONV o TOP_DEPTH_CONV) asms th' in
      let tms = sort free_in (find_terms pam (rand(concl th''))) in
      (MP_TAC th'' THEN MAP_EVERY AUTO_ABBREV_TAC tms THEN DISCH_TAC) (asl,w) in
  TRY(FIRST_X_ASSUM(ttac o check (simdable o concl)));;
