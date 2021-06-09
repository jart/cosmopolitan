/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __macos_charmap_h
#define __macos_charmap_h

/*

Conversion table from MacOS Roman to
"Western Europe & America" Windows codepage 1252

     Notes on Mac OS Roman:
     ----------------------

       Mac OS Roman character set is used for at least the following Mac OS
       localizations: U.S., British, Canadian French, French, Swiss
       French, German, Swiss German, Italian, Swiss Italian, Dutch,
       Swedish, Norwegian, Danish, Finnish, Spanish, Catalan,
       Portuguese, Brazilian, and the default International system.

       Not every char of the charset MacRoman has their equivalent
       in Windows CodePage1252.
       To make the mapping in most cases possible, I choosed
       most similar chars or at least the BULLET. Chars that
       do not have a direct match are marked with '***'

       The Windows codepage 1252 contains the ISO 8859-1 "Latin 1" codepage,
       with some additional printable characters in the range (0x80 - 0x9F),
       that is reserved to control codes in the ISO 8859-1 character table.

In all Mac OS encodings, character codes 0x00-0x7F are identical to ASCII

*/



ZCONST unsigned char MacRoman_to_WinCP1252[128] = {
/*  Win CP1252          UniCode  UniCode Names       */
    0xC4    ,       /*  0x00C4  #LATIN CAPITAL LETTER A WITH DIAERESIS      */
    0xC5    ,       /*  0x00C5  #LATIN CAPITAL LETTER A WITH RING ABOVE     */
    0xC7    ,       /*  0x00C7  #LATIN CAPITAL LETTER C WITH CEDILLA        */
    0xC9    ,       /*  0x00C9  #LATIN CAPITAL LETTER E WITH ACUTE      */
    0xD1    ,       /*  0x00D1  #LATIN CAPITAL LETTER N WITH TILDE      */
    0xD6    ,       /*  0x00D6  #LATIN CAPITAL LETTER O WITH DIAERESIS      */
    0xDC    ,       /*  0x00DC  #LATIN CAPITAL LETTER U WITH DIAERESIS      */
    0xE1    ,       /*  0x00E1  #LATIN SMALL LETTER A WITH ACUTE        */
    0xE0    ,       /*  0x00E0  #LATIN SMALL LETTER A WITH GRAVE        */
    0xE2    ,       /*  0x00E2  #LATIN SMALL LETTER A WITH CIRCUMFLEX       */
    0xE4    ,       /*  0x00E4  #LATIN SMALL LETTER A WITH DIAERESIS        */
    0xE3    ,       /*  0x00E3  #LATIN SMALL LETTER A WITH TILDE        */
    0xE5    ,       /*  0x00E5  #LATIN SMALL LETTER A WITH RING ABOVE       */
    0xE7    ,       /*  0x00E7  #LATIN SMALL LETTER C WITH CEDILLA      */
    0xE9    ,       /*  0x00E9  #LATIN SMALL LETTER E WITH ACUTE        */
    0xE8    ,       /*  0x00E8  #LATIN SMALL LETTER E WITH GRAVE        */
    0xEA    ,       /*  0x00EA  #LATIN SMALL LETTER E WITH CIRCUMFLEX       */
    0xEB    ,       /*  0x00EB  #LATIN SMALL LETTER E WITH DIAERESIS        */
    0xED    ,       /*  0x00ED  #LATIN SMALL LETTER I WITH ACUTE        */
    0xEC    ,       /*  0x00EC  #LATIN SMALL LETTER I WITH GRAVE        */
    0xEE    ,       /*  0x00EE  #LATIN SMALL LETTER I WITH CIRCUMFLEX       */
    0xEF    ,       /*  0x00EF  #LATIN SMALL LETTER I WITH DIAERESIS        */
    0xF1    ,       /*  0x00F1  #LATIN SMALL LETTER N WITH TILDE        */
    0xF3    ,       /*  0x00F3  #LATIN SMALL LETTER O WITH ACUTE        */
    0xF2    ,       /*  0x00F2  #LATIN SMALL LETTER O WITH GRAVE        */
    0xF4    ,       /*  0x00F4  #LATIN SMALL LETTER O WITH CIRCUMFLEX       */
    0xF6    ,       /*  0x00F6  #LATIN SMALL LETTER O WITH DIAERESIS        */
    0xF5    ,       /*  0x00F5  #LATIN SMALL LETTER O WITH TILDE        */
    0xFA    ,       /*  0x00FA  #LATIN SMALL LETTER U WITH ACUTE        */
    0xF9    ,       /*  0x00F9  #LATIN SMALL LETTER U WITH GRAVE        */
    0xFB    ,       /*  0x00FB  #LATIN SMALL LETTER U WITH CIRCUMFLEX       */
    0xFC    ,       /*  0x00FC  #LATIN SMALL LETTER U WITH DIAERESIS        */
    0x86    ,       /*  0x2020  #DAGGER     */
    0xB0    ,       /*  0x00B0  #DEGREE SIGN        */
    0xA2    ,       /*  0x00A2  #CENT SIGN      */
    0xA3    ,       /*  0x00A3  #POUND SIGN     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  #BULLET     */
    0xB6    ,       /*  0x00B6  #PILCROW SIGN       */
    0xDF    ,       /*  0x00DF  #LATIN SMALL LETTER SHARP S     */
    0xAE    ,       /*  0x00AE  #REGISTERED SIGN        */
    0xA9    ,       /*  0x00A9  #COPYRIGHT SIGN     */
    0x99    ,       /*  0x2122  #TRADE MARK SIGN        */
    0xB4    ,       /*  0x00B4  #ACUTE ACCENT       */
    0xA8    ,       /*  0x00A8  #DIAERESIS      */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xC6    ,       /*  0x00C6  #LATIN CAPITAL LETTER AE        */
    0xD8    ,       /*  0x00D8  #LATIN CAPITAL LETTER O WITH STROKE     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xB1    ,       /*  0x00B1  #PLUS-MINUS SIGN        */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x00A5  #YEN SIGN       */
    0xB5    ,       /*  0x00B5  #MICRO SIGN     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xAA    ,       /*  0x00AA  #FEMININE ORDINAL INDICATOR     */
    0xBA    ,       /*  0x00BA  #MASCULINE ORDINAL INDICATOR        */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xE6    ,       /*  0x00E6  #LATIN SMALL LETTER AE      */
    0xF8    ,       /*  0x00F8  #LATIN SMALL LETTER O WITH STROKE       */
    0xBF    ,       /*  0x00BF  #INVERTED QUESTION MARK     */
    0xA1    ,       /*  0x00A1  #INVERTED EXCLAMATION MARK      */
    0xAC    ,       /*  0x00AC  #NOT SIGN       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x83    ,       /*  0x0192  #LATIN SMALL LETTER F WITH HOOK     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xAB    ,       /*  0x00AB  #LEFT-POINTING DOUBLE ANGLE QUOTATION MARK      */
    0xBB    ,       /*  0x00BB  #RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK     */
    0x85    ,       /*  0x2026  #HORIZONTAL ELLIPSIS        */
    0xA0    ,       /*  0x00A0  #NO-BREAK SPACE     */
    0xC0    ,       /*  0x00C0  #LATIN CAPITAL LETTER A WITH GRAVE      */
    0xC3    ,       /*  0x00C3  #LATIN CAPITAL LETTER A WITH TILDE      */
    0xD5    ,       /*  0x00D5  #LATIN CAPITAL LETTER O WITH TILDE      */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x96    ,       /*  0x2013  #EN DASH        */
    0x97    ,       /*  0x2014  #EM DASH        */
    0x93    ,       /*  0x201C  #LEFT DOUBLE QUOTATION MARK     */
    0x94    ,       /*  0x201D  #RIGHT DOUBLE QUOTATION MARK        */
    0x91    ,       /*  0x2018  #LEFT SINGLE QUOTATION MARK     */
    0x92    ,       /*  0x2019  #RIGHT SINGLE QUOTATION MARK        */
    0xF7    ,       /*  0x00F7  #DIVISION SIGN      */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xFF    ,       /*  0x00FF  #LATIN SMALL LETTER Y WITH DIAERESIS        */
    0x9F    ,       /*  0x0178  #LATIN CAPITAL LETTER Y WITH DIAERESIS      */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xA4    ,       /*  0x00A4  #CURRENCY SIGN      */
    0x8B    ,       /*  0x2039  #SINGLE LEFT-POINTING ANGLE QUOTATION MARK      */
    0x9B    ,       /*  0x203A  #SINGLE RIGHT-POINTING ANGLE QUOTATION MARK     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x87    ,       /*  0x2021  #DOUBLE DAGGER      */
    0xB7    ,       /*  0x00B7  #MIDDLE DOT     */
    0x82    ,       /*  0x201A  #SINGLE LOW-9 QUOTATION MARK        */
    0x84    ,       /*  0x201E  #DOUBLE LOW-9 QUOTATION MARK        */
    0x89    ,       /*  0x2030  #PER MILLE SIGN     */
    0xC2    ,       /*  0x00C2  #LATIN CAPITAL LETTER A WITH CIRCUMFLEX     */
    0xCA    ,       /*  0x00CA  #LATIN CAPITAL LETTER E WITH CIRCUMFLEX     */
    0xC1    ,       /*  0x00C1  #LATIN CAPITAL LETTER A WITH ACUTE      */
    0xCB    ,       /*  0x00CB  #LATIN CAPITAL LETTER E WITH DIAERESIS      */
    0xC8    ,       /*  0x00C8  #LATIN CAPITAL LETTER E WITH GRAVE      */
    0xCD    ,       /*  0x00CD  #LATIN CAPITAL LETTER I WITH ACUTE      */
    0xCE    ,       /*  0x00CE  #LATIN CAPITAL LETTER I WITH CIRCUMFLEX     */
    0xCF    ,       /*  0x00CF  #LATIN CAPITAL LETTER I WITH DIAERESIS      */
    0xCC    ,       /*  0x00CC  #LATIN CAPITAL LETTER I WITH GRAVE      */
    0xD3    ,       /*  0x00D3  #LATIN CAPITAL LETTER O WITH ACUTE      */
    0xD4    ,       /*  0x00D4  #LATIN CAPITAL LETTER O WITH CIRCUMFLEX     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xD2    ,       /*  0x00D2  #LATIN CAPITAL LETTER O WITH GRAVE      */
    0xDA    ,       /*  0x00DA  #LATIN CAPITAL LETTER U WITH ACUTE      */
    0xDB    ,       /*  0x00DB  #LATIN CAPITAL LETTER U WITH CIRCUMFLEX     */
    0xD9    ,       /*  0x00D9  #LATIN CAPITAL LETTER U WITH GRAVE      */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x88    ,       /*  0x02C6  #MODIFIER LETTER CIRCUMFLEX ACCENT      */
    0x98    ,       /*  0x02DC  #SMALL TILDE        */
    0xAF    ,       /*  0x00AF  #MACRON     */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0xB8    ,       /*  0x00B8  #CEDILLA        */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95    ,       /*  0x2022  # ***  BULLET       */
    0x95            /*  0x2022  # ***  BULLET       */
 };



