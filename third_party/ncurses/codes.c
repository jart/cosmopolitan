/* This file was generated by MKcodes.awk */

#include "curses.priv.h"

#define IT NCURSES_CONST char * const


#if BROKEN_LINKER || USE_REENTRANT

static const char _nc_code_blob[] = 
"bw\0" "am\0" "xb\0" "xs\0" "xn\0" "eo\0" "gn\0" "hc\0" "km\0" "hs\0" "in\0" \
"da\0" "db\0" "mi\0" "ms\0" "os\0" "es\0" "xt\0" "hz\0" "ul\0" "xo\0" "nx\0" \
"5i\0" "HC\0" "NR\0" "NP\0" "ND\0" "cc\0" "ut\0" "hl\0" "YA\0" "YB\0" "YC\0" \
"YD\0" "YE\0" "YF\0" "YG\0" "co\0" "it\0" "li\0" "lm\0" "sg\0" "pb\0" "vt\0" \
"ws\0" "Nl\0" "lh\0" "lw\0" "ma\0" "MW\0" "Co\0" "pa\0" "NC\0" "Ya\0" "Yb\0" \
"Yc\0" "Yd\0" "Ye\0" "Yf\0" "Yg\0" "Yh\0" "Yi\0" "Yj\0" "Yk\0" "Yl\0" "Ym\0" \
"Yn\0" "BT\0" "Yo\0" "Yp\0" "bt\0" "bl\0" "cr\0" "cs\0" "ct\0" "cl\0" "ce\0" \
"cd\0" "ch\0" "CC\0" "cm\0" "do\0" "ho\0" "vi\0" "le\0" "CM\0" "ve\0" "nd\0" \
"ll\0" "up\0" "vs\0" "dc\0" "dl\0" "ds\0" "hd\0" "as\0" "mb\0" "md\0" "ti\0" \
"dm\0" "mh\0" "im\0" "mk\0" "mp\0" "mr\0" "so\0" "us\0" "ec\0" "ae\0" "me\0" \
"te\0" "ed\0" "ei\0" "se\0" "ue\0" "vb\0" "ff\0" "fs\0" "i1\0" "is\0" "i3\0" \
"if\0" "ic\0" "al\0" "ip\0" "kb\0" "ka\0" "kC\0" "kt\0" "kD\0" "kL\0" "kd\0" \
"kM\0" "kE\0" "kS\0" "k0\0" "k1\0" "k;\0" "k2\0" "k3\0" "k4\0" "k5\0" "k6\0" \
"k7\0" "k8\0" "k9\0" "kh\0" "kI\0" "kA\0" "kl\0" "kH\0" "kN\0" "kP\0" "kr\0" \
"kF\0" "kR\0" "kT\0" "ku\0" "ke\0" "ks\0" "l0\0" "l1\0" "la\0" "l2\0" "l3\0" \
"l4\0" "l5\0" "l6\0" "l7\0" "l8\0" "l9\0" "mo\0" "mm\0" "nw\0" "pc\0" "DC\0" \
"DL\0" "DO\0" "IC\0" "SF\0" "AL\0" "LE\0" "RI\0" "SR\0" "UP\0" "pk\0" "pl\0" \
"px\0" "ps\0" "pf\0" "po\0" "rp\0" "r1\0" "r2\0" "r3\0" "rf\0" "rc\0" "cv\0" \
"sc\0" "sf\0" "sr\0" "sa\0" "st\0" "wi\0" "ta\0" "ts\0" "uc\0" "hu\0" "iP\0" \
"K1\0" "K3\0" "K2\0" "K4\0" "K5\0" "pO\0" "rP\0" "ac\0" "pn\0" "kB\0" "SX\0" \
"RX\0" "SA\0" "RA\0" "XN\0" "XF\0" "eA\0" "LO\0" "LF\0" "@1\0" "@2\0" "@3\0" \
"@4\0" "@5\0" "@6\0" "@7\0" "@8\0" "@9\0" "@0\0" "%1\0" "%2\0" "%3\0" "%4\0" \
"%5\0" "%6\0" "%7\0" "%8\0" "%9\0" "%0\0" "&1\0" "&2\0" "&3\0" "&4\0" "&5\0" \
"&6\0" "&7\0" "&8\0" "&9\0" "&0\0" "*1\0" "*2\0" "*3\0" "*4\0" "*5\0" "*6\0" \
"*7\0" "*8\0" "*9\0" "*0\0" "#1\0" "#2\0" "#3\0" "#4\0" "%a\0" "%b\0" "%c\0" \
"%d\0" "%e\0" "%f\0" "%g\0" "%h\0" "%i\0" "%j\0" "!1\0" "!2\0" "!3\0" "RF\0" \
"F1\0" "F2\0" "F3\0" "F4\0" "F5\0" "F6\0" "F7\0" "F8\0" "F9\0" "FA\0" "FB\0" \
"FC\0" "FD\0" "FE\0" "FF\0" "FG\0" "FH\0" "FI\0" "FJ\0" "FK\0" "FL\0" "FM\0" \
"FN\0" "FO\0" "FP\0" "FQ\0" "FR\0" "FS\0" "FT\0" "FU\0" "FV\0" "FW\0" "FX\0" \
"FY\0" "FZ\0" "Fa\0" "Fb\0" "Fc\0" "Fd\0" "Fe\0" "Ff\0" "Fg\0" "Fh\0" "Fi\0" \
"Fj\0" "Fk\0" "Fl\0" "Fm\0" "Fn\0" "Fo\0" "Fp\0" "Fq\0" "Fr\0" "cb\0" "MC\0" \
"ML\0" "MR\0" "Lf\0" "SC\0" "DK\0" "RC\0" "CW\0" "WG\0" "HU\0" "DI\0" "QD\0" \
"TO\0" "PU\0" "fh\0" "PA\0" "WA\0" "u0\0" "u1\0" "u2\0" "u3\0" "u4\0" "u5\0" \
"u6\0" "u7\0" "u8\0" "u9\0" "op\0" "oc\0" "Ic\0" "Ip\0" "sp\0" "Sf\0" "Sb\0" \
"ZA\0" "ZB\0" "ZC\0" "ZD\0" "ZE\0" "ZF\0" "ZG\0" "ZH\0" "ZI\0" "ZJ\0" "ZK\0" \
"ZL\0" "ZM\0" "ZN\0" "ZO\0" "ZP\0" "ZQ\0" "ZR\0" "ZS\0" "ZT\0" "ZU\0" "ZV\0" \
"ZW\0" "ZX\0" "ZY\0" "ZZ\0" "Za\0" "Zb\0" "Zc\0" "Zd\0" "Ze\0" "Zf\0" "Zg\0" \
"Zh\0" "Zi\0" "Zj\0" "Zk\0" "Zl\0" "Zm\0" "Zn\0" "Zo\0" "Zp\0" "Zq\0" "Zr\0" \
"Zs\0" "Zt\0" "Zu\0" "Zv\0" "Zw\0" "Zx\0" "Zy\0" "Km\0" "Mi\0" "RQ\0" "Gm\0" \
"AF\0" "AB\0" "xl\0" "dv\0" "ci\0" "s0\0" "s1\0" "s2\0" "s3\0" "ML\0" "MT\0" \
"Xy\0" "Zz\0" "Yv\0" "Yw\0" "Yx\0" "Yy\0" "Yz\0" "YZ\0" "S1\0" "S2\0" "S3\0" \
"S4\0" "S5\0" "S6\0" "S7\0" "S8\0" "Xh\0" "Xl\0" "Xo\0" "Xr\0" "Xt\0" "Xv\0" \
"sA\0" "YI\0" "i2\0" "rs\0" "ug\0" "bs\0" "ns\0" "nc\0" "dC\0" "dN\0" "nl\0" \
"bc\0" "MT\0" "NL\0" "dB\0" "dT\0" "kn\0" "ko\0" "ma\0" "pt\0" "xr\0" "G2\0" \
"G3\0" "G1\0" "G4\0" "GR\0" "GL\0" "GU\0" "GD\0" "GH\0" "GV\0" "GC\0" "ml\0" \
"mu\0" "bx\0" ;
static const short _nc_offset_boolcodes[] = {
0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42,
45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87,
90, 93, 96, 99, 102, 105, 108, 1401, 1404, 1407, 1422, 1425, 1443, 1446, };

