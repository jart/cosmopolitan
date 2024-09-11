// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

/**
 * @fileoverview Roman Transliteration, e.g.
 *
 *     $ echo 'gaius julius cæsar' | o//examples/romanize
 *     CAIVS IVLIVS CAESAR
 *     $ echo 'гаиус юлиус цаесар' | o//examples/romanize
 *     CAIVS IVLIVS TSAESAR
 *     $ echo 'عودة أبو تايه' | o//examples/romanize
 *     EVVDTA AEBVV TAIH
 *
 */

#define PASSPORT 0

enum Mode {
  kArchaic,
  kOld,
  kClassical,
  kMedieval,
  kModern,
} mode = kModern;

bool IsHindiConsonant(wint_t c) {
  switch (c) {
    case 0x915:  // क
    case 0x916:  // ख
    case 0x917:  // ग
    case 0x918:  // घ
    case 0x91a:  // च
    case 0x91b:  // छ
    case L'ज':
    case L'झ':
    case L'ट':
    case L'ठ':
    case L'ड':
    case L'ढ':
    case L'ण':
    case 0x924:  // त
    case L'थ':
    case L'द':
    case L'ध':
    case L'न':
    case L'प':
    case L'फ':
    case L'ब':
    case 0x92d:  // भ
    case L'म':
    case L'य':
    case 0x930:  // र
    case L'ल':
    case L'व':
    case L'श':
    case L'ष':
    case L'स':
    case L'ह':
      return true;
    default:
      return false;
  }
}

bool IsHindiMagicConsonant(wint_t c) {
  switch (c) {
    case 0x902:  // ं
      return true;
    default:
      return false;
  }
}

