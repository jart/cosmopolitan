#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_TZ

THIRD_PARTY_TZ_ARTIFACTS += THIRD_PARTY_TZ_A
THIRD_PARTY_TZ = $(THIRD_PARTY_TZ_A_DEPS) $(THIRD_PARTY_TZ_A)
THIRD_PARTY_TZ_A = o/$(MODE)/third_party/tz/tz.a
THIRD_PARTY_TZ_A_FILES := $(wildcard third_party/tz/*)
THIRD_PARTY_TZ_A_HDRS = $(filter %.h,$(THIRD_PARTY_TZ_A_FILES))
THIRD_PARTY_TZ_A_SRCS = $(filter %.c,$(THIRD_PARTY_TZ_A_FILES))

LIBC_TIME_ZONEINFOS :=						\
	usr/share/zoneinfo/					\
	usr/share/zoneinfo/CET					\
	usr/share/zoneinfo/CST6CDT				\
	usr/share/zoneinfo/EET					\
	usr/share/zoneinfo/EST					\
	usr/share/zoneinfo/EST5EDT				\
	usr/share/zoneinfo/GMT					\
	usr/share/zoneinfo/HST					\
	usr/share/zoneinfo/MET					\
	usr/share/zoneinfo/MST					\
	usr/share/zoneinfo/MST7MDT				\
	usr/share/zoneinfo/PST8PDT				\
	usr/share/zoneinfo/WET					\
	usr/share/zoneinfo/Antarctica/				\
	usr/share/zoneinfo/Antarctica/Casey			\
	usr/share/zoneinfo/Antarctica/Davis			\
	usr/share/zoneinfo/Antarctica/Macquarie			\
	usr/share/zoneinfo/Antarctica/Mawson			\
	usr/share/zoneinfo/Antarctica/Palmer			\
	usr/share/zoneinfo/Antarctica/Rothera			\
	usr/share/zoneinfo/Antarctica/Troll			\
	usr/share/zoneinfo/Antarctica/Vostok			\
	usr/share/zoneinfo/Europe/				\
	usr/share/zoneinfo/Europe/Andorra			\
	usr/share/zoneinfo/Europe/Astrakhan			\
	usr/share/zoneinfo/Europe/Athens			\
	usr/share/zoneinfo/Europe/Belgrade			\
	usr/share/zoneinfo/Europe/Berlin			\
	usr/share/zoneinfo/Europe/Brussels			\
	usr/share/zoneinfo/Europe/Bucharest			\
	usr/share/zoneinfo/Europe/Budapest			\
	usr/share/zoneinfo/Europe/Chisinau			\
	usr/share/zoneinfo/Europe/Dublin			\
	usr/share/zoneinfo/Europe/Gibraltar			\
	usr/share/zoneinfo/Europe/Helsinki			\
	usr/share/zoneinfo/Europe/Istanbul			\
	usr/share/zoneinfo/Europe/Kaliningrad			\
	usr/share/zoneinfo/Europe/Kirov				\
	usr/share/zoneinfo/Europe/Kyiv				\
	usr/share/zoneinfo/Europe/Lisbon			\
	usr/share/zoneinfo/Europe/London			\
	usr/share/zoneinfo/Europe/Madrid			\
	usr/share/zoneinfo/Europe/Malta				\
	usr/share/zoneinfo/Europe/Minsk				\
	usr/share/zoneinfo/Europe/Moscow			\
	usr/share/zoneinfo/Europe/Paris				\
	usr/share/zoneinfo/Europe/Prague			\
	usr/share/zoneinfo/Europe/Riga				\
	usr/share/zoneinfo/Europe/Rome				\
	usr/share/zoneinfo/Europe/Samara			\
	usr/share/zoneinfo/Europe/Saratov			\
	usr/share/zoneinfo/Europe/Simferopol			\
	usr/share/zoneinfo/Europe/Sofia				\
	usr/share/zoneinfo/Europe/Tallinn			\
	usr/share/zoneinfo/Europe/Tirane			\
	usr/share/zoneinfo/Europe/Ulyanovsk			\
	usr/share/zoneinfo/Europe/Vienna			\
	usr/share/zoneinfo/Europe/Vilnius			\
	usr/share/zoneinfo/Europe/Volgograd			\
	usr/share/zoneinfo/Europe/Warsaw			\
	usr/share/zoneinfo/Europe/Zurich			\
	usr/share/zoneinfo/Asia/				\
	usr/share/zoneinfo/Asia/Almaty				\
	usr/share/zoneinfo/Asia/Amman				\
	usr/share/zoneinfo/Asia/Anadyr				\
	usr/share/zoneinfo/Asia/Aqtau				\
	usr/share/zoneinfo/Asia/Aqtobe				\
	usr/share/zoneinfo/Asia/Ashgabat			\
	usr/share/zoneinfo/Asia/Atyrau				\
	usr/share/zoneinfo/Asia/Baghdad				\
	usr/share/zoneinfo/Asia/Baku				\
	usr/share/zoneinfo/Asia/Bangkok				\
	usr/share/zoneinfo/Asia/Barnaul				\
	usr/share/zoneinfo/Asia/Beirut				\
	usr/share/zoneinfo/Asia/Bishkek				\
	usr/share/zoneinfo/Asia/Chita				\
	usr/share/zoneinfo/Asia/Choibalsan			\
	usr/share/zoneinfo/Asia/Colombo				\
	usr/share/zoneinfo/Asia/Damascus			\
	usr/share/zoneinfo/Asia/Dhaka				\
	usr/share/zoneinfo/Asia/Dili				\
	usr/share/zoneinfo/Asia/Dubai				\
	usr/share/zoneinfo/Asia/Dushanbe			\
	usr/share/zoneinfo/Asia/Famagusta			\
	usr/share/zoneinfo/Asia/Gaza				\
	usr/share/zoneinfo/Asia/Hebron				\
	usr/share/zoneinfo/Asia/Ho_Chi_Minh			\
	usr/share/zoneinfo/Asia/Hong_Kong			\
	usr/share/zoneinfo/Asia/Hovd				\
	usr/share/zoneinfo/Asia/Irkutsk				\
	usr/share/zoneinfo/Asia/Jakarta				\
	usr/share/zoneinfo/Asia/Jayapura			\
	usr/share/zoneinfo/Asia/Jerusalem			\
	usr/share/zoneinfo/Asia/Kabul				\
	usr/share/zoneinfo/Asia/Kamchatka			\
	usr/share/zoneinfo/Asia/Karachi				\
	usr/share/zoneinfo/Asia/Kathmandu			\
	usr/share/zoneinfo/Asia/Khandyga			\
	usr/share/zoneinfo/Asia/Kolkata				\
	usr/share/zoneinfo/Asia/Krasnoyarsk			\
	usr/share/zoneinfo/Asia/Kuching				\
	usr/share/zoneinfo/Asia/Macau				\
	usr/share/zoneinfo/Asia/Magadan				\
	usr/share/zoneinfo/Asia/Makassar			\
	usr/share/zoneinfo/Asia/Manila				\
	usr/share/zoneinfo/Asia/Nicosia				\
	usr/share/zoneinfo/Asia/Novokuznetsk			\
	usr/share/zoneinfo/Asia/Novosibirsk			\
	usr/share/zoneinfo/Asia/Omsk				\
	usr/share/zoneinfo/Asia/Oral				\
	usr/share/zoneinfo/Asia/Pontianak			\
	usr/share/zoneinfo/Asia/Pyongyang			\
	usr/share/zoneinfo/Asia/Qatar				\
	usr/share/zoneinfo/Asia/Qostanay			\
	usr/share/zoneinfo/Asia/Qyzylorda			\
	usr/share/zoneinfo/Asia/Riyadh				\
	usr/share/zoneinfo/Asia/Sakhalin			\
	usr/share/zoneinfo/Asia/Samarkand			\
	usr/share/zoneinfo/Asia/Seoul				\
	usr/share/zoneinfo/Asia/Shanghai			\
	usr/share/zoneinfo/Asia/Singapore			\
	usr/share/zoneinfo/Asia/Srednekolymsk			\
	usr/share/zoneinfo/Asia/Taipei				\
	usr/share/zoneinfo/Asia/Tashkent			\
	usr/share/zoneinfo/Asia/Tbilisi				\
	usr/share/zoneinfo/Asia/Tehran				\
	usr/share/zoneinfo/Asia/Thimphu				\
	usr/share/zoneinfo/Asia/Tokyo				\
	usr/share/zoneinfo/Asia/Tomsk				\
	usr/share/zoneinfo/Asia/Ulaanbaatar			\
	usr/share/zoneinfo/Asia/Urumqi				\
	usr/share/zoneinfo/Asia/Ust-Nera			\
	usr/share/zoneinfo/Asia/Vladivostok			\
	usr/share/zoneinfo/Asia/Yakutsk				\
	usr/share/zoneinfo/Asia/Yangon				\
	usr/share/zoneinfo/Asia/Yekaterinburg			\
	usr/share/zoneinfo/Asia/Yerevan				\
	usr/share/zoneinfo/Pacific/				\
	usr/share/zoneinfo/Pacific/Apia				\
	usr/share/zoneinfo/Pacific/Auckland			\
	usr/share/zoneinfo/Pacific/Bougainville			\
	usr/share/zoneinfo/Pacific/Chatham			\
	usr/share/zoneinfo/Pacific/Easter			\
	usr/share/zoneinfo/Pacific/Efate			\
	usr/share/zoneinfo/Pacific/Fakaofo			\
	usr/share/zoneinfo/Pacific/Fiji				\
	usr/share/zoneinfo/Pacific/Galapagos			\
	usr/share/zoneinfo/Pacific/Gambier			\
	usr/share/zoneinfo/Pacific/Guadalcanal			\
	usr/share/zoneinfo/Pacific/Guam				\
	usr/share/zoneinfo/Pacific/Honolulu			\
	usr/share/zoneinfo/Pacific/Kanton			\
	usr/share/zoneinfo/Pacific/Kiritimati			\
	usr/share/zoneinfo/Pacific/Kosrae			\
	usr/share/zoneinfo/Pacific/Kwajalein			\
	usr/share/zoneinfo/Pacific/Marquesas			\
	usr/share/zoneinfo/Pacific/Nauru			\
	usr/share/zoneinfo/Pacific/Niue				\
	usr/share/zoneinfo/Pacific/Norfolk			\
	usr/share/zoneinfo/Pacific/Noumea			\
	usr/share/zoneinfo/Pacific/Pago_Pago			\
	usr/share/zoneinfo/Pacific/Palau			\
	usr/share/zoneinfo/Pacific/Pitcairn			\
	usr/share/zoneinfo/Pacific/Port_Moresby			\
	usr/share/zoneinfo/Pacific/Rarotonga			\
	usr/share/zoneinfo/Pacific/Tahiti			\
	usr/share/zoneinfo/Pacific/Tarawa			\
	usr/share/zoneinfo/Pacific/Tongatapu			\
	usr/share/zoneinfo/America/				\
	usr/share/zoneinfo/America/Adak				\
	usr/share/zoneinfo/America/Anchorage			\
	usr/share/zoneinfo/America/Araguaina			\
	usr/share/zoneinfo/America/Argentina/			\
	usr/share/zoneinfo/America/Argentina/Buenos_Aires	\
	usr/share/zoneinfo/America/Argentina/Catamarca		\
	usr/share/zoneinfo/America/Argentina/Cordoba		\
	usr/share/zoneinfo/America/Argentina/Jujuy		\
	usr/share/zoneinfo/America/Argentina/La_Rioja		\
	usr/share/zoneinfo/America/Argentina/Mendoza		\
	usr/share/zoneinfo/America/Argentina/Rio_Gallegos	\
	usr/share/zoneinfo/America/Argentina/Salta		\
	usr/share/zoneinfo/America/Argentina/San_Juan		\
	usr/share/zoneinfo/America/Argentina/San_Luis		\
	usr/share/zoneinfo/America/Argentina/Tucuman		\
	usr/share/zoneinfo/America/Argentina/Ushuaia		\
	usr/share/zoneinfo/America/Asuncion			\
	usr/share/zoneinfo/America/Bahia			\
	usr/share/zoneinfo/America/Bahia_Banderas		\
	usr/share/zoneinfo/America/Barbados			\
	usr/share/zoneinfo/America/Belem			\
	usr/share/zoneinfo/America/Belize			\
	usr/share/zoneinfo/America/Boa_Vista			\
	usr/share/zoneinfo/America/Bogota			\
	usr/share/zoneinfo/America/Boise			\
	usr/share/zoneinfo/America/Cambridge_Bay		\
	usr/share/zoneinfo/America/Campo_Grande			\
	usr/share/zoneinfo/America/Cancun			\
	usr/share/zoneinfo/America/Caracas			\
	usr/share/zoneinfo/America/Cayenne			\
	usr/share/zoneinfo/America/Chicago			\
	usr/share/zoneinfo/America/Chihuahua			\
	usr/share/zoneinfo/America/Ciudad_Juarez		\
	usr/share/zoneinfo/America/Costa_Rica			\
	usr/share/zoneinfo/America/Cuiaba			\
	usr/share/zoneinfo/America/Danmarkshavn			\
	usr/share/zoneinfo/America/Dawson			\
	usr/share/zoneinfo/America/Dawson_Creek			\
	usr/share/zoneinfo/America/Denver			\
	usr/share/zoneinfo/America/Detroit			\
	usr/share/zoneinfo/America/Edmonton			\
	usr/share/zoneinfo/America/Eirunepe			\
	usr/share/zoneinfo/America/El_Salvador			\
	usr/share/zoneinfo/America/Fort_Nelson			\
	usr/share/zoneinfo/America/Fortaleza			\
	usr/share/zoneinfo/America/Glace_Bay			\
	usr/share/zoneinfo/America/Goose_Bay			\
	usr/share/zoneinfo/America/Grand_Turk			\
	usr/share/zoneinfo/America/Guatemala			\
	usr/share/zoneinfo/America/Guayaquil			\
	usr/share/zoneinfo/America/Guyana			\
	usr/share/zoneinfo/America/Halifax			\
	usr/share/zoneinfo/America/Havana			\
	usr/share/zoneinfo/America/Hermosillo			\
	usr/share/zoneinfo/America/Indiana/			\
	usr/share/zoneinfo/America/Indiana/Indianapolis		\
	usr/share/zoneinfo/America/Indiana/Knox			\
	usr/share/zoneinfo/America/Indiana/Marengo		\
	usr/share/zoneinfo/America/Indiana/Petersburg		\
	usr/share/zoneinfo/America/Indiana/Tell_City		\
	usr/share/zoneinfo/America/Indiana/Vevay		\
	usr/share/zoneinfo/America/Indiana/Vincennes		\
	usr/share/zoneinfo/America/Indiana/Winamac		\
	usr/share/zoneinfo/America/Inuvik			\
	usr/share/zoneinfo/America/Iqaluit			\
	usr/share/zoneinfo/America/Jamaica			\
	usr/share/zoneinfo/America/Juneau			\
	usr/share/zoneinfo/America/Kentucky/			\
	usr/share/zoneinfo/America/Kentucky/Louisville		\
	usr/share/zoneinfo/America/Kentucky/Monticello		\
	usr/share/zoneinfo/America/La_Paz			\
	usr/share/zoneinfo/America/Lima				\
	usr/share/zoneinfo/America/Los_Angeles			\
	usr/share/zoneinfo/America/Maceio			\
	usr/share/zoneinfo/America/Managua			\
	usr/share/zoneinfo/America/Manaus			\
	usr/share/zoneinfo/America/Martinique			\
	usr/share/zoneinfo/America/Matamoros			\
	usr/share/zoneinfo/America/Mazatlan			\
	usr/share/zoneinfo/America/Menominee			\
	usr/share/zoneinfo/America/Merida			\
	usr/share/zoneinfo/America/Metlakatla			\
	usr/share/zoneinfo/America/Mexico_City			\
	usr/share/zoneinfo/America/Miquelon			\
	usr/share/zoneinfo/America/Moncton			\
	usr/share/zoneinfo/America/Monterrey			\
	usr/share/zoneinfo/America/Montevideo			\
	usr/share/zoneinfo/America/New_York			\
	usr/share/zoneinfo/America/Nome				\
	usr/share/zoneinfo/America/Noronha			\
	usr/share/zoneinfo/America/North_Dakota/		\
	usr/share/zoneinfo/America/North_Dakota/Beulah		\
	usr/share/zoneinfo/America/North_Dakota/Center		\
	usr/share/zoneinfo/America/North_Dakota/New_Salem	\
	usr/share/zoneinfo/America/Nuuk				\
	usr/share/zoneinfo/America/Ojinaga			\
	usr/share/zoneinfo/America/Panama			\
	usr/share/zoneinfo/America/Paramaribo			\
	usr/share/zoneinfo/America/Phoenix			\
	usr/share/zoneinfo/America/Port-au-Prince		\
	usr/share/zoneinfo/America/Porto_Velho			\
	usr/share/zoneinfo/America/Puerto_Rico			\
	usr/share/zoneinfo/America/Punta_Arenas			\
	usr/share/zoneinfo/America/Rankin_Inlet			\
	usr/share/zoneinfo/America/Recife			\
	usr/share/zoneinfo/America/Regina			\
	usr/share/zoneinfo/America/Resolute			\
	usr/share/zoneinfo/America/Rio_Branco			\
	usr/share/zoneinfo/America/Santarem			\
	usr/share/zoneinfo/America/Santiago			\
	usr/share/zoneinfo/America/Santo_Domingo		\
	usr/share/zoneinfo/America/Sao_Paulo			\
	usr/share/zoneinfo/America/Scoresbysund			\
	usr/share/zoneinfo/America/Sitka			\
	usr/share/zoneinfo/America/St_Johns			\
	usr/share/zoneinfo/America/Swift_Current		\
	usr/share/zoneinfo/America/Tegucigalpa			\
	usr/share/zoneinfo/America/Thule			\
	usr/share/zoneinfo/America/Tijuana			\
	usr/share/zoneinfo/America/Toronto			\
	usr/share/zoneinfo/America/Vancouver			\
	usr/share/zoneinfo/America/Whitehorse			\
	usr/share/zoneinfo/America/Winnipeg			\
	usr/share/zoneinfo/America/Yakutat			\
	usr/share/zoneinfo/Africa/				\
	usr/share/zoneinfo/Africa/Abidjan			\
	usr/share/zoneinfo/Africa/Algiers			\
	usr/share/zoneinfo/Africa/Bissau			\
	usr/share/zoneinfo/Africa/Cairo				\
	usr/share/zoneinfo/Africa/Casablanca			\
	usr/share/zoneinfo/Africa/Ceuta				\
	usr/share/zoneinfo/Africa/El_Aaiun			\
	usr/share/zoneinfo/Africa/Johannesburg			\
	usr/share/zoneinfo/Africa/Juba				\
	usr/share/zoneinfo/Africa/Khartoum			\
	usr/share/zoneinfo/Africa/Lagos				\
	usr/share/zoneinfo/Africa/Maputo			\
	usr/share/zoneinfo/Africa/Monrovia			\
	usr/share/zoneinfo/Africa/Nairobi			\
	usr/share/zoneinfo/Africa/Ndjamena			\
	usr/share/zoneinfo/Africa/Sao_Tome			\
	usr/share/zoneinfo/Africa/Tripoli			\
	usr/share/zoneinfo/Africa/Tunis				\
	usr/share/zoneinfo/Africa/Windhoek			\
	usr/share/zoneinfo/Australia/				\
	usr/share/zoneinfo/Australia/Adelaide			\
	usr/share/zoneinfo/Australia/Brisbane			\
	usr/share/zoneinfo/Australia/Broken_Hill		\
	usr/share/zoneinfo/Australia/Darwin			\
	usr/share/zoneinfo/Australia/Eucla			\
	usr/share/zoneinfo/Australia/Hobart			\
	usr/share/zoneinfo/Australia/Lindeman			\
	usr/share/zoneinfo/Australia/Lord_Howe			\
	usr/share/zoneinfo/Australia/Melbourne			\
	usr/share/zoneinfo/Australia/Perth			\
	usr/share/zoneinfo/Australia/Sydney			\
	usr/share/zoneinfo/Atlantic/				\
	usr/share/zoneinfo/Atlantic/Azores			\
	usr/share/zoneinfo/Atlantic/Bermuda			\
	usr/share/zoneinfo/Atlantic/Canary			\
	usr/share/zoneinfo/Atlantic/Cape_Verde			\
	usr/share/zoneinfo/Atlantic/Faroe			\
	usr/share/zoneinfo/Atlantic/Madeira			\
	usr/share/zoneinfo/Atlantic/South_Georgia		\
	usr/share/zoneinfo/Atlantic/Stanley			\
	usr/share/zoneinfo/Etc/					\
	usr/share/zoneinfo/Etc/UTC				\
	usr/share/zoneinfo/Etc/GMT-14				\
	usr/share/zoneinfo/Etc/GMT-13				\
	usr/share/zoneinfo/Etc/GMT-12				\
	usr/share/zoneinfo/Etc/GMT-11				\
	usr/share/zoneinfo/Etc/GMT-10				\
	usr/share/zoneinfo/Etc/GMT-9				\
	usr/share/zoneinfo/Etc/GMT-8				\
	usr/share/zoneinfo/Etc/GMT-7				\
	usr/share/zoneinfo/Etc/GMT-6				\
	usr/share/zoneinfo/Etc/GMT-5				\
	usr/share/zoneinfo/Etc/GMT-4				\
	usr/share/zoneinfo/Etc/GMT-3				\
	usr/share/zoneinfo/Etc/GMT-2				\
	usr/share/zoneinfo/Etc/GMT-1				\
	usr/share/zoneinfo/Etc/GMT				\
	usr/share/zoneinfo/Etc/GMT+1				\
	usr/share/zoneinfo/Etc/GMT+2				\
	usr/share/zoneinfo/Etc/GMT+3				\
	usr/share/zoneinfo/Etc/GMT+4				\
	usr/share/zoneinfo/Etc/GMT+5				\
	usr/share/zoneinfo/Etc/GMT+6				\
	usr/share/zoneinfo/Etc/GMT+7				\
	usr/share/zoneinfo/Etc/GMT+8				\
	usr/share/zoneinfo/Etc/GMT+9				\
	usr/share/zoneinfo/Etc/GMT+10				\
	usr/share/zoneinfo/Etc/GMT+11				\
	usr/share/zoneinfo/Etc/GMT+12				\
	usr/share/zoneinfo/Indian/				\
	usr/share/zoneinfo/Indian/Chagos			\
	usr/share/zoneinfo/Indian/Maldives			\
	usr/share/zoneinfo/Indian/Mauritius

THIRD_PARTY_TZ_A_OBJS =						\
	$(THIRD_PARTY_TZ_A_SRCS:%.c=o/$(MODE)/%.o)		\
	$(LIBC_TIME_ZONEINFOS:%=o/$(MODE)/%.zip.o)

THIRD_PARTY_TZ_A_CHECKS =					\
	$(THIRD_PARTY_TZ_A).pkg					\
	$(THIRD_PARTY_TZ_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_TZ_A_DIRECTDEPS =					\
	LIBC_INTRIN						\
	LIBC_CALLS						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_KERNEL32					\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV

THIRD_PARTY_TZ_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_TZ_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_TZ_A):						\
		third_party/tz/					\
		$(THIRD_PARTY_TZ_A).pkg				\
		$(THIRD_PARTY_TZ_A_OBJS)

$(THIRD_PARTY_TZ_A).pkg:					\
		$(THIRD_PARTY_TZ_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_TZ_A_DIRECTDEPS),$($(x)_A).pkg)

# make this library tinier
o/$(MODE)/third_party/tz/strftime.o: private CFLAGS += -fno-jump-tables
o/$(MODE)/third_party/tz/localtime.o: private CFLAGS += -fdata-sections -ffunction-sections

# offer same stack safety assurances as cosmo libc
$(THIRD_PARTY_TZ_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

o/$(MODE)/usr/share/zoneinfo/.zip.o: usr/share/zoneinfo

THIRD_PARTY_TZ_LIBS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)))
THIRD_PARTY_TZ_SRCS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_TZ_HDRS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_TZ_BINS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_TZ_CHECKS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_TZ_OBJS = $(foreach x,$(THIRD_PARTY_TZ_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_TZ_OBJS): $(BUILD_FILES) third_party/tz/BUILD.mk

.PHONY: o/$(MODE)/third_party/tz
o/$(MODE)/third_party/tz:					\
		$(THIRD_PARTY_TZ_A)				\
		$(THIRD_PARTY_TZ_CHECKS)