static NCURSES_CONST char ** ptr_boolcodes = 0;

static const short _nc_offset_numcodes[] = {
111, 114, 117, 120, 123, 126,
129, 132, 135, 138, 141, 144, 147, 150, 153, 156, 159, 162,
165, 168, 171, 174, 177, 180, 183, 186, 189, 192, 195, 198,
201, 204, 207, 1398, 1410, 1413, 1428, 1431, 1434, };

static NCURSES_CONST char ** ptr_numcodes = 0;

static const short _nc_offset_strcodes[] = {
210, 213, 216, 219, 222, 225, 228, 231, 234,
237, 240, 243, 246, 249, 252, 255, 258, 261, 264, 267, 270,
273, 276, 279, 282, 285, 288, 291, 294, 297, 300, 303, 306,
309, 312, 315, 318, 321, 324, 327, 330, 333, 336, 339, 342,
345, 348, 351, 354, 357, 360, 363, 366, 369, 372, 375, 378,
381, 384, 387, 390, 393, 396, 399, 402, 405, 408, 411, 414,
417, 420, 423, 426, 429, 432, 435, 438, 441, 444, 447, 450,
453, 456, 459, 462, 465, 468, 471, 474, 477, 480, 483, 486,
489, 492, 495, 498, 501, 504, 507, 510, 513, 516, 519, 522,
525, 528, 531, 534, 537, 540, 543, 546, 549, 552, 555, 558,
561, 564, 567, 570, 573, 576, 579, 582, 585, 588, 591, 594,
597, 600, 603, 606, 609, 612, 615, 618, 621, 624, 627, 630,
633, 636, 639, 642, 645, 648, 651, 654, 657, 660, 663, 666,
669, 672, 675, 678, 681, 684, 687, 690, 693, 696, 699, 702,
705, 708, 711, 714, 717, 720, 723, 726, 729, 732, 735, 738,
741, 744, 747, 750, 753, 756, 759, 762, 765, 768, 771, 774,
777, 780, 783, 786, 789, 792, 795, 798, 801, 804, 807, 810,
813, 816, 819, 822, 825, 828, 831, 834, 837, 840, 843, 846,
849, 852, 855, 858, 861, 864, 867, 870, 873, 876, 879, 882,
885, 888, 891, 894, 897, 900, 903, 906, 909, 912, 915, 918,
921, 924, 927, 930, 933, 936, 939, 942, 945, 948, 951, 954,
957, 960, 963, 966, 969, 972, 975, 978, 981, 984, 987, 990,
993, 996, 999, 1002, 1005, 1008, 1011, 1014, 1017, 1020, 1023,
1026, 1029, 1032, 1035, 1038, 1041, 1044, 1047, 1050, 1053, 1056,
1059, 1062, 1065, 1068, 1071, 1074, 1077, 1080, 1083, 1086, 1089,
1092, 1095, 1098, 1101, 1104, 1107, 1110, 1113, 1116, 1119, 1122,
1125, 1128, 1131, 1134, 1137, 1140, 1143, 1146, 1149, 1152, 1155,
1158, 1161, 1164, 1167, 1170, 1173, 1176, 1179, 1182, 1185, 1188,
1191, 1194, 1197, 1200, 1203, 1206, 1209, 1212, 1215, 1218, 1221,
1224, 1227, 1230, 1233, 1236, 1239, 1242, 1245, 1248, 1251, 1254,
1257, 1260, 1263, 1266, 1269, 1272, 1275, 1278, 1281, 1284, 1287,
1290, 1293, 1296, 1299, 1302, 1305, 1308, 1311, 1314, 1317, 1320,
1323, 1326, 1329, 1332, 1335, 1338, 1341, 1344, 1347, 1350, 1353,
1356, 1359, 1362, 1365, 1368, 1371, 1374, 1377, 1380, 1383, 1386,
1389, 1392, 1395, 1416, 1419,
1437, 1440, 1449, 1452,
1455, 1458, 1461, 1464, 1467, 1470, 1473, 1476, 1479, 1482, 1485,
1488, };