int main(int argc, char* argv[]) {
  wint_t c1, c2;
  while ((c1 = towupper(fgetwc(stdin))) != -1) {
    if (!iswcntrl(c1)) {
      c2 = fgetwc(stdin);
      if (mode < kMedieval || !isascii(c2))
        c2 = towupper(c2);
    } else {
      c2 = 0;
    }
    switch (c1) {
      case '.':
        if (c2 == ' ') {
          fputwc(L'·', stdout);
          continue;
        }
        break;
      case '"':
      case '\'':
      case ',':
      case ';':
      case 0xFEFF:  // ZERO WIDTH NO-BREAK SPACE (UTF-8 BOM)
      case 0x200E:  // LEFT-TO-RIGHT MARK
      case 0x200F:  // RIGHT-TO-LEFT MARK
        fputwc(c1, stdout);
        break;
      case L'Ĵ':
      case L'Ј':
      case 'J':
      J:
        if (mode >= kModern) {
          fputc('J', stdout);
        } else {
          fputc('I', stdout);
        }
        break;
      case L'Ũ':
      case L'Ū':
      case L'Ŭ':
      case L'Ů':
      case L'Ű':
      case L'Ų':
      case L'Ù':
      case L'Ú':
      case L'Û':
      case L'ў':
      case L'У':
      case 0x046A:
      case 'U':
      U:
        if (mode >= kMedieval) {
          fputc('U', stdout);
        } else {
          fputc('V', stdout);
        }
        break;
      case L'Ŵ':
      case L'Ƿ':
      case 'W':
      W:
        if (mode >= kMedieval) {
          fputc('W', stdout);
        } else {
          fputc('V', stdout);
          fputc('V', stdout);
        }
        break;
      case 'Y':
      case L'Ý':
      case L'Ŷ':
      case L'Ÿ':
      case L'Ы':
      Y:
        if (mode == kClassical) {
          fputc('Y', stdout);
        } else {
          fputc('I', stdout);
        }
        break;
      case L'Ẍ':
      case L'ẍ':
      case L'Ẋ':
      case L'ẋ':
      case 'X':
        fputc('X', stdout);
        break;
      case L'Ź':
      case L'Ż':
      case L'Ž':
      case L'З':
      case 'Z':
      Z:
        if (mode == kOld) {
          fputc('G', stdout);
        } else {
          fputc('Z', stdout);
        }
        break;
      case L'Ĝ':
      case L'Ğ':
      case L'Ġ':
      case L'Ģ':
      case L'Ґ':
      case L'Г':
      case 0x0492:
      case 'G':
        if (mode >= kOld) {
          fputc('G', stdout);
        } else if (c2 == 'U' || c2 == 'O') {
          fputc('Q', stdout);
        } else {
          fputc('C', stdout);
        }
        break;
      case L'Ķ':
      case L'К':
      case 'K':
        if (mode >= kMedieval) {
          fputc('K', stdout);
          break;
        }
        if (c2 == 'O') {
          fputc('Q', stdout);
          break;
        }
        if (c2 == 'N') {
          break;
        }
        /* fallthrough */
      case 'C':
      case 0x04BA:
        switch (c2) {
          case 'A':
            if (mode >= kOld) {
              fputc('C', stdout);
            } else {
              fputc('K', stdout);
            }
            break;
          /* case 'O': */
          case 'U':
          case 'V':
            fputc('Q', stdout);
            break;
          default:
            fputc('C', stdout);
            break;
        }
        break;
      case L'Æ':
      case L'Ä':
        fputc('A', stdout);
        fputc('E', stdout);
        break;
      case L'Ĳ':
        fputc('I', stdout);
        goto J;
      case L'Þ':
        fputc('T', stdout);
        fputc('H', stdout);
        break;
      case L'Œ':
      case L'Ö':
      case L'Ø':
        fputc('O', stdout);
        fputc('E', stdout);
        break;
      case L'Ü':
        if (mode >= kMedieval) {
          fputc('U', stdout);
        } else {
          fputc('V', stdout);
        }
        fputc('E', stdout);
        break;
      case L'ẞ':
        fputc('S', stdout);
        fputc('S', stdout);
        break;
      case L'À':
      case L'Á':
      case L'Â':
      case L'Ã':
      case L'Ā':
      case L'Ă':
      case L'Ą':
      case L'А':
        fputc('A', stdout);
        break;
      case L'Ç':
      case L'Ć':
      case L'Ĉ':
      case L'Ċ':
      case L'Č':
        fputc('C', stdout);
        break;
      case L'È':
      case L'É':
      case L'Ê':
      case L'Ë':
      case L'Ē':
      case L'Ĕ':
      case L'Ė':
      case L'Ę':
      case L'Ě':
        fputc('E', stdout);
        break;
      case L'Ì':
      case L'Í':
      case L'Î':
      case L'Ï':
        fputc('I', stdout);
        break;
      case L'Ð':
      case L'Ď':
        fputc('D', stdout);
        break;
      case L'Ñ':
      case L'Ń':
      case L'Ņ':
      case L'Ň':
      case L'Ŋ':
        fputc('N', stdout);
        break;
      case L'Ò':
      case L'Ó':
      case L'Ô':
      case L'Õ':
      case L'Ō':
      case L'Ŏ':
      case L'Ő':
        fputc('O', stdout);
        break;
      default:
        fputwc(c1, stdout);
        break;
      case L'Ĥ':
      case L'Ħ':
        fputc('H', stdout);
        break;
      case L'Ĩ':
      case L'Ī':
      case L'Ĭ':
      case L'Į':
      case L'İ':
      case L'I':
      case L'И':
      case L'Й':
        fputc('I', stdout);
        break;
      case L'Ĺ':
      case L'Ļ':
      case L'Ľ':
      case L'Ŀ':
      case L'Ł':
        fputc('L', stdout);
        break;
      case L'Ŕ':
      case L'Ŗ':
      case L'Ř':
        fputc('R', stdout);
        break;
      case L'Ś':
      case L'Ŝ':
      case L'Ş':
      case L'Š':
        fputc('S', stdout);
        break;
      case L'Ţ':
      case L'Ť':
      case L'Ŧ':
        fputc('T', stdout);
        break;
      case L'Ё':
        fputc('E', stdout);
        break;
      case L'Ћ':
        fputc('D', stdout);
        break;
      case L'Є':
        fputc('I', stdout);
        fputc('E', stdout);
        break;
      case L'Ѕ':
        fputc('D', stdout);
        fputc('Z', stdout);
        break;
      case L'І':
        fputc('I', stdout);
        break;
      case L'Ї':
        fputc('I', stdout);
        break;
      case L'Љ':
        fputc('L', stdout);
        if (mode >= kMedieval) {
          fputc('J', stdout);
        } else {
          fputc('I', stdout);
        }
        break;
      case L'Њ':
        fputc('N', stdout);
        goto J;
      case L'Ќ':
        fputc('K', stdout);
        break;
      case L'Џ':
        fputc('D', stdout);
        goto Z;
      case L'Б':
        fputc('B', stdout);
        break;
      case L'В':
        fputc('V', stdout);
        break;
      case L'Д':
        fputc('D', stdout);
        break;
      case L'Е':
        fputc('E', stdout);
        break;
      case L'Ж':
        if (mode == kOld) {
          fputc('G', stdout);
        } else {
          fputc('Z', stdout);
        }
        fputc('H', stdout);
        break;
      case L'Л':
        fputc('L', stdout);
        break;
      case L'М':
        fputc('M', stdout);
        break;
      case L'Н':
        fputc('N', stdout);
        break;
      case L'О':
        fputc('O', stdout);
        break;
      case L'П':
        fputc('P', stdout);
        break;
      case L'Р':
        fputc('R', stdout);
        break;
      case L'С':
        fputc('S', stdout);
        break;
      case L'Т':
        fputc('T', stdout);
        break;
      case L'Ф':
        fputc('F', stdout);
        break;
      case L'Х':
        /* fputc('K', stdout); */
        fputc('H', stdout);
        break;
      case L'Ц':
        fputc('T', stdout);
        fputc('S', stdout);
        break;
      case L'Ч':
        fputc('C', stdout);
        fputc('H', stdout);
        break;
      case L'Ш':
        fputc('S', stdout);
        fputc('H', stdout);
        break;
      case L'Щ':
        fputc('S', stdout);
        fputc('H', stdout);
        fputc('C', stdout);
        fputc('H', stdout);
        break;
      case L'Ъ':
        fputc('I', stdout);
        fputc('E', stdout);
        break;
      case L'Э':
        fputc('E', stdout);
        break;
      case L'Ю':
        fputc('I', stdout);
        goto U;
      case L'Я':
        fputc('I', stdout);
        fputc('A', stdout);
        break;
      case L'Ȝ':
        if (mode >= kOld) {
          fputc('G', stdout);
        } else if (mode == kArchaic) {
          fputc('C', stdout);
        }
        fputc('H', stdout);
        break;
      case L'ſ':
        fputc('S', stdout);
        break;
      case 0x0621:  // hamza
        if (PASSPORT)
          fputc('X', stdout);
        fputc('E', stdout);
        break;
      case 0x0622:  // alef with madda above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('A', stdout);
        fputc('A', stdout);
        break;
      case 0x0623:  // alef with hamza above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('A', stdout);
        fputc('E', stdout);
        break;
      case 0x0624:  // waw with hamza above
        goto U;
      case 0x0625:  // alef with hamza below
        fputc('I', stdout);
        break;
      case 0x0626:  // yeh with hamza above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('I', stdout);
        break;
      case 0x0627:  // alef
        fputc('A', stdout);
        break;
      case 0x0628:  // beh
        fputc('B', stdout);
        break;
      case 0x0629:  // teh marbuta
        if (PASSPORT)
          fputc('X', stdout);
        fputc('T', stdout);
        fputc('A', stdout);
        break;
      case 0x062A:  // teh
        fputc('T', stdout);
        break;
      case 0x062B:  // theh
        if (PASSPORT)
          fputc('X', stdout);
        fputc('T', stdout);
        fputc('H', stdout);
        break;
      case 0x062C:  // jeem
        goto J;
      case 0x062D:  // hah
        if (PASSPORT)
          fputc('X', stdout);
        fputc('H', stdout);
        break;
      case 0x062E:  // khah
        if (PASSPORT)
          fputc('X', stdout);
        fputc('K', stdout);
        fputc('H', stdout);
        break;
      case 0x062F:  // dal
        fputc('D', stdout);
        break;
      case 0x0630:  // thal
        if (PASSPORT)
          fputc('X', stdout);
        fputc('D', stdout);
        fputc('H', stdout);
        break;
      case 0x0631:  // reh
        fputc('R', stdout);
        break;
      case 0x0632:  // zain
        fputc('Z', stdout);
        break;
      case 0x0633:  // seen
        fputc('S', stdout);
        break;
      case 0x0634:  // sheen
        if (PASSPORT)
          fputc('X', stdout);
        fputc('S', stdout);
        fputc('H', stdout);
        break;
      case 0x0635:  // sad
        if (PASSPORT)
          fputc('X', stdout);
        fputc('S', stdout);
        fputc('S', stdout);
        break;
      case 0x0636:  // dad
        if (PASSPORT)
          fputc('X', stdout);
        fputc('D', stdout);
        fputc('Z', stdout);
        break;
      case 0x0637:  // tah
        if (PASSPORT)
          fputc('X', stdout);
        fputc('T', stdout);
        fputc('T', stdout);
        break;
      case 0x0638:  // zah
        if (PASSPORT)
          fputc('X', stdout);
        fputc('Z', stdout);
        fputc('Z', stdout);
        break;
      case 0x0639:  // ain
        fputc('E', stdout);
        break;
      case 0x063A:  // ghain
        fputc('G', stdout);
        break;
      case 0x0641:  // feh
        fputc('F', stdout);
        break;
      case 0x0642:  // qaf
        fputc('Q', stdout);
        break;
      case 0x0643:  // kaf
        fputc('K', stdout);
        break;
      case 0x0644:  // lam
        fputc('L', stdout);
        break;
      case 0x0645:  // meem
        fputc('M', stdout);
        break;
      case 0x0646:  // noon
        fputc('N', stdout);
        break;
      case 0x0647:  // heh
        fputc('H', stdout);
        break;
      case 0x0648:  // waw
        goto W;
      case 0x0649:  // alef maksura
        if (PASSPORT)
          fputc('X', stdout);
        fputc('A', stdout);
        goto Y;
      case 0x064A:  // yeh
        goto Y;
      case 0x0671:  // alef wasla
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('A', stdout);
        break;
      case 0x0679:  // tteh
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('T', stdout);
        break;
      case 0x067C:  // teh with ring
        if (PASSPORT)
          fputc('X', stdout);
        fputc('R', stdout);
        fputc('T', stdout);
        break;
      case 0x067E:  // peh
        fputc('P', stdout);
        break;
      case 0x0681:  // hah with hamza above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('K', stdout);
        fputc('E', stdout);
        break;
      case 0x0685:  // hah with 3 dots above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('X', stdout);
        fputc('H', stdout);
        break;
      case 0x0686:  // tcheh
        if (PASSPORT)
          fputc('X', stdout);
        fputc('C', stdout);
        break;
      case 0x0688:  // ddal
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('D', stdout);
        break;
      case 0x0689:  // dal with ring
        if (PASSPORT)
          fputc('X', stdout);
        fputc('D', stdout);
        fputc('R', stdout);
        break;
      case 0x0691:  // rreh
        if (PASSPORT)
          fputc('X', stdout);
        fputc('X', stdout);
        fputc('R', stdout);
        break;
      case 0x0693:  // reh with ring
        if (PASSPORT)
          fputc('X', stdout);
        fputc('R', stdout);
        fputc('R', stdout);
        break;
      case 0x0696:  // reh with dot below and dot above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('R', stdout);
        fputc('X', stdout);
        break;
      case 0x0698:  // jeh
        if (PASSPORT)
          fputc('X', stdout);
        goto J;
      case 0x069A:  // seen with dot below and dot above
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('S', stdout);
        break;
      case 0x06A9:  // keheh
        if (PASSPORT)
          fputc('X', stdout);
        fputc('K', stdout);
        fputc('K', stdout);
        break;
      case 0x06AB:  // kaf with ring
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('K', stdout);
        break;
      case 0x06AD:  // ng
        if (PASSPORT)
          fputc('X', stdout);
        fputc('N', stdout);
        fputc('G', stdout);
        break;
      case 0x06AF:  // gaf
        if (PASSPORT)
          fputc('X', stdout);
        fputc('G', stdout);
        fputc('G', stdout);
        break;
      case 0x06BA:  // noon ghunna
        if (PASSPORT)
          fputc('X', stdout);
        fputc('N', stdout);
        fputc('N', stdout);
        break;
      case 0x06BC:  // noon with ring
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('N', stdout);
        break;
      case 0x06BE:  // heh doachashmee
        if (PASSPORT)
          fputc('X', stdout);
        fputc('D', stdout);
        fputc('O', stdout);
        break;
      case 0x06C0:  // heh with yeh above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('Y', stdout);
        fputc('H', stdout);
        break;
      case 0x06C1:  // heh goal
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('G', stdout);
        break;
      case 0x06C2:  // heh goal with hamza above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('G', stdout);
        fputc('E', stdout);
        break;
      case 0x06C3:  // teh marbuta goal
        if (PASSPORT)
          fputc('X', stdout);
        fputc('T', stdout);
        fputc('G', stdout);
        break;
      case 0x06CC:  // farsi yeh
        if (PASSPORT)
          fputc('X', stdout);
        fputc('Y', stdout);
        fputc('A', stdout);
        break;
      case 0x06CD:  // yeh with tail
        if (PASSPORT)
          fputc('X', stdout);
        if (PASSPORT)
          fputc('X', stdout);
        fputc('Y', stdout);
        break;
      case 0x06D0:  // yeh
        goto Y;
      case 0x06D2:  // yeh barree
        if (PASSPORT)
          fputc('X', stdout);
        fputc('Y', stdout);
        fputc('B', stdout);
        break;
      case 0x06D3:  // yeh barree with hamza above
        if (PASSPORT)
          fputc('X', stdout);
        fputc('B', stdout);
        fputc('E', stdout);
        break;
      case 0x069C:  // seen with 3 dots below and 3 dots above
      case 0x06A2:  // feh with dot moved below
      case 0x06A7:  // qaf with dot above
      case 0x06A8:  // qaf with 3 dots above
      case 0x0651:  // shadda
      case 0x0652:  // sukun
      case 0x0670:  // superscript alef
      case 0x064B:  // fathatan
      case 0x064C:  // dammatan
      case 0x064D:  // kasratan
      case 0x064E:  // fatha
      case 0x064F:  // damma
      case 0x0650:  // kasra
      case 0x0640:  // tatwheel
        break;

      //
      // HINDI
      //
      // The following C code for the romanization of Hindi was designed
      // and written by vasant and jart on 2024-08-20.
      //
      // भारत देश का नाम है,
      // तिरंगा झंडा इसकी शान है।
      // अलग-अलग हैं बोली-भाषा,
      // कहीं पहाड़, तो कहीं मैदान हैं।
      // बहुत बड़ा है देश हमारा,
      // परम्पराओं पर हमको अभिमान है।
      // अनेकता में एकता,
      // यही हमारा संविधान है।
      //
      // BHARAT DESH KA NAM HAI,
      // TIRANGA JHANDA ISAKII SHAN HAI.
      // ALAG-ALAG HAIN BOLII-BHASSA,
      // KAHIIN PAHAD, TO KAHIIN MAIDAN HAIN.
      // BAHUT BADA HAI DESH HAMARA,
      // PARAMPARAON PAR HAMAKO ABHIMAN HAI.
      // ANEKATA MEN EKATA,
      // YAHII HAMARA SANVIDHAN HAI.
      //

      // Hindi Consonants
      case 0x915:  // क
        fputs("K", stdout);
        break;
      case 0x916:  // ख
        fputs("KH", stdout);
        break;
      case 0x917:  // ग
        fputs("G", stdout);
        break;
      case 0x918:  // घ
        fputs("GH", stdout);
        break;
      case 0x91a:  // च
        fputs("CH", stdout);
        break;
      case 0x91b:  // छ
        fputs("CHH", stdout);
        break;
      case L'ज':
        fputs("J", stdout);
        break;
      case L'झ':
        fputs("JH", stdout);
        break;
      case L'ट':
        fputs("T", stdout);
        break;
      case L'ठ':
        fputs("TH", stdout);
        break;
      case L'ड':
        fputs("D", stdout);
        break;
      case L'ढ':
        fputs("DH", stdout);
        break;
      case L'ण':
        fputs("N", stdout);
        break;
      case 0x924:  // त
        fputs("T", stdout);
        break;
      case L'थ':
        fputs("TH", stdout);
        break;
      case L'द':
        fputs("D", stdout);
        break;
      case L'ध':
        fputs("DH", stdout);
        break;
      case L'न':
        fputs("N", stdout);
        break;
      case L'प':
        fputs("P", stdout);
        break;
      case L'फ':
        fputs("PH", stdout);
        break;
      case L'ब':
        fputs("B", stdout);
        break;
      case 0x92d:  // भ
        fputs("BH", stdout);
        break;
      case L'म':
        fputs("M", stdout);
        break;
      case L'य':
        fputs("Y", stdout);
        break;
      case 0x930:  // र
        fputs("R", stdout);
        break;
      case L'ल':
        fputs("L", stdout);
        break;
      case L'व':
        fputs("V", stdout);
        break;
      case L'श':
        fputs("SH", stdout);
        break;
      case L'ष':
        fputs("SS", stdout);
        break;
      case L'स':
        fputs("S", stdout);
        break;
      case L'ह':
        fputs("H", stdout);
        break;

      // Hindi Vowels
      case 0x905:  // अ
      case 0x93e:  // ा
        fputs("A", stdout);
        break;
      case 0x906:  // आ
        fputs("AA", stdout);
        break;
      case 0x907:  // इ
      case 0x93f:  // ि
        fputs("I", stdout);
        break;
      case 0x940:  // ी
      case 0x908:  // ई
        fputs("II", stdout);
        break;
      case 0x942:  // ू
      case 0x90A:  // ऊ
        fputs("UU", stdout);
        break;
      case 0x947:  // े
      case 0x90F:  // ए
        fputs("E", stdout);
        break;
      case 0x948:  // ै
      case 0x910:  // ऐ
        fputs("AI", stdout);
        break;
      case 0x94b:  // ो
      case 0x913:  // ओ
        fputs("O", stdout);
        break;
      case 0x941:  // ु
      case 0x909:  // उ
        fputs("U", stdout);
        break;
      case 0x94c:  // ौ
      case 0x914:  // औ
        fputs("AU", stdout);
        break;

      // Hindi Magic Consonants
      case 0x902:  // ं
        fputs("N", stdout);
        break;

      // Hindi Miscellaneous
      case 0x93c:  // ़ Devanagari Sign Nukta
        break;
      case 0x94d:  // ् Devanagari Sign Virama
        break;

      // Hindi Punctuation
      case L'।':
        fputc('.', stdout);
        break;
    }

    if ((IsHindiConsonant(c1) && IsHindiConsonant(c2)) ||
        (IsHindiConsonant(c1) && IsHindiMagicConsonant(c2)))
      fputs("A", stdout);

    if (c2) {
      ungetwc(c2, stdin);
    }
  }
  return 0;
}