ZCONST unsigned char WinCP1252_to_MacRoman[128] = {
/*  Mac Roman           UniCode  UniCode Names       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xE2    ,       /*  0x201A  # SINGLE LOW-9 QUOTATION MARK       */
    0xC4    ,       /*  0x0192  # LATIN SMALL LETTER F WITH HOOK        */
    0xE3    ,       /*  0x201E  # DOUBLE LOW-9 QUOTATION MARK       */
    0xC9    ,       /*  0x2026  # HORIZONTAL ELLIPSIS       */
    0xA0    ,       /*  0x2020  # DAGGER        */
    0xE0    ,       /*  0x2021  # DOUBLE DAGGER     */
    0xF6    ,       /*  0x02C6  # MODIFIER LETTER CIRCUMFLEX ACCENT     */
    0xE4    ,       /*  0x2030  # PER MILLE SIGN        */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xDC    ,       /*  0x2039  # SINGLE LEFT-POINTING ANGLE QUOTATION MARK     */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xD4    ,       /*  0x2018  # LEFT SINGLE QUOTATION MARK        */
    0xD5    ,       /*  0x2019  # RIGHT SINGLE QUOTATION MARK       */
    0xD2    ,       /*  0x201C  # LEFT DOUBLE QUOTATION MARK        */
    0xD3    ,       /*  0x201D  # RIGHT DOUBLE QUOTATION MARK       */
    0xA5    ,       /*  0x2022  # BULLET        */
    0xD0    ,       /*  0x2013  # EN DASH       */
    0xD1    ,       /*  0x2014  # EM DASH       */
    0xF7    ,       /*  0x02DC  # SMALL TILDE       */
    0xAA    ,       /*  0x2122  # TRADE MARK SIGN       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xDD    ,       /*  0x203A  # SINGLE RIGHT-POINTING ANGLE QUOTATION MARK        */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xD9    ,       /*  0x0178  # LATIN CAPITAL LETTER Y WITH DIAERESIS     */
    0xCA    ,       /*  0x00A0  # NO-BREAK SPACE        */
    0xC1    ,       /*  0x00A1  # INVERTED EXCLAMATION MARK     */
    0xA2    ,       /*  0x00A2  # CENT SIGN     */
    0xA3    ,       /*  0x00A3  # POUND SIGN        */
    0xDB    ,       /*  0x00A4  # CURRENCY SIGN     */
    0xB4    ,       /*  0x00A5  # YEN SIGN      */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xAC    ,       /*  0x00A8  # DIAERESIS     */
    0xA9    ,       /*  0x00A9  # COPYRIGHT SIGN        */
    0xBB    ,       /*  0x00AA  # FEMININE ORDINAL INDICATOR        */
    0xC7    ,       /*  0x00AB  # LEFT-POINTING DOUBLE ANGLE QUOTATION MARK     */
    0xC2    ,       /*  0x00AC  # NOT SIGN      */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA8    ,       /*  0x00AE  # REGISTERED SIGN       */
    0xF8    ,       /*  0x00AF  # MACRON        */
    0xA1    ,       /*  0x00B0  # DEGREE SIGN       */
    0xB1    ,       /*  0x00B1  # PLUS-MINUS SIGN       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xAB    ,       /*  0x00B4  # ACUTE ACCENT      */
    0xB5    ,       /*  0x00B5  # MICRO SIGN        */
    0xA6    ,       /*  0x00B6  # PILCROW SIGN      */
    0xE1    ,       /*  0x00B7  # MIDDLE DOT        */
    0xFC    ,       /*  0x00B8  # CEDILLA       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xBC    ,       /*  0x00BA  # MASCULINE ORDINAL INDICATOR       */
    0xC8    ,       /*  0x00BB  # RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK        */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xC0    ,       /*  0x00BF  # INVERTED QUESTION MARK        */
    0xCB    ,       /*  0x00C0  # LATIN CAPITAL LETTER A WITH GRAVE     */
    0xE7    ,       /*  0x00C1  # LATIN CAPITAL LETTER A WITH ACUTE     */
    0xE5    ,       /*  0x00C2  # LATIN CAPITAL LETTER A WITH CIRCUMFLEX        */
    0xCC    ,       /*  0x00C3  # LATIN CAPITAL LETTER A WITH TILDE     */
    0x80    ,       /*  0x00C4  # LATIN CAPITAL LETTER A WITH DIAERESIS     */
    0x81    ,       /*  0x00C5  # LATIN CAPITAL LETTER A WITH RING ABOVE        */
    0xAE    ,       /*  0x00C6  # LATIN CAPITAL LETTER AE       */
    0x82    ,       /*  0x00C7  # LATIN CAPITAL LETTER C WITH CEDILLA       */
    0xE9    ,       /*  0x00C8  # LATIN CAPITAL LETTER E WITH GRAVE     */
    0x83    ,       /*  0x00C9  # LATIN CAPITAL LETTER E WITH ACUTE     */
    0xE6    ,       /*  0x00CA  # LATIN CAPITAL LETTER E WITH CIRCUMFLEX        */
    0xE8    ,       /*  0x00CB  # LATIN CAPITAL LETTER E WITH DIAERESIS     */
    0xED    ,       /*  0x00CC  # LATIN CAPITAL LETTER I WITH GRAVE     */
    0xEA    ,       /*  0x00CD  # LATIN CAPITAL LETTER I WITH ACUTE     */
    0xEB    ,       /*  0x00CE  # LATIN CAPITAL LETTER I WITH CIRCUMFLEX        */
    0xEC    ,       /*  0x00CF  # LATIN CAPITAL LETTER I WITH DIAERESIS     */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0x84    ,       /*  0x00D1  # LATIN CAPITAL LETTER N WITH TILDE     */
    0xF1    ,       /*  0x00D2  # LATIN CAPITAL LETTER O WITH GRAVE     */
    0xEE    ,       /*  0x00D3  # LATIN CAPITAL LETTER O WITH ACUTE     */
    0xEF    ,       /*  0x00D4  # LATIN CAPITAL LETTER O WITH CIRCUMFLEX        */
    0xCD    ,       /*  0x00D5  # LATIN CAPITAL LETTER O WITH TILDE     */
    0x85    ,       /*  0x00D6  # LATIN CAPITAL LETTER O WITH DIAERESIS     */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xAF    ,       /*  0x00D8  # LATIN CAPITAL LETTER O WITH STROKE        */
    0xF4    ,       /*  0x00D9  # LATIN CAPITAL LETTER U WITH GRAVE     */
    0xF2    ,       /*  0x00DA  # LATIN CAPITAL LETTER U WITH ACUTE     */
    0xF3    ,       /*  0x00DB  # LATIN CAPITAL LETTER U WITH CIRCUMFLEX        */
    0x86    ,       /*  0x00DC  # LATIN CAPITAL LETTER U WITH DIAERESIS     */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA7    ,       /*  0x00DF  # LATIN SMALL LETTER SHARP S        */
    0x88    ,       /*  0x00E0  # LATIN SMALL LETTER A WITH GRAVE       */
    0x87    ,       /*  0x00E1  # LATIN SMALL LETTER A WITH ACUTE       */
    0x89    ,       /*  0x00E2  # LATIN SMALL LETTER A WITH CIRCUMFLEX      */
    0x8B    ,       /*  0x00E3  # LATIN SMALL LETTER A WITH TILDE       */
    0x8A    ,       /*  0x00E4  # LATIN SMALL LETTER A WITH DIAERESIS       */
    0x8C    ,       /*  0x00E5  # LATIN SMALL LETTER A WITH RING ABOVE      */
    0xBE    ,       /*  0x00E6  # LATIN SMALL LETTER AE     */
    0x8D    ,       /*  0x00E7  # LATIN SMALL LETTER C WITH CEDILLA     */
    0x8F    ,       /*  0x00E8  # LATIN SMALL LETTER E WITH GRAVE       */
    0x8E    ,       /*  0x00E9  # LATIN SMALL LETTER E WITH ACUTE       */
    0x90    ,       /*  0x00EA  # LATIN SMALL LETTER E WITH CIRCUMFLEX      */
    0x91    ,       /*  0x00EB  # LATIN SMALL LETTER E WITH DIAERESIS       */
    0x93    ,       /*  0x00EC  # LATIN SMALL LETTER I WITH GRAVE       */
    0x92    ,       /*  0x00ED  # LATIN SMALL LETTER I WITH ACUTE       */
    0x94    ,       /*  0x00EE  # LATIN SMALL LETTER I WITH CIRCUMFLEX      */
    0x95    ,       /*  0x00EF  # LATIN SMALL LETTER I WITH DIAERESIS       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0x96    ,       /*  0x00F1  # LATIN SMALL LETTER N WITH TILDE       */
    0x98    ,       /*  0x00F2  # LATIN SMALL LETTER O WITH GRAVE       */
    0x97    ,       /*  0x00F3  # LATIN SMALL LETTER O WITH ACUTE       */
    0x99    ,       /*  0x00F4  # LATIN SMALL LETTER O WITH CIRCUMFLEX      */
    0x9B    ,       /*  0x00F5  # LATIN SMALL LETTER O WITH TILDE       */
    0x9A    ,       /*  0x00F6  # LATIN SMALL LETTER O WITH DIAERESIS       */
    0xD6    ,       /*  0x00F7  # DIVISION SIGN     */
    0xBF    ,       /*  0x00F8  # LATIN SMALL LETTER O WITH STROKE      */
    0x9D    ,       /*  0x00F9  # LATIN SMALL LETTER U WITH GRAVE       */
    0x9C    ,       /*  0x00FA  # LATIN SMALL LETTER U WITH ACUTE       */
    0x9E    ,       /*  0x00FB  # LATIN SMALL LETTER U WITH CIRCUMFLEX      */
    0x9F    ,       /*  0x00FC  # LATIN SMALL LETTER U WITH DIAERESIS       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xA5    ,       /*  0x2022  # ***  BULLET       */
    0xD8            /*  0x00FF  # LATIN SMALL LETTER Y WITH DIAERESIS       */
 };