static NCURSES_CONST char ** ptr_strcodes = 0;


static IT *
alloc_array(NCURSES_CONST char ***value, const short *offsets, unsigned size)
{
	if (*value == 0) {
		if ((*value = typeCalloc(NCURSES_CONST char *, size + 1)) != 0) {
			unsigned n;
			for (n = 0; n < size; ++n) {
				(*value)[n] = (NCURSES_CONST char *) _nc_code_blob + offsets[n];
			}
		}
	}
	return *value;
}

#define FIX(it) NCURSES_IMPEXP IT * NCURSES_API NCURSES_PUBLIC_VAR(it)(void) { return alloc_array(&ptr_##it, _nc_offset_##it, SIZEOF(_nc_offset_##it)); }

/* remove public definition which conflicts with FIX() */
#undef boolcodes
#undef numcodes
#undef strcodes

/* add local definition */
FIX(boolcodes)
FIX(numcodes)
FIX(strcodes)

/* restore the public definition */

#define FREE_FIX(it) if (ptr_##it) { FreeAndNull(ptr_##it); }
#define boolcodes  NCURSES_PUBLIC_VAR(boolcodes())
#define numcodes   NCURSES_PUBLIC_VAR(numcodes())
#define strcodes   NCURSES_PUBLIC_VAR(strcodes())