/*

The following characters has no equivalent
to each other:

MacCodes
164 0xA4    0x00A7  # SECTION SIGN
253 0xFD    0x02DD  # DOUBLE ACUTE ACCENT
189 0xBD    0x03A9  # GREEK CAPITAL LETTER OMEGA
185 0xB9    0x03C0  # GREEK SMALL LETTER PI
255 0xFF    0x02C7  # CARON
249 0xF9    0x02D8  # BREVE
250 0xFA    0x02D9  # DOT ABOVE
251 0xFB    0x02DA  # RING ABOVE
254 0xFE    0x02DB  # OGONEK
218 0xDA    0x2044  # FRACTION SLASH
182 0xB6    0x2202  # PARTIAL DIFFERENTIAL
198 0xC6    0x2206  # INCREMENT
184 0xB8    0x220F  # N-ARY PRODUCT
183 0xB7    0x2211  # N-ARY SUMMATION
195 0xC3    0x221A  # SQUARE ROOT
176 0xB0    0x221E  # INFINITY
186 0xBA    0x222B  # INTEGRAL
197 0xC5    0x2248  # ALMOST EQUAL TO
173 0xAD    0x2260  # NOT EQUAL TO
178 0xB2    0x2264  # LESS-THAN OR EQUAL TO
179 0xB3    0x2265  # GREATER-THAN OR EQUAL TO
215 0xD7    0x25CA  # LOZENGE
240 0xF0    0xF8FF  # Apple logo
222 0xDE    0xFB01  # LATIN SMALL LIGATURE FI
223 0xDF    0xFB02  # LATIN SMALL LIGATURE FL
245 0xF5    0x0131  # LATIN SMALL LETTER DOTLESS I
206 0xCE    0x0152  # LATIN CAPITAL LIGATURE OE
207 0xCF    0x0153  # LATIN SMALL LIGATURE OE

WinCodes
129 0x81            #UNDEFINED
141 0x8D            #UNDEFINED
143 0x8F            #UNDEFINED
144 0x90            #UNDEFINED
157 0x9D            #UNDEFINED
167 0xA7    0x00A7  #SECTION SIGN
173 0xAD    0x00AD  #SOFT HYPHEN
178 0xB2    0x00B2  #SUPERSCRIPT TWO
179 0xB3    0x00B3  #SUPERSCRIPT THREE
185 0xB9    0x00B9  #SUPERSCRIPT ONE
188 0xBC    0x00BC  #VULGAR FRACTION ONE QUARTER
189 0xBD    0x00BD  #VULGAR FRACTION ONE HALF
190 0xBE    0x00BE  #VULGAR FRACTION THREE QUARTERS
208 0xD0    0x00D0  #LATIN CAPITAL LETTER ETH
215 0xD7    0x00D7  #MULTIPLICATION SIGN
221 0xDD    0x00DD  #LATIN CAPITAL LETTER Y WITH ACUTE
222 0xDE    0x00DE  #LATIN CAPITAL LETTER THORN
240 0xF0    0x00F0  #LATIN SMALL LETTER ETH
253 0xFD    0x00FD  #LATIN SMALL LETTER Y WITH ACUTE
254 0xFE    0x00FE  #LATIN SMALL LETTER THORN
140 0x8C    0x0152  #LATIN CAPITAL LIGATURE OE
156 0x9C    0x0153  #LATIN SMALL LIGATURE OE
138 0x8A    0x0160  #LATIN CAPITAL LETTER S WITH CARON
154 0x9A    0x0161  #LATIN SMALL LETTER S WITH CARON
142 0x8E    0x017D  #LATIN CAPITAL LETTER Z WITH CARON
158 0x9E    0x017E  #LATIN SMALL LETTER Z WITH CARON
128 0x80    0x20AC  #EURO SIGN
166 0xA6    0x00A6  #BROKEN BAR


*/




#endif /* !__macos_charmap_h */