#if NO_LEAKS
NCURSES_EXPORT(void)
_nc_codes_leaks(void)
{
FREE_FIX(boolcodes)
FREE_FIX(numcodes)
FREE_FIX(strcodes)
}
#endif

#else

#define DCL(it) NCURSES_EXPORT_VAR(IT) it[]

DCL(boolcodes) = {
		"bw",
		"am",
		"xb",
		"xs",
		"xn",
		"eo",
		"gn",
		"hc",
		"km",
		"hs",
		"in",
		"da",
		"db",
		"mi",
		"ms",
		"os",
		"es",
		"xt",
		"hz",
		"ul",
		"xo",
		"nx",
		"5i",
		"HC",
		"NR",
		"NP",
		"ND",
		"cc",
		"ut",
		"hl",
		"YA",
		"YB",
		"YC",
		"YD",
		"YE",
		"YF",
		"YG",
		"bs",
		"ns",
		"nc",
		"MT",
		"NL",
		"pt",
		"xr",

		(NCURSES_CONST char *)0,
};

DCL(numcodes) = {
		"co",
		"it",
		"li",
		"lm",
		"sg",
		"pb",
		"vt",
		"ws",
		"Nl",
		"lh",
		"lw",
		"ma",
		"MW",
		"Co",
		"pa",
		"NC",
		"Ya",
		"Yb",
		"Yc",
		"Yd",
		"Ye",
		"Yf",
		"Yg",
		"Yh",
		"Yi",
		"Yj",
		"Yk",
		"Yl",
		"Ym",
		"Yn",
		"BT",
		"Yo",
		"Yp",
		"ug",
		"dC",
		"dN",
		"dB",
		"dT",
		"kn",

		(NCURSES_CONST char *)0,
};

DCL(strcodes) = {
		"bt",
		"bl",
		"cr",
		"cs",
		"ct",
		"cl",
		"ce",
		"cd",
		"ch",
		"CC",
		"cm",
		"do",
		"ho",
		"vi",
		"le",
		"CM",
		"ve",
		"nd",
		"ll",
		"up",
		"vs",
		"dc",
		"dl",
		"ds",
		"hd",
		"as",
		"mb",
		"md",
		"ti",
		"dm",
		"mh",
		"im",
		"mk",
		"mp",
		"mr",
		"so",
		"us",
		"ec",
		"ae",
		"me",
		"te",
		"ed",
		"ei",
		"se",
		"ue",
		"vb",
		"ff",
		"fs",
		"i1",
		"is",
		"i3",
		"if",
		"ic",
		"al",
		"ip",
		"kb",
		"ka",
		"kC",
		"kt",
		"kD",
		"kL",
		"kd",
		"kM",
		"kE",
		"kS",
		"k0",
		"k1",
		"k;",
		"k2",
		"k3",
		"k4",
		"k5",
		"k6",
		"k7",
		"k8",
		"k9",
		"kh",
		"kI",
		"kA",
		"kl",
		"kH",
		"kN",
		"kP",
		"kr",
		"kF",
		"kR",
		"kT",
		"ku",
		"ke",
		"ks",
		"l0",
		"l1",
		"la",
		"l2",
		"l3",
		"l4",
		"l5",
		"l6",
		"l7",
		"l8",
		"l9",
		"mo",
		"mm",
		"nw",
		"pc",
		"DC",
		"DL",
		"DO",
		"IC",
		"SF",
		"AL",
		"LE",
		"RI",
		"SR",
		"UP",
		"pk",
		"pl",
		"px",
		"ps",
		"pf",
		"po",
		"rp",
		"r1",
		"r2",
		"r3",
		"rf",
		"rc",
		"cv",
		"sc",
		"sf",
		"sr",
		"sa",
		"st",
		"wi",
		"ta",
		"ts",
		"uc",
		"hu",
		"iP",
		"K1",
		"K3",
		"K2",
		"K4",
		"K5",
		"pO",
		"rP",
		"ac",
		"pn",
		"kB",
		"SX",
		"RX",
		"SA",
		"RA",
		"XN",
		"XF",
		"eA",
		"LO",
		"LF",
		"@1",
		"@2",
		"@3",
		"@4",
		"@5",
		"@6",
		"@7",
		"@8",
		"@9",
		"@0",
		"%1",
		"%2",
		"%3",
		"%4",
		"%5",
		"%6",
		"%7",
		"%8",
		"%9",
		"%0",
		"&1",
		"&2",
		"&3",
		"&4",
		"&5",
		"&6",
		"&7",
		"&8",
		"&9",
		"&0",
		"*1",
		"*2",
		"*3",
		"*4",
		"*5",
		"*6",
		"*7",
		"*8",
		"*9",
		"*0",
		"#1",
		"#2",
		"#3",
		"#4",
		"%a",
		"%b",
		"%c",
		"%d",
		"%e",
		"%f",
		"%g",
		"%h",
		"%i",
		"%j",
		"!1",
		"!2",
		"!3",
		"RF",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"FA",
		"FB",
		"FC",
		"FD",
		"FE",
		"FF",
		"FG",
		"FH",
		"FI",
		"FJ",
		"FK",
		"FL",
		"FM",
		"FN",
		"FO",
		"FP",
		"FQ",
		"FR",
		"FS",
		"FT",
		"FU",
		"FV",
		"FW",
		"FX",
		"FY",
		"FZ",
		"Fa",
		"Fb",
		"Fc",
		"Fd",
		"Fe",
		"Ff",
		"Fg",
		"Fh",
		"Fi",
		"Fj",
		"Fk",
		"Fl",
		"Fm",
		"Fn",
		"Fo",
		"Fp",
		"Fq",
		"Fr",
		"cb",
		"MC",
		"ML",
		"MR",
		"Lf",
		"SC",
		"DK",
		"RC",
		"CW",
		"WG",
		"HU",
		"DI",
		"QD",
		"TO",
		"PU",
		"fh",
		"PA",
		"WA",
		"u0",
		"u1",
		"u2",
		"u3",
		"u4",
		"u5",
		"u6",
		"u7",
		"u8",
		"u9",
		"op",
		"oc",
		"Ic",
		"Ip",
		"sp",
		"Sf",
		"Sb",
		"ZA",
		"ZB",
		"ZC",
		"ZD",
		"ZE",
		"ZF",
		"ZG",
		"ZH",
		"ZI",
		"ZJ",
		"ZK",
		"ZL",
		"ZM",
		"ZN",
		"ZO",
		"ZP",
		"ZQ",
		"ZR",
		"ZS",
		"ZT",
		"ZU",
		"ZV",
		"ZW",
		"ZX",
		"ZY",
		"ZZ",
		"Za",
		"Zb",
		"Zc",
		"Zd",
		"Ze",
		"Zf",
		"Zg",
		"Zh",
		"Zi",
		"Zj",
		"Zk",
		"Zl",
		"Zm",
		"Zn",
		"Zo",
		"Zp",
		"Zq",
		"Zr",
		"Zs",
		"Zt",
		"Zu",
		"Zv",
		"Zw",
		"Zx",
		"Zy",
		"Km",
		"Mi",
		"RQ",
		"Gm",
		"AF",
		"AB",
		"xl",
		"dv",
		"ci",
		"s0",
		"s1",
		"s2",
		"s3",
		"ML",
		"MT",
		"Xy",
		"Zz",
		"Yv",
		"Yw",
		"Yx",
		"Yy",
		"Yz",
		"YZ",
		"S1",
		"S2",
		"S3",
		"S4",
		"S5",
		"S6",
		"S7",
		"S8",
		"Xh",
		"Xl",
		"Xo",
		"Xr",
		"Xt",
		"Xv",
		"sA",
		"YI",
		"i2",
		"rs",
		"nl",
		"bc",
		"ko",
		"ma",
		"G2",
		"G3",
		"G1",
		"G4",
		"GR",
		"GL",
		"GU",
		"GD",
		"GH",
		"GV",
		"GC",
		"ml",
		"mu",
		"bx",

		(NCURSES_CONST char *)0,
};


#endif /* BROKEN_LINKER */
