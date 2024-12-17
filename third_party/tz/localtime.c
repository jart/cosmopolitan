/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#define LOCALTIME_IMPLEMENTATION
#include "lock.h"
#include "tzdir.h"
#include "tzfile.h"
#include "private.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/cxxabi.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/time.h"
#include "libc/inttypes.h"
#include "libc/sysv/consts/ok.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/time.h"
#include "libc/nt/struct/timezoneinformation.h"
#include "libc/nt/time.h"
#include "libc/dce.h"

/* Convert timestamp from time_t to struct tm.  */

/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson.
*/

/*
** Leap second handling from Bradley White.
** POSIX.1-1988 style TZ environment variable handling from Guy Harris.
*/

/*LINTLIBRARY*/

__static_yoink("zipos");

__static_yoink("usr/share/zoneinfo/");
__static_yoink("usr/share/zoneinfo/GMT"); // Greenwich Mean Time (UTC +0)
__static_yoink("usr/share/zoneinfo/EST5EDT"); // Eastern Standard Time (UTC -5) and Eastern Daylight Time (UTC -4)
__static_yoink("usr/share/zoneinfo/CST6CDT"); // Central Standard Time (UTC -6) and Central Daylight Time (UTC -5)
__static_yoink("usr/share/zoneinfo/MST7MDT"); // Mountain Standard Time (UTC -7) and Mountain Daylight Time (UTC -6)
__static_yoink("usr/share/zoneinfo/PST8PDT"); // Pacific Standard Time (UTC -8) and Pacific Daylight Time (UTC -7)
__static_yoink("usr/share/zoneinfo/EST"); // Eastern Standard Time (UTC -5)
__static_yoink("usr/share/zoneinfo/HST"); // Hawaii Standard Time (UTC -10)
__static_yoink("usr/share/zoneinfo/MST"); // Mountain Standard Time (UTC -7)
__static_yoink("usr/share/zoneinfo/EET"); // Eastern European Time (UTC +2)
__static_yoink("usr/share/zoneinfo/MET"); // Middle European Time (UTC +1), also known as Central European Time
__static_yoink("usr/share/zoneinfo/CET"); // Central European Time (UTC +1)
__static_yoink("usr/share/zoneinfo/WET"); // Western European Time (UTC +0)

__static_yoink("usr/share/zoneinfo/Etc/");
__static_yoink("usr/share/zoneinfo/Etc/UTC");
__static_yoink("usr/share/zoneinfo/Etc/GMT-14");
__static_yoink("usr/share/zoneinfo/Etc/GMT-13");
__static_yoink("usr/share/zoneinfo/Etc/GMT-12");
__static_yoink("usr/share/zoneinfo/Etc/GMT-11");
__static_yoink("usr/share/zoneinfo/Etc/GMT-10");
__static_yoink("usr/share/zoneinfo/Etc/GMT-9");
__static_yoink("usr/share/zoneinfo/Etc/GMT-8");
__static_yoink("usr/share/zoneinfo/Etc/GMT-7");
__static_yoink("usr/share/zoneinfo/Etc/GMT-6");
__static_yoink("usr/share/zoneinfo/Etc/GMT-5");
__static_yoink("usr/share/zoneinfo/Etc/GMT-4");
__static_yoink("usr/share/zoneinfo/Etc/GMT-3");
__static_yoink("usr/share/zoneinfo/Etc/GMT-2");
__static_yoink("usr/share/zoneinfo/Etc/GMT-1");
__static_yoink("usr/share/zoneinfo/Etc/GMT");
__static_yoink("usr/share/zoneinfo/Etc/GMT+1");
__static_yoink("usr/share/zoneinfo/Etc/GMT+2");
__static_yoink("usr/share/zoneinfo/Etc/GMT+3");
__static_yoink("usr/share/zoneinfo/Etc/GMT+4");
__static_yoink("usr/share/zoneinfo/Etc/GMT+5");
__static_yoink("usr/share/zoneinfo/Etc/GMT+6");
__static_yoink("usr/share/zoneinfo/Etc/GMT+7");
__static_yoink("usr/share/zoneinfo/Etc/GMT+8");
__static_yoink("usr/share/zoneinfo/Etc/GMT+9");
__static_yoink("usr/share/zoneinfo/Etc/GMT+10");
__static_yoink("usr/share/zoneinfo/Etc/GMT+11");
__static_yoink("usr/share/zoneinfo/Etc/GMT+12");

__static_yoink("usr/share/zoneinfo/America/");
__static_yoink("usr/share/zoneinfo/America/Anchorage"); // U.S.
__static_yoink("usr/share/zoneinfo/America/Chicago"); // U.S.
__static_yoink("usr/share/zoneinfo/America/Denver"); // U.S.
__static_yoink("usr/share/zoneinfo/America/Los_Angeles"); // U.S.
__static_yoink("usr/share/zoneinfo/America/New_York"); // U.S.
__static_yoink("usr/share/zoneinfo/America/Phoenix"); // U.S.
__static_yoink("usr/share/zoneinfo/America/Argentina/");
__static_yoink("usr/share/zoneinfo/America/Argentina/Buenos_Aires");
__static_yoink("usr/share/zoneinfo/America/Bogota"); // Columbia
__static_yoink("usr/share/zoneinfo/America/Halifax"); // Canada
__static_yoink("usr/share/zoneinfo/America/St_Johns"); // Canada
__static_yoink("usr/share/zoneinfo/America/Lima"); // Peru
__static_yoink("usr/share/zoneinfo/America/Mexico_City"); // Mexico
__static_yoink("usr/share/zoneinfo/America/Santiago"); // Chile
__static_yoink("usr/share/zoneinfo/America/Sao_Paulo"); // Brazil
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/America/Adak");
__static_yoink("usr/share/zoneinfo/America/Araguaina");
__static_yoink("usr/share/zoneinfo/America/Argentina/Catamarca");
__static_yoink("usr/share/zoneinfo/America/Argentina/Cordoba");
__static_yoink("usr/share/zoneinfo/America/Argentina/Jujuy");
__static_yoink("usr/share/zoneinfo/America/Argentina/La_Rioja");
__static_yoink("usr/share/zoneinfo/America/Argentina/Mendoza");
__static_yoink("usr/share/zoneinfo/America/Argentina/Rio_Gallegos");
__static_yoink("usr/share/zoneinfo/America/Argentina/Salta");
__static_yoink("usr/share/zoneinfo/America/Argentina/San_Juan");
__static_yoink("usr/share/zoneinfo/America/Argentina/San_Luis");
__static_yoink("usr/share/zoneinfo/America/Argentina/Tucuman");
__static_yoink("usr/share/zoneinfo/America/Argentina/Ushuaia");
__static_yoink("usr/share/zoneinfo/America/Asuncion");
__static_yoink("usr/share/zoneinfo/America/Bahia");
__static_yoink("usr/share/zoneinfo/America/Bahia_Banderas");
__static_yoink("usr/share/zoneinfo/America/Barbados");
__static_yoink("usr/share/zoneinfo/America/Belem");
__static_yoink("usr/share/zoneinfo/America/Belize");
__static_yoink("usr/share/zoneinfo/America/Boa_Vista");
__static_yoink("usr/share/zoneinfo/America/Boise");
__static_yoink("usr/share/zoneinfo/America/Cambridge_Bay");
__static_yoink("usr/share/zoneinfo/America/Campo_Grande");
__static_yoink("usr/share/zoneinfo/America/Cancun");
__static_yoink("usr/share/zoneinfo/America/Caracas");
__static_yoink("usr/share/zoneinfo/America/Cayenne");
__static_yoink("usr/share/zoneinfo/America/Chihuahua");
__static_yoink("usr/share/zoneinfo/America/Ciudad_Juarez");
__static_yoink("usr/share/zoneinfo/America/Costa_Rica");
__static_yoink("usr/share/zoneinfo/America/Cuiaba");
__static_yoink("usr/share/zoneinfo/America/Danmarkshavn");
__static_yoink("usr/share/zoneinfo/America/Dawson");
__static_yoink("usr/share/zoneinfo/America/Dawson_Creek");
__static_yoink("usr/share/zoneinfo/America/Detroit");
__static_yoink("usr/share/zoneinfo/America/Edmonton");
__static_yoink("usr/share/zoneinfo/America/Eirunepe");
__static_yoink("usr/share/zoneinfo/America/El_Salvador");
__static_yoink("usr/share/zoneinfo/America/Fort_Nelson");
__static_yoink("usr/share/zoneinfo/America/Fortaleza");
__static_yoink("usr/share/zoneinfo/America/Glace_Bay");
__static_yoink("usr/share/zoneinfo/America/Goose_Bay");
__static_yoink("usr/share/zoneinfo/America/Grand_Turk");
__static_yoink("usr/share/zoneinfo/America/Guatemala");
__static_yoink("usr/share/zoneinfo/America/Guayaquil");
__static_yoink("usr/share/zoneinfo/America/Guyana");
__static_yoink("usr/share/zoneinfo/America/Havana");
__static_yoink("usr/share/zoneinfo/America/Hermosillo");
__static_yoink("usr/share/zoneinfo/America/Indiana/");
__static_yoink("usr/share/zoneinfo/America/Indiana/Indianapolis");
__static_yoink("usr/share/zoneinfo/America/Indiana/Knox");
__static_yoink("usr/share/zoneinfo/America/Indiana/Marengo");
__static_yoink("usr/share/zoneinfo/America/Indiana/Petersburg");
__static_yoink("usr/share/zoneinfo/America/Indiana/Tell_City");
__static_yoink("usr/share/zoneinfo/America/Indiana/Vevay");
__static_yoink("usr/share/zoneinfo/America/Indiana/Vincennes");
__static_yoink("usr/share/zoneinfo/America/Indiana/Winamac");
__static_yoink("usr/share/zoneinfo/America/Inuvik");
__static_yoink("usr/share/zoneinfo/America/Iqaluit");
__static_yoink("usr/share/zoneinfo/America/Jamaica");
__static_yoink("usr/share/zoneinfo/America/Juneau");
__static_yoink("usr/share/zoneinfo/America/Kentucky/");
__static_yoink("usr/share/zoneinfo/America/Kentucky/Louisville");
__static_yoink("usr/share/zoneinfo/America/Kentucky/Monticello");
__static_yoink("usr/share/zoneinfo/America/La_Paz");
__static_yoink("usr/share/zoneinfo/America/Maceio");
__static_yoink("usr/share/zoneinfo/America/Managua");
__static_yoink("usr/share/zoneinfo/America/Manaus");
__static_yoink("usr/share/zoneinfo/America/Martinique");
__static_yoink("usr/share/zoneinfo/America/Matamoros");
__static_yoink("usr/share/zoneinfo/America/Mazatlan");
__static_yoink("usr/share/zoneinfo/America/Menominee");
__static_yoink("usr/share/zoneinfo/America/Merida");
__static_yoink("usr/share/zoneinfo/America/Metlakatla");
__static_yoink("usr/share/zoneinfo/America/Miquelon");
__static_yoink("usr/share/zoneinfo/America/Moncton");
__static_yoink("usr/share/zoneinfo/America/Monterrey");
__static_yoink("usr/share/zoneinfo/America/Montevideo");
__static_yoink("usr/share/zoneinfo/America/Nome");
__static_yoink("usr/share/zoneinfo/America/Noronha");
__static_yoink("usr/share/zoneinfo/America/North_Dakota/");
__static_yoink("usr/share/zoneinfo/America/North_Dakota/Beulah");
__static_yoink("usr/share/zoneinfo/America/North_Dakota/Center");
__static_yoink("usr/share/zoneinfo/America/North_Dakota/New_Salem");
__static_yoink("usr/share/zoneinfo/America/Nuuk");
__static_yoink("usr/share/zoneinfo/America/Ojinaga");
__static_yoink("usr/share/zoneinfo/America/Panama");
__static_yoink("usr/share/zoneinfo/America/Paramaribo");
__static_yoink("usr/share/zoneinfo/America/Port-au-Prince");
__static_yoink("usr/share/zoneinfo/America/Porto_Velho");
__static_yoink("usr/share/zoneinfo/America/Puerto_Rico");
__static_yoink("usr/share/zoneinfo/America/Punta_Arenas");
__static_yoink("usr/share/zoneinfo/America/Rankin_Inlet");
__static_yoink("usr/share/zoneinfo/America/Recife");
__static_yoink("usr/share/zoneinfo/America/Regina");
__static_yoink("usr/share/zoneinfo/America/Resolute");
__static_yoink("usr/share/zoneinfo/America/Rio_Branco");
__static_yoink("usr/share/zoneinfo/America/Santarem");
__static_yoink("usr/share/zoneinfo/America/Santo_Domingo");
__static_yoink("usr/share/zoneinfo/America/Scoresbysund");
__static_yoink("usr/share/zoneinfo/America/Sitka");
__static_yoink("usr/share/zoneinfo/America/Swift_Current");
__static_yoink("usr/share/zoneinfo/America/Tegucigalpa");
__static_yoink("usr/share/zoneinfo/America/Thule");
__static_yoink("usr/share/zoneinfo/America/Tijuana");
__static_yoink("usr/share/zoneinfo/America/Toronto");
__static_yoink("usr/share/zoneinfo/America/Vancouver");
__static_yoink("usr/share/zoneinfo/America/Whitehorse");
__static_yoink("usr/share/zoneinfo/America/Winnipeg");
__static_yoink("usr/share/zoneinfo/America/Yakuatt");
#endif

__static_yoink("usr/share/zoneinfo/Europe/");
__static_yoink("usr/share/zoneinfo/Europe/Brussels"); // Belgium
__static_yoink("usr/share/zoneinfo/Europe/Bucharest"); // Romania
__static_yoink("usr/share/zoneinfo/Europe/Dublin"); // Ireland
__static_yoink("usr/share/zoneinfo/Europe/Istanbul"); // Turkey
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Europe/Andorra");
__static_yoink("usr/share/zoneinfo/Europe/Astrakhan");
__static_yoink("usr/share/zoneinfo/Europe/Athens");
__static_yoink("usr/share/zoneinfo/Europe/Belgrade");
__static_yoink("usr/share/zoneinfo/Europe/Berlin");
__static_yoink("usr/share/zoneinfo/Europe/Budapest");
__static_yoink("usr/share/zoneinfo/Europe/Chisinau");
__static_yoink("usr/share/zoneinfo/Europe/Gibraltar");
__static_yoink("usr/share/zoneinfo/Europe/Helsinki");
__static_yoink("usr/share/zoneinfo/Europe/Kaliningrad");
__static_yoink("usr/share/zoneinfo/Europe/Kirov");
__static_yoink("usr/share/zoneinfo/Europe/Kyiv");
__static_yoink("usr/share/zoneinfo/Europe/Lisbon");
__static_yoink("usr/share/zoneinfo/Europe/London");
__static_yoink("usr/share/zoneinfo/Europe/Madrid");
__static_yoink("usr/share/zoneinfo/Europe/Malta");
__static_yoink("usr/share/zoneinfo/Europe/Minsk");
__static_yoink("usr/share/zoneinfo/Europe/Moscow");
__static_yoink("usr/share/zoneinfo/Europe/Paris");
__static_yoink("usr/share/zoneinfo/Europe/Prague");
__static_yoink("usr/share/zoneinfo/Europe/Riga");
__static_yoink("usr/share/zoneinfo/Europe/Rome");
__static_yoink("usr/share/zoneinfo/Europe/Samara");
__static_yoink("usr/share/zoneinfo/Europe/Saratov");
__static_yoink("usr/share/zoneinfo/Europe/Simferopol");
__static_yoink("usr/share/zoneinfo/Europe/Sofia");
__static_yoink("usr/share/zoneinfo/Europe/Tallinn");
__static_yoink("usr/share/zoneinfo/Europe/Tirane");
__static_yoink("usr/share/zoneinfo/Europe/Ulyanovsk");
__static_yoink("usr/share/zoneinfo/Europe/Vienna");
__static_yoink("usr/share/zoneinfo/Europe/Vilnius");
__static_yoink("usr/share/zoneinfo/Europe/Volgograd");
__static_yoink("usr/share/zoneinfo/Europe/Warsaw");
__static_yoink("usr/share/zoneinfo/Europe/Zurich");
#endif

__static_yoink("usr/share/zoneinfo/Asia/");
__static_yoink("usr/share/zoneinfo/Asia/Jerusalem"); // Israel
__static_yoink("usr/share/zoneinfo/Asia/Taipei"); // Taiwan
__static_yoink("usr/share/zoneinfo/Asia/Kolkata"); // India
__static_yoink("usr/share/zoneinfo/Asia/Tokyo"); // Japan
__static_yoink("usr/share/zoneinfo/Asia/Shanghai"); // China
__static_yoink("usr/share/zoneinfo/Asia/Dubai"); // U.A.E.
__static_yoink("usr/share/zoneinfo/Asia/Seoul"); // South Korea
__static_yoink("usr/share/zoneinfo/Asia/Singapore"); // Singapore
__static_yoink("usr/share/zoneinfo/Asia/Tehran"); // Iran
__static_yoink("usr/share/zoneinfo/Asia/Hong_Kong"); // Hong Kong
__static_yoink("usr/share/zoneinfo/Asia/Manila"); // Philippines
__static_yoink("usr/share/zoneinfo/Asia/Bangkok"); // Thailand
__static_yoink("usr/share/zoneinfo/Asia/Jakarta"); // Indonesia
__static_yoink("usr/share/zoneinfo/Asia/Karachi"); // Pakistan
__static_yoink("usr/share/zoneinfo/Asia/Kabul"); // Afghanistan
__static_yoink("usr/share/zoneinfo/Asia/Dhaka"); // Bangladesh
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Asia/Almaty");
__static_yoink("usr/share/zoneinfo/Asia/Amman");
__static_yoink("usr/share/zoneinfo/Asia/Anadyr");
__static_yoink("usr/share/zoneinfo/Asia/Aqtau");
__static_yoink("usr/share/zoneinfo/Asia/Aqtobe");
__static_yoink("usr/share/zoneinfo/Asia/Ashgabat");
__static_yoink("usr/share/zoneinfo/Asia/Atyrau");
__static_yoink("usr/share/zoneinfo/Asia/Baghdad");
__static_yoink("usr/share/zoneinfo/Asia/Baku");
__static_yoink("usr/share/zoneinfo/Asia/Barnaul");
__static_yoink("usr/share/zoneinfo/Asia/Beirut");
__static_yoink("usr/share/zoneinfo/Asia/Bishkek");
__static_yoink("usr/share/zoneinfo/Asia/Chita");
__static_yoink("usr/share/zoneinfo/Asia/Choibalsan");
__static_yoink("usr/share/zoneinfo/Asia/Colombo");
__static_yoink("usr/share/zoneinfo/Asia/Damascus");
__static_yoink("usr/share/zoneinfo/Asia/Dili");
__static_yoink("usr/share/zoneinfo/Asia/Dushanbe");
__static_yoink("usr/share/zoneinfo/Asia/Famagusta");
__static_yoink("usr/share/zoneinfo/Asia/Gaza");
__static_yoink("usr/share/zoneinfo/Asia/Hebron");
__static_yoink("usr/share/zoneinfo/Asia/Ho_Chi_Minh");
__static_yoink("usr/share/zoneinfo/Asia/Hovd");
__static_yoink("usr/share/zoneinfo/Asia/Irkutsk");
__static_yoink("usr/share/zoneinfo/Asia/Jayapura");
__static_yoink("usr/share/zoneinfo/Asia/Kamchatka");
__static_yoink("usr/share/zoneinfo/Asia/Kathmandu");
__static_yoink("usr/share/zoneinfo/Asia/Khandyga");
__static_yoink("usr/share/zoneinfo/Asia/Krasnoyarsk");
__static_yoink("usr/share/zoneinfo/Asia/Kuching");
__static_yoink("usr/share/zoneinfo/Asia/Macau");
__static_yoink("usr/share/zoneinfo/Asia/Magadan");
__static_yoink("usr/share/zoneinfo/Asia/Makassar");
__static_yoink("usr/share/zoneinfo/Asia/Nicosia");
__static_yoink("usr/share/zoneinfo/Asia/Novokuznetsk");
__static_yoink("usr/share/zoneinfo/Asia/Novosibirsk");
__static_yoink("usr/share/zoneinfo/Asia/Omsk");
__static_yoink("usr/share/zoneinfo/Asia/Oral");
__static_yoink("usr/share/zoneinfo/Asia/Pontianak");
__static_yoink("usr/share/zoneinfo/Asia/Pyongyang");
__static_yoink("usr/share/zoneinfo/Asia/Qatar");
__static_yoink("usr/share/zoneinfo/Asia/Qostanay");
__static_yoink("usr/share/zoneinfo/Asia/Qyzylorda");
__static_yoink("usr/share/zoneinfo/Asia/Riyadh");
__static_yoink("usr/share/zoneinfo/Asia/Sakhalin");
__static_yoink("usr/share/zoneinfo/Asia/Samarkand");
__static_yoink("usr/share/zoneinfo/Asia/Srednekolymsk");
__static_yoink("usr/share/zoneinfo/Asia/Tashkent");
__static_yoink("usr/share/zoneinfo/Asia/Tbilisi");
__static_yoink("usr/share/zoneinfo/Asia/Thimphu");
__static_yoink("usr/share/zoneinfo/Asia/Tomsk");
__static_yoink("usr/share/zoneinfo/Asia/Ulaanbaatar");
__static_yoink("usr/share/zoneinfo/Asia/Urumqi");
__static_yoink("usr/share/zoneinfo/Asia/Ust-Nera");
__static_yoink("usr/share/zoneinfo/Asia/Vladivostok");
__static_yoink("usr/share/zoneinfo/Asia/Yakutsk");
__static_yoink("usr/share/zoneinfo/Asia/Yangon");
__static_yoink("usr/share/zoneinfo/Asia/Yekaterinburg");
__static_yoink("usr/share/zoneinfo/Asia/Yerevan");
#endif

__static_yoink("usr/share/zoneinfo/Pacific/");
__static_yoink("usr/share/zoneinfo/Pacific/Guam"); // U.S.
__static_yoink("usr/share/zoneinfo/Pacific/Honolulu"); // U.S.
__static_yoink("usr/share/zoneinfo/Pacific/Auckland"); // New Zealand
__static_yoink("usr/share/zoneinfo/Pacific/Chatham"); // New Zealand
__static_yoink("usr/share/zoneinfo/Pacific/Fiji"); // Fiji
__static_yoink("usr/share/zoneinfo/Pacific/Port_Moresby"); // Papua New Guinea
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Pacific/Apia");
__static_yoink("usr/share/zoneinfo/Pacific/Bougainville");
__static_yoink("usr/share/zoneinfo/Pacific/Easter");
__static_yoink("usr/share/zoneinfo/Pacific/Efate");
__static_yoink("usr/share/zoneinfo/Pacific/Fakaofo");
__static_yoink("usr/share/zoneinfo/Pacific/Galapagos");
__static_yoink("usr/share/zoneinfo/Pacific/Gambier");
__static_yoink("usr/share/zoneinfo/Pacific/Guadalcanal");
__static_yoink("usr/share/zoneinfo/Pacific/Kanton");
__static_yoink("usr/share/zoneinfo/Pacific/Kiritimati");
__static_yoink("usr/share/zoneinfo/Pacific/Kosrae");
__static_yoink("usr/share/zoneinfo/Pacific/Kwajalein");
__static_yoink("usr/share/zoneinfo/Pacific/Marquesas");
__static_yoink("usr/share/zoneinfo/Pacific/Nauru");
__static_yoink("usr/share/zoneinfo/Pacific/Niue");
__static_yoink("usr/share/zoneinfo/Pacific/Norfolk");
__static_yoink("usr/share/zoneinfo/Pacific/Noumea");
__static_yoink("usr/share/zoneinfo/Pacific/Pago_Pago");
__static_yoink("usr/share/zoneinfo/Pacific/Palau");
__static_yoink("usr/share/zoneinfo/Pacific/Pitcairn");
__static_yoink("usr/share/zoneinfo/Pacific/Rarotonga");
__static_yoink("usr/share/zoneinfo/Pacific/Tahiti");
__static_yoink("usr/share/zoneinfo/Pacific/Tarawa");
__static_yoink("usr/share/zoneinfo/Pacific/Tongatapu");
#endif

__static_yoink("usr/share/zoneinfo/Africa/");
__static_yoink("usr/share/zoneinfo/Africa/Lagos"); // Nigeria
__static_yoink("usr/share/zoneinfo/Africa/Cairo"); // Egypt
__static_yoink("usr/share/zoneinfo/Africa/Algiers"); // Algeria
__static_yoink("usr/share/zoneinfo/Africa/Johannesburg"); // South Africa
__static_yoink("usr/share/zoneinfo/Africa/Nairobi"); // Kenya
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Africa/Casablanca");
__static_yoink("usr/share/zoneinfo/Africa/Abidjan");
__static_yoink("usr/share/zoneinfo/Africa/Bissau");
__static_yoink("usr/share/zoneinfo/Africa/Ceuta");
__static_yoink("usr/share/zoneinfo/Africa/El_Aaiun");
__static_yoink("usr/share/zoneinfo/Africa/Juba");
__static_yoink("usr/share/zoneinfo/Africa/Khartoum");
__static_yoink("usr/share/zoneinfo/Africa/Maputo");
__static_yoink("usr/share/zoneinfo/Africa/Monrovia");
__static_yoink("usr/share/zoneinfo/Africa/Ndjamena");
__static_yoink("usr/share/zoneinfo/Africa/Sao_Tome");
__static_yoink("usr/share/zoneinfo/Africa/Tripoli");
__static_yoink("usr/share/zoneinfo/Africa/Tunis");
__static_yoink("usr/share/zoneinfo/Africa/Windhoek");
#endif

__static_yoink("usr/share/zoneinfo/Australia/");
__static_yoink("usr/share/zoneinfo/Australia/Sydney");
__static_yoink("usr/share/zoneinfo/Australia/Melbourne");
__static_yoink("usr/share/zoneinfo/Australia/Brisbane");
__static_yoink("usr/share/zoneinfo/Australia/Perth");
__static_yoink("usr/share/zoneinfo/Australia/Adelaide");
#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Australia/Broken_Hill");
__static_yoink("usr/share/zoneinfo/Australia/Darwin");
__static_yoink("usr/share/zoneinfo/Australia/Eucla");
__static_yoink("usr/share/zoneinfo/Australia/Hobart");
__static_yoink("usr/share/zoneinfo/Australia/Lindeman");
__static_yoink("usr/share/zoneinfo/Australia/Lord_Howe");
#endif

#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Atlantic/");
__static_yoink("usr/share/zoneinfo/Atlantic/Canary");
__static_yoink("usr/share/zoneinfo/Atlantic/Azores");
__static_yoink("usr/share/zoneinfo/Atlantic/Bermuda");
__static_yoink("usr/share/zoneinfo/Atlantic/Cape_Verde");
__static_yoink("usr/share/zoneinfo/Atlantic/Faroe");
__static_yoink("usr/share/zoneinfo/Atlantic/Madeira");
__static_yoink("usr/share/zoneinfo/Atlantic/South_Georgia");
__static_yoink("usr/share/zoneinfo/Atlantic/Stanley");
#endif

#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Indian/");
__static_yoink("usr/share/zoneinfo/Indian/Chagos");
__static_yoink("usr/share/zoneinfo/Indian/Maldives");
__static_yoink("usr/share/zoneinfo/Indian/Mauritius");
#endif

#ifdef EMBED_EVERY_TIME_ZONE
__static_yoink("usr/share/zoneinfo/Antarctica/");
__static_yoink("usr/share/zoneinfo/Antarctica/Casey");
__static_yoink("usr/share/zoneinfo/Antarctica/Davis");
__static_yoink("usr/share/zoneinfo/Antarctica/Macquarie");
__static_yoink("usr/share/zoneinfo/Antarctica/Mawson");
__static_yoink("usr/share/zoneinfo/Antarctica/Palmer");
__static_yoink("usr/share/zoneinfo/Antarctica/Rothera");
__static_yoink("usr/share/zoneinfo/Antarctica/Troll");
__static_yoink("usr/share/zoneinfo/Antarctica/Vostok");
#endif

#define ABS(X) ((X) >= 0 ? (X) : -(X))

static const struct {
	const char *const wname;
	const char *const zname;
} kWindowsZones[] = {
	{"AUS Eastern Standard Time", "Australia/Sydney"}, // +1000 AEST
	{"Afghanistan Standard Time", "Asia/Kabul"}, // +0430 +0430
	{"Alaskan Standard Time", "America/Anchorage"}, // -0800 AKDT
	{"Arabian Standard Time", "Asia/Dubai"}, // +0400 +04
	{"Argentina Standard Time", "America/Argentina/Buenos_Aires"}, // -0300 -03
	{"Bangladesh Standard Time", "Asia/Dhaka"}, // +0600 +06
	{"Cen. Australia Standard Time", "Australia/Adelaide"}, // +0930 ACST
	{"Central Standard Time", "America/Chicago"}, // -0500 CDT
	{"Central Standard Time (Mexico)", "America/Mexico_City"}, // -0600 CST
	{"China Standard Time", "Asia/Shanghai"}, // +0800 CST
	{"Dateline Standard Time", "Etc/GMT+12"}, // -1200 -12
	{"E. Africa Standard Time", "Africa/Nairobi"}, // +0300 EAT
	{"E. Australia Standard Time", "Australia/Brisbane"}, // +1000 AEST
	{"E. South America Standard Time", "America/Sao_Paulo"}, // -0300 -03
	{"Eastern Standard Time", "America/New_York"}, // -0400 EDT
	{"Egypt Standard Time", "Africa/Cairo"}, // +0300 EEST
	{"Fiji Standard Time", "Pacific/Fiji"}, // +1200 +12
	{"GTB Standard Time", "Europe/Bucharest"}, // +0300 EEST
	{"Hawaiian Standard Time", "Pacific/Honolulu"}, // -1000 HST
	{"India Standard Time", "Asia/Kolkata"}, // +0530 IST
	{"Iran Standard Time", "Asia/Tehran"}, // +0330 +0330
	{"Israel Standard Time", "Asia/Jerusalem"}, // +0300 IDT
	{"Korea Standard Time", "Asia/Seoul"}, // +0900 KST
	{"Mid-Atlantic Standard Time", "Etc/GMT+2"}, // -0200 -02
	{"Mountain Standard Time", "America/Denver"}, // -0600 MDT
	{"New Zealand Standard Time", "Pacific/Auckland"}, // +1200 NZST
	{"Pacific SA Standard Time", "America/Santiago"}, // -0400 -04
	{"Pacific Standard Time", "America/Los_Angeles"}, // -0700 PDT
	{"Pakistan Standard Time", "Asia/Karachi"}, // +0500 PKT
	{"SA Pacific Standard Time", "America/Bogota"}, // -0500 -05
	{"SE Asia Standard Time", "Asia/Bangkok"}, // +0700 +07
	{"Singapore Standard Time", "Asia/Singapore"}, // +0800 +08
	{"South Africa Standard Time", "Africa/Johannesburg"}, // +0200 SAST
	{"Taipei Standard Time", "Asia/Taipei"}, // +0800 CST
	{"Tokyo Standard Time", "Asia/Tokyo"}, // +0900 JST
	{"Turkey Standard Time", "Europe/Istanbul"}, // +0300 +03
	{"US Mountain Standard Time", "America/Phoenix"}, // -0700 MST
	{"UTC", "Etc/UTC"}, // +0000 UTC
	{"UTC+12", "Etc/GMT-12"}, // +1200 +12
	{"UTC-02", "Etc/GMT+2"}, // -0200 -02
	{"UTC-08", "Etc/GMT+8"}, // -0800 -08
	{"UTC-09", "Etc/GMT+9"}, // -0900 -09
	{"UTC-11", "Etc/GMT+11"}, // -1100 -11
	{"W. Australia Standard Time", "Australia/Perth"}, // +0800 AWST
	{"W. Central Africa Standard Time", "Africa/Lagos"}, // +0100 WAT
	{"West Pacific Standard Time", "Pacific/Port_Moresby"}, // +1000 +10
#ifdef EMBED_EVERY_TIME_ZONE
	{"AUS Central Standard Time", "Australia/Darwin"}, // +0930 ACST
	{"Aleutian Standard Time", "America/Adak"}, // -0900 HDT
	{"Altai Standard Time", "Asia/Barnaul"}, // +0700 +07
	{"Arab Standard Time", "Asia/Riyadh"}, // +0300 +03
	{"Arabic Standard Time", "Asia/Baghdad"}, // +0300 +03
	{"Astrakhan Standard Time", "Europe/Astrakhan"}, // +0400 +04
	{"Atlantic Standard Time", "America/Halifax"}, // -0300 ADT
	{"Aus Central W. Standard Time", "Australia/Eucla"}, // +0845 +0845
	{"Azerbaijan Standard Time", "Asia/Baku"}, // +0400 +04
	{"Azores Standard Time", "Atlantic/Azores"}, // +0000 +00
	{"Bahia Standard Time", "America/Bahia"}, // -0300 -03
	{"Belarus Standard Time", "Europe/Minsk"}, // +0300 +03
	{"Bougainville Standard Time", "Pacific/Bougainville"}, // +1100 +11
	{"Canada Central Standard Time", "America/Regina"}, // -0600 CST
	{"Cape Verde Standard Time", "Atlantic/Cape_Verde"}, // -0100 -01
	{"Caucasus Standard Time", "Asia/Yerevan"}, // +0400 +04
	{"Central America Standard Time", "America/Guatemala"}, // -0600 CST
	{"Central Asia Standard Time", "Asia/Almaty"}, // +0500 +05
	{"Central Brazilian Standard Time", "America/Cuiaba"}, // -0400 -04
	{"Central Europe Standard Time", "Europe/Budapest"}, // +0200 CEST
	{"Central European Standard Time", "Europe/Warsaw"}, // +0200 CEST
	{"Central Pacific Standard Time", "Pacific/Guadalcanal"}, // +1100 +11
	{"Chatham Islands Standard Time", "Pacific/Chatham"}, // +1245 +1245
	{"Cuba Standard Time", "America/Havana"}, // -0400 CDT
	{"E. Europe Standard Time", "Europe/Chisinau"}, // +0300 EEST
	{"Easter Island Standard Time", "Pacific/Easter"}, // -0600 -06
	{"Eastern Standard Time (Mexico)", "America/Cancun"}, // -0500 EST
	{"Ekaterinburg Standard Time", "Asia/Yekaterinburg"}, // +0500 +05
	{"FLE Standard Time", "Europe/Kiev"}, // +0300 EEST
	{"GMT Standard Time", "Europe/London"}, // +0100 BST
	{"Georgian Standard Time", "Asia/Tbilisi"}, // +0400 +04
	{"Greenland Standard Time", "America/Godthab"}, // -0100 -01
	{"Greenwich Standard Time", "Atlantic/Reykjavik"}, // +0000 GMT
	{"Haiti Standard Time", "America/Port-au-Prince"}, // -0400 EDT
	{"Jordan Standard Time", "Asia/Amman"}, // +0300 +03
	{"Kaliningrad Standard Time", "Europe/Kaliningrad"}, // +0200 EET
	{"Kamchatka Standard Time", "Asia/Kamchatka"}, // +1200 +12
	{"Libya Standard Time", "Africa/Tripoli"}, // +0200 EET
	{"Line Islands Standard Time", "Pacific/Kiritimati"}, // +1400 +14
	{"Lord Howe Standard Time", "Australia/Lord_Howe"}, // +1030 +1030
	{"Magadan Standard Time", "Asia/Magadan"}, // +1100 +11
	{"Marquesas Standard Time", "Pacific/Marquesas"}, // -0930 -0930
	{"Mauritius Standard Time", "Indian/Mauritius"}, // +0400 +04
	{"Middle East Standard Time", "Asia/Beirut"}, // +0300 EEST
	{"Montevideo Standard Time", "America/Montevideo"}, // -0300 -03
	{"Morocco Standard Time", "Africa/Casablanca"}, // +0100 +01
	{"Mountain Standard Time (Mexico)", "America/Chihuahua"}, // -0600 CST
	{"Myanmar Standard Time", "Asia/Yangon"}, // +0630 +0630
	{"N. Central Asia Standard Time", "Asia/Novosibirsk"}, // +0700 +07
	{"Namibia Standard Time", "Africa/Windhoek"}, // +0200 CAT
	{"Nepal Standard Time", "Asia/Kathmandu"}, // +0545 +0545
	{"Newfoundland Standard Time", "America/St_Johns"}, // -0230 NDT
	{"Norfolk Standard Time", "Pacific/Norfolk"}, // +1100 +11
	{"North Asia East Standard Time", "Asia/Irkutsk"}, // +0800 +08
	{"North Asia Standard Time", "Asia/Krasnoyarsk"}, // +0700 +07
	{"North Korea Standard Time", "Asia/Pyongyang"}, // +0900 KST
	{"Omsk Standard Time", "Asia/Omsk"}, // +0600 +06
	{"Pacific Standard Time (Mexico)", "America/Tijuana"}, // -0700 PDT
	{"Paraguay Standard Time", "America/Asuncion"}, // -0400 -04
	{"Romance Standard Time", "Europe/Paris"}, // +0200 CEST
	{"Russia Time Zone 10", "Asia/Srednekolymsk"}, // +1100 +11
	{"Russia Time Zone 11", "Asia/Kamchatka"}, // +1200 +12
	{"Russia Time Zone 3", "Europe/Samara"}, // +0400 +04
	{"Russian Standard Time", "Europe/Moscow"}, // +0300 MSK
	{"SA Eastern Standard Time", "America/Cayenne"}, // -0300 -03
	{"SA Western Standard Time", "America/La_Paz"}, // -0400 -04
	{"Saint Pierre Standard Time", "America/Miquelon"}, // -0200 -02
	{"Sakhalin Standard Time", "Asia/Sakhalin"}, // +1100 +11
	{"Samoa Standard Time", "Pacific/Apia"}, // +1300 +13
	{"Sri Lanka Standard Time", "Asia/Colombo"}, // +0530 +0530
	{"Syria Standard Time", "Asia/Damascus"}, // +0300 +03
	{"Tasmania Standard Time", "Australia/Hobart"}, // +1000 AEST
	{"Tocantins Standard Time", "America/Araguaina"}, // -0300 -03
	{"Tomsk Standard Time", "Asia/Tomsk"}, // +0700 +07
	{"Tonga Standard Time", "Pacific/Tongatapu"}, // +1300 +13
	{"Transbaikal Standard Time", "Asia/Chita"}, // +0900 +09
	{"Turks And Caicos Standard Time", "America/Grand_Turk"}, // -0400 EDT
	{"US Eastern Standard Time", "America/Indiana/Indianapolis"}, // -0400 EDT
	{"Ulaanbaatar Standard Time", "Asia/Ulaanbaatar"}, // +0800 +08
	{"Venezuela Standard Time", "America/Caracas"}, // -0400 -04
	{"Vladivostok Standard Time", "Asia/Vladivostok"}, // +1000 +10
	{"W. Europe Standard Time", "Europe/Berlin"}, // +0200 CEST
	{"W. Mongolia Standard Time", "Asia/Hovd"}, // +0700 +07
	{"West Asia Standard Time", "Asia/Tashkent"}, // +0500 +05
	{"West Bank Standard Time", "Asia/Hebron"}, // +0300 EEST
	{"Yakutsk Standard Time", "Asia/Yakutsk"}, // +0900 +09
#endif
};

static int
strcmp168(const char16_t *l, const char *r)
{
	int i = 0;
	while (l[i] == r[i] && r[i])
		++i;
	return l[i] - r[i];
}

static textwindows dontinline void
localtime_windows_init(void)
{
	int i;
	if (getenv("TZ"))
		return;
	struct NtTimeZoneInformation tzi;
	GetTimeZoneInformation(&tzi);
	if (!tzi.Bias)
		return;
	for (i = 0; i < sizeof(kWindowsZones) / sizeof(kWindowsZones[0]); ++i) {
		if (!strcmp168(tzi.StandardName, kWindowsZones[i].wname)) {
			setenv("TZ", kWindowsZones[i].zname, true);
			return;
		}
	}
	int hours = ABS(tzi.Bias) / 60 % 24;
	int minutes = ABS(tzi.Bias) % 60;
	char buf[16];
	buf[0] = '<';
	buf[1] = tzi.Bias > 0 ? '-' : '+';
	buf[2] = '0' + hours / 10;
	buf[4] = '0' + hours % 10;
	buf[5] = '0' + minutes / 10;
	buf[6] = '0' + minutes % 10;
	buf[7] = '>';
	buf[8] = tzi.Bias > 0 ? '+' : '-';
	buf[9] = '0' + hours / 10;
	buf[10] = '0' + hours % 10;
	buf[11] = ':';
	buf[12] = '0' + minutes / 10;
	buf[13] = '0' + minutes % 10;
	buf[14] = 0;
	setenv("TZ", buf, true);
}

__attribute__((__constructor__(80)))
textstartup static void
localtime_init(void)
{
	if (IsWindows())
		localtime_windows_init();
}

#ifndef TZ_ABBR_CHAR_SET
# define TZ_ABBR_CHAR_SET \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :+-._"
#endif /* !defined TZ_ABBR_CHAR_SET */

#ifndef TZ_ABBR_ERR_CHAR
# define TZ_ABBR_ERR_CHAR '_'
#endif /* !defined TZ_ABBR_ERR_CHAR */

/*
** Support non-POSIX platforms that distinguish between text and binary files.
*/

#ifndef O_BINARY
# define O_BINARY 0
#endif

#ifndef WILDABBR
/*
** Someone might make incorrect use of a time zone abbreviation:
**	1.	They might reference tzname[0] before calling tzset (explicitly
**		or implicitly).
**	2.	They might reference tzname[1] before calling tzset (explicitly
**		or implicitly).
**	3.	They might reference tzname[1] after setting to a time zone
**		in which Daylight Saving Time is never observed.
**	4.	They might reference tzname[0] after setting to a time zone
**		in which Standard Time is never observed.
**	5.	They might reference tm.TM_ZONE after calling offtime.
** What's best to do in the above cases is open to debate;
** for now, we just set things up so that in any of the five cases
** WILDABBR is used. Another possibility: initialize tzname[0] to the
** string "tzname[0] used before set", and similarly for the other cases.
** And another: initialize tzname[0] to "ERA", with an explanation in the
** manual page of what this "time zone abbreviation" means (doing this so
** that tzname[0] has the "normal" length of three characters).
*/
# define WILDABBR "   "
#endif /* !defined WILDABBR */

static const char	wildabbr[] = WILDABBR;

static char const etc_utc[] = "Etc/UTC";
static char const *utc = etc_utc + sizeof "Etc/" - 1;

/*
** The DST rules to use if TZ has no rules and we can't load TZDEFRULES.
** Default to US rules as of 2017-05-07.
** POSIX does not specify the default DST rules;
** for historical reasons, US rules are a common default.
*/
#ifndef TZDEFRULESTRING
# define TZDEFRULESTRING ",M3.2.0,M11.1.0"
#endif

struct ttinfo {				/* time type information */
	int_fast32_t	tt_utoff;	/* UT offset in seconds */
	bool		tt_isdst;	/* used to set tm_isdst */
	int		tt_desigidx;	/* abbreviation list index */
	bool		tt_ttisstd;	/* transition is std time */
	bool		tt_ttisut;	/* transition is UT */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	int_fast32_t	ls_corr;	/* correction to apply */
};

/* This abbreviation means local time is unspecified.  */
static char const UNSPEC[] = "-00";

/* How many extra bytes are needed at the end of struct state's chars array.
   This needs to be at least 1 for null termination in case the input
   data isn't properly terminated, and it also needs to be big enough
   for ttunspecified to work without crashing.  */
enum { CHARS_EXTRA = max(sizeof UNSPEC, 2) - 1 };

/* Limit to time zone abbreviation length in POSIX.1-2017-style TZ strings.
   This is distinct from TZ_MAX_CHARS, which limits TZif file contents.  */
#ifndef TZNAME_MAXIMUM
# define TZNAME_MAXIMUM 255
#endif

/* A representation of the contents of a TZif file.  Ideally this
   would have no size limits; the following sizes should suffice for
   practical use.  This struct should not be too large, as instances
   are put on the stack and stacks are relatively small on some platforms.
   See tzfile.h for more about the sizes.  */
struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	bool		goback;
	bool		goahead;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char chars[max(max(TZ_MAX_CHARS + CHARS_EXTRA, sizeof "UTC"),
		       2 * (TZNAME_MAXIMUM + 1))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];
};

enum r_type {
  JULIAN_DAY,		/* Jn = Julian day */
  DAY_OF_YEAR,		/* n = day of year */
  MONTH_NTH_DAY_OF_WEEK	/* Mm.n.d = month, week, day of week */
};

struct rule {
	enum r_type	r_type;		/* type of rule */
	int		r_day;		/* day number of rule */
	int		r_week;		/* week number of rule */
	int		r_mon;		/* month number of rule */
	int_fast32_t	r_time;		/* transition time of rule */
};

static struct tm *gmtsub(struct state const *, time_t const *, int_fast32_t,
			 struct tm *);
static bool increment_overflow(int *, int);
static bool increment_overflow_time(time_t *, int_fast32_t);
static int_fast32_t leapcorr(struct state const *, time_t);
static bool normalize_overflow32(int_fast32_t *, int *, int);
static struct tm *timesub(time_t const *, int_fast32_t, struct state const *,
			  struct tm *);
static bool localtime_tzparse(char const *, struct state *, struct state const *);

#ifdef ALL_STATE
static struct state *	lclptr;
static struct state *	gmtptr;
#endif /* defined ALL_STATE */

#ifndef ALL_STATE
static struct state	lclmem;
static struct state	gmtmem;
static struct state *const lclptr = &lclmem;
static struct state *const gmtptr = &gmtmem;
#endif /* State Farm */

#ifndef TZ_STRLEN_MAX
# define TZ_STRLEN_MAX 255
#endif /* !defined TZ_STRLEN_MAX */

static char		lcl_TZname[TZ_STRLEN_MAX + 1];
static int		lcl_is_set;

/*
** Section 4.12.3 of X3.159-1989 requires that
**	Except for the strftime function, these functions [asctime,
**	ctime, gmtime, localtime] return values in one of two static
**	objects: a broken-down time structure and an array of char.
** Thanks to Paul Eggert for noting this.
**
** This requirement was removed in C99, so support it only if requested,
** as support is more likely to lead to bugs in badly written programs.
*/

#if SUPPORT_C89
static struct tm	tm;
#endif

#if 2 <= HAVE_TZNAME + TZ_TIME_T
char *			tzname[2] = {
	(char *) wildabbr,
	(char *) wildabbr
};
#endif
#if 2 <= USG_COMPAT + TZ_TIME_T
long			timezone;
int			daylight;
#endif
#if 2 <= ALTZONE + TZ_TIME_T
long			altzone;
#endif

/* Initialize *S to a value based on UTOFF, ISDST, and DESIGIDX.  */
static void
init_ttinfo(struct ttinfo *s, int_fast32_t utoff, bool isdst, int desigidx)
{
	s->tt_utoff = utoff;
	s->tt_isdst = isdst;
	s->tt_desigidx = desigidx;
	s->tt_ttisstd = false;
	s->tt_ttisut = false;
}

/* Return true if SP's time type I does not specify local time.  */
static bool
ttunspecified(struct state const *sp, int i)
{
	char const *abbr = &sp->chars[sp->ttis[i].tt_desigidx];
	/* memcmp is likely faster than strcmp, and is safe due to CHARS_EXTRA.  */
	return memcmp(abbr, UNSPEC, sizeof UNSPEC) == 0;
}

static int_fast32_t
detzcode(const char *const codep)
{
	register int_fast32_t	result;
	register int		i;
	int_fast32_t one = 1;
	int_fast32_t halfmaxval = one << (32 - 2);
	int_fast32_t maxval = halfmaxval - 1 + halfmaxval;
	int_fast32_t minval = -1 - maxval;

	result = codep[0] & 0x7f;
	for (i = 1; i < 4; ++i)
		result = (result << 8) | (codep[i] & 0xff);

	if (codep[0] & 0x80) {
	  /* Do two's-complement negation even on non-two's-complement machines.
	     If the result would be minval - 1, return minval.  */
	  result -= !TWOS_COMPLEMENT(int_fast32_t) && result != 0;
	  result += minval;
	}
	return result;
}

static int_fast64_t
detzcode64(const char *const codep)
{
	register int_fast64_t result;
	register int	i;
	int_fast64_t one = 1;
	int_fast64_t halfmaxval = one << (64 - 2);
	int_fast64_t maxval = halfmaxval - 1 + halfmaxval;
	int_fast64_t minval = -TWOS_COMPLEMENT(int_fast64_t) - maxval;

	result = codep[0] & 0x7f;
	for (i = 1; i < 8; ++i)
		result = (result << 8) | (codep[i] & 0xff);

	if (codep[0] & 0x80) {
	  /* Do two's-complement negation even on non-two's-complement machines.
	     If the result would be minval - 1, return minval.  */
	  result -= !TWOS_COMPLEMENT(int_fast64_t) && result != 0;
	  result += minval;
	}
	return result;
}

static void
update_tzname_etc(struct state const *sp, struct ttinfo const *ttisp)
{
#if HAVE_TZNAME
	tzname[ttisp->tt_isdst] = (char *) &sp->chars[ttisp->tt_desigidx];
#endif
#if USG_COMPAT
	if (!ttisp->tt_isdst)
		timezone = - ttisp->tt_utoff;
#endif
#if ALTZONE
	if (ttisp->tt_isdst)
		altzone = - ttisp->tt_utoff;
#endif
}

/* If STDDST_MASK indicates that SP's TYPE provides useful info,
   update tzname, timezone, and/or altzone and return STDDST_MASK,
   diminished by the provided info if it is a specified local time.
   Otherwise, return STDDST_MASK.  See settzname for STDDST_MASK.  */
static int
may_update_tzname_etc(int stddst_mask, struct state *sp, int type)
{
	struct ttinfo *ttisp = &sp->ttis[type];
	int this_bit = 1 << ttisp->tt_isdst;
	if (stddst_mask & this_bit) {
		update_tzname_etc(sp, ttisp);
		if (!ttunspecified(sp, type))
			return stddst_mask & ~this_bit;
	}
	return stddst_mask;
}

static void
settzname(void)
{
	register struct state * const	sp = lclptr;
	register int			i;

	/* If STDDST_MASK & 1 we need info about a standard time.
	   If STDDST_MASK & 2 we need info about a daylight saving time.
	   When STDDST_MASK becomes zero we can stop looking.  */
	int stddst_mask = 0;

#if HAVE_TZNAME
	tzname[0] = tzname[1] = (char *) (sp ? wildabbr : utc);
	stddst_mask = 3;
#endif
#if USG_COMPAT
	timezone = 0;
	stddst_mask = 3;
#endif
#if ALTZONE
	altzone = 0;
	stddst_mask |= 2;
#endif
	/*
	** And to get the latest time zone abbreviations into tzname. . .
	*/
	if (sp) {
	  for (i = sp->timecnt - 1; stddst_mask && 0 <= i; i--)
	    stddst_mask = may_update_tzname_etc(stddst_mask, sp, sp->types[i]);
	  for (i = sp->typecnt - 1; stddst_mask && 0 <= i; i--)
	    stddst_mask = may_update_tzname_etc(stddst_mask, sp, i);
	}
#if USG_COMPAT
	daylight = stddst_mask >> 1 ^ 1;
#endif
}

/* Replace bogus characters in time zone abbreviations.
   Return 0 on success, an errno value if a time zone abbreviation is
   too long.  */
static int
scrub_abbrs(struct state *sp)
{
	int i;

	/* Reject overlong abbreviations.  */
	for (i = 0; i < sp->charcnt - (TZNAME_MAXIMUM + 1); ) {
	  int len = strlen(&sp->chars[i]);
	  if (TZNAME_MAXIMUM < len)
	    return EOVERFLOW;
	  i += len + 1;
	}

	/* Replace bogus characters.  */
	for (i = 0; i < sp->charcnt; ++i)
		if (strchr(TZ_ABBR_CHAR_SET, sp->chars[i]) == NULL)
			sp->chars[i] = TZ_ABBR_ERR_CHAR;

	return 0;
}

/* Input buffer for data read from a compiled tz file.  */
union input_buffer {
  /* The first part of the buffer, interpreted as a header.  */
  struct tzhead tzhead;

  /* The entire buffer.  Ideally this would have no size limits;
     the following should suffice for practical use.  */
  char buf[2 * sizeof(struct tzhead) + 2 * sizeof(struct state)
	   + 4 * TZ_MAX_TIMES];
};

/* TZDIR with a trailing '/' rather than a trailing '\0'.  */
static char const tzdirslash[sizeof TZDIR] = TZDIR "/";

/* Local storage needed for 'tzloadbody'.  */
union local_storage {
  /* The results of analyzing the file's contents after it is opened.  */
  struct file_analysis {
    /* The input buffer.  */
    union input_buffer u;

    /* A temporary state used for parsing a TZ string in the file.  */
    struct state st;
  } u;

  /* The name of the file to be opened.  Ideally this would have no
     size limits, to support arbitrarily long Zone names.
     Limiting Zone names to 1024 bytes should suffice for practical use.
     However, there is no need for this to be smaller than struct
     file_analysis as that struct is allocated anyway, as the other
     union member.  */
  char fullname[max(sizeof(struct file_analysis), sizeof tzdirslash + 1024)];
};

/* Load tz data from the file named NAME into *SP.  Read extended
   format if DOEXTEND.  Use *LSP for temporary storage.  Return 0 on
   success, an errno value on failure.  */
static int
localtime_tzloadbody_(char const *name, struct state *sp, bool doextend,
		      union local_storage *lsp)
{
	register int			i;
	register int			fid;
	register int			stored;
	register ssize_t		nread;
	register bool doaccess;
	register union input_buffer *up = &lsp->u.u;
	register int tzheadsize = sizeof(struct tzhead);

	// [jart] 1. polyfill common aliases
	//        2. polyfill legacy cosmo timezone names
	if (name) {
		if (startswith(name, "US/")) {
			if (!strcmp(name, "US/Eastern"))
				name = "America/New_York";
			else if (!strcmp(name, "US/Pacific"))
				name = "America/Los_Angeles";
			else if (!strcmp(name, "US/Alaska"))
				name = "America/Anchorage";
			else if (!strcmp(name, "US/Central"))
				name = "America/Chicago";
			else if (!strcmp(name, "US/Hawaii"))
				name = "Pacific/Honolulu";
			else if (!strcmp(name, "US/Michigan"))
				name = "America/Detroit";
			else if (!strcmp(name, "US/Mountain"))
				name = "America/Denver";
		} else if (!strcmp(name, "UTC")) {
			name = "Etc/UTC";
		} else if (!strcmp(name, "GST")) {
			name = "America/Los_Angeles";
		} else if (!strcmp(name, "Boulder")) {
			name = "America/Denver";
		} else if (!strcmp(name, "Chicago")) {
			name = "America/Chicago";
		} else if (!strcmp(name, "Anchorage")) {
			name = "America/Anchorage";
		} else if (!strcmp(name, "Honolulu")) {
			name = "Pacific/Honolulu";
		} else if (!strcmp(name, "London")) {
			name = "Europe/London";
		} else if (!strcmp(name, "Berlin")) {
			name = "Europe/Berlin";
		} else if (!strcmp(name, "Israel")) {
			name = "Asia/Jerusalem";
		} else if (!strcmp(name, "Beijing") ||
			   !strcmp(name, "Asia/Beijing")) {
			// "In China, there are only two time zone which
			//  are Asia/Beijing and Asia/Urumuqi, but no
			//  Asia/Shanghai or Asia/Chongqing"
			// https://bugs.launchpad.net/ubuntu/+source/libgweather/+bug/228554
			name = "Asia/Shanghai";
		}
	}

	sp->goback = sp->goahead = false;

	if (! name) {
		name = TZDEFAULT;
		if (! name)
		  return EINVAL;
	}

	if (name[0] == ':')
		++name;
#ifdef SUPPRESS_TZDIR
	/* Do not prepend TZDIR.  This is intended for specialized
	   applications only, due to its security implications.  */
	doaccess = true;
#else
	doaccess = name[0] == '/';
#endif
	if (!doaccess) {
		char const *dot;
		if (sizeof lsp->fullname - sizeof tzdirslash <= strlen(name))
		  return ENAMETOOLONG;

		/* Create a string "TZDIR/NAME".  Using sprintf here
		   would pull in stdio (and would fail if the
		   resulting string length exceeded INT_MAX!).  */
		memcpy(lsp->fullname, tzdirslash, sizeof tzdirslash);
		strcpy(lsp->fullname + sizeof tzdirslash, name);

		/* Set doaccess if NAME contains a ".." file name
		   component, as such a name could read a file outside
		   the TZDIR virtual subtree.  */
		for (dot = name; (dot = strchr(dot, '.')); dot++)
		  if ((dot == name || dot[-1] == '/') && dot[1] == '.'
		      && (dot[2] == '/' || !dot[2])) {
		    doaccess = true;
		    break;
		  }

		name = lsp->fullname;
	}
	if (doaccess && access(name, R_OK) != 0)
	  return errno;
	fid = open(name, O_RDONLY);
	if (fid < 0)
	  return errno;

	nread = read(fid, up->buf, sizeof up->buf);
	if (nread < tzheadsize) {
	  int err = nread < 0 ? errno : EINVAL;
	  close(fid);
	  return err;
	}
	if (close(fid) < 0)
	  return errno;
	for (stored = 4; stored <= 8; stored *= 2) {
	    char version = up->tzhead.tzh_version[0];
	    bool skip_datablock = stored == 4 && version;
	    int_fast32_t datablock_size;
	    int_fast32_t ttisstdcnt = detzcode(up->tzhead.tzh_ttisstdcnt);
	    int_fast32_t ttisutcnt = detzcode(up->tzhead.tzh_ttisutcnt);
	    int_fast64_t prevtr = -1;
	    int_fast32_t prevcorr = 0;
	    int_fast32_t leapcnt = detzcode(up->tzhead.tzh_leapcnt);
	    int_fast32_t timecnt = detzcode(up->tzhead.tzh_timecnt);
	    int_fast32_t typecnt = detzcode(up->tzhead.tzh_typecnt);
	    int_fast32_t charcnt = detzcode(up->tzhead.tzh_charcnt);
	    char const *p = up->buf + tzheadsize;
	    /* Although tzfile(5) currently requires typecnt to be nonzero,
	       support future formats that may allow zero typecnt
	       in files that have a TZ string and no transitions.  */
	    if (! (0 <= leapcnt && leapcnt < TZ_MAX_LEAPS
		   && 0 <= typecnt && typecnt < TZ_MAX_TYPES
		   && 0 <= timecnt && timecnt < TZ_MAX_TIMES
		   && 0 <= charcnt && charcnt < TZ_MAX_CHARS
		   && 0 <= ttisstdcnt && ttisstdcnt < TZ_MAX_TYPES
		   && 0 <= ttisutcnt && ttisutcnt < TZ_MAX_TYPES))
	      return EINVAL;
	    datablock_size
		    = (timecnt * stored		/* ats */
		       + timecnt		/* types */
		       + typecnt * 6		/* ttinfos */
		       + charcnt		/* chars */
		       + leapcnt * (stored + 4)	/* lsinfos */
		       + ttisstdcnt		/* ttisstds */
		       + ttisutcnt);		/* ttisuts */
	    if (nread < tzheadsize + datablock_size)
	      return EINVAL;
	    if (skip_datablock)
		p += datablock_size;
	    else {
		if (! ((ttisstdcnt == typecnt || ttisstdcnt == 0)
		       && (ttisutcnt == typecnt || ttisutcnt == 0)))
		  return EINVAL;

		sp->leapcnt = leapcnt;
		sp->timecnt = timecnt;
		sp->typecnt = typecnt;
		sp->charcnt = charcnt;

		/* Read transitions, discarding those out of time_t range.
		   But pretend the last transition before TIME_T_MIN
		   occurred at TIME_T_MIN.  */
		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			int_fast64_t at
			  = stored == 4 ? detzcode(p) : detzcode64(p);
			sp->types[i] = at <= TIME_T_MAX;
			if (sp->types[i]) {
			  time_t attime
			    = ((TYPE_SIGNED(time_t) ? at < TIME_T_MIN : at < 0)
			       ? TIME_T_MIN : at);
			  if (timecnt && attime <= sp->ats[timecnt - 1]) {
			    if (attime < sp->ats[timecnt - 1])
			      return EINVAL;
			    sp->types[i - 1] = 0;
			    timecnt--;
			  }
			  sp->ats[timecnt++] = attime;
			}
			p += stored;
		}

		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			unsigned char typ = *p++;
			if (sp->typecnt <= typ)
			  return EINVAL;
			if (sp->types[i])
				sp->types[timecnt++] = typ;
		}
		sp->timecnt = timecnt;
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;
			unsigned char isdst, desigidx;

			ttisp = &sp->ttis[i];
			ttisp->tt_utoff = detzcode(p);
			p += 4;
			isdst = *p++;
			if (! (isdst < 2))
			  return EINVAL;
			ttisp->tt_isdst = isdst;
			desigidx = *p++;
			if (! (desigidx < sp->charcnt))
			  return EINVAL;
			ttisp->tt_desigidx = desigidx;
		}
		for (i = 0; i < sp->charcnt; ++i)
			sp->chars[i] = *p++;
		/* Ensure '\0'-terminated, and make it safe to call
		   ttunspecified later.  */
		bzero(&sp->chars[i], CHARS_EXTRA);

		/* Read leap seconds, discarding those out of time_t range.  */
		leapcnt = 0;
		for (i = 0; i < sp->leapcnt; ++i) {
		  int_fast64_t tr = stored == 4 ? detzcode(p) : detzcode64(p);
		  int_fast32_t corr = detzcode(p + stored);
		  p += stored + 4;

		  /* Leap seconds cannot occur before the Epoch,
		     or out of order.  */
		  if (tr <= prevtr)
		    return EINVAL;

		  /* To avoid other botches in this code, each leap second's
		     correction must differ from the previous one's by 1
		     second or less, except that the first correction can be
		     any value; these requirements are more generous than
		     RFC 8536, to allow future RFC extensions.  */
		  if (! (i == 0
			 || (prevcorr < corr
			     ? corr == prevcorr + 1
			     : (corr == prevcorr
				|| corr == prevcorr - 1))))
		    return EINVAL;
		  prevtr = tr;
		  prevcorr = corr;

		  if (tr <= TIME_T_MAX) {
		    sp->lsis[leapcnt].ls_trans = tr;
		    sp->lsis[leapcnt].ls_corr = corr;
		    leapcnt++;
		  }
		}
		sp->leapcnt = leapcnt;

		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisstdcnt == 0)
				ttisp->tt_ttisstd = false;
			else {
				if (*p != true && *p != false)
				  return EINVAL;
				ttisp->tt_ttisstd = *p++;
			}
		}
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisutcnt == 0)
				ttisp->tt_ttisut = false;
			else {
				if (*p != true && *p != false)
						return EINVAL;
				ttisp->tt_ttisut = *p++;
			}
		}
	    }

	    nread -= p - up->buf;
	    memmove(up->buf, p, nread);

	    /* If this is an old file, we're done.  */
	    if (!version)
	      break;
	}
	if (doextend && nread > 2 &&
		up->buf[0] == '\n' && up->buf[nread - 1] == '\n' &&
		sp->typecnt + 2 <= TZ_MAX_TYPES) {
			struct state	*ts = &lsp->u.st;

			up->buf[nread - 1] = '\0';
			if (localtime_tzparse(&up->buf[1], ts, sp)) {

			  /* Attempt to reuse existing abbreviations.
			     Without this, America/Anchorage would be right on
			     the edge after 2037 when TZ_MAX_CHARS is 50, as
			     sp->charcnt equals 40 (for LMT AST AWT APT AHST
			     AHDT YST AKDT AKST) and ts->charcnt equals 10
			     (for AKST AKDT).  Reusing means sp->charcnt can
			     stay 40 in this example.  */
			  int gotabbr = 0;
			  int charcnt = sp->charcnt;
			  for (i = 0; i < ts->typecnt; i++) {
			    char *tsabbr = ts->chars + ts->ttis[i].tt_desigidx;
			    int j;
			    for (j = 0; j < charcnt; j++)
			      if (strcmp(sp->chars + j, tsabbr) == 0) {
				ts->ttis[i].tt_desigidx = j;
				gotabbr++;
				break;
			      }
			    if (! (j < charcnt)) {
			      int tsabbrlen = strlen(tsabbr);
			      if (j + tsabbrlen < TZ_MAX_CHARS) {
				strcpy(sp->chars + j, tsabbr);
				charcnt = j + tsabbrlen + 1;
				ts->ttis[i].tt_desigidx = j;
				gotabbr++;
			      }
			    }
			  }
			  if (gotabbr == ts->typecnt) {
			    sp->charcnt = charcnt;

			    /* Ignore any trailing, no-op transitions generated
			       by zic as they don't help here and can run afoul
			       of bugs in zic 2016j or earlier.  */
			    while (1 < sp->timecnt
				   && (sp->types[sp->timecnt - 1]
				       == sp->types[sp->timecnt - 2]))
			      sp->timecnt--;

			    sp->goahead = ts->goahead;

			    for (i = 0; i < ts->timecnt; i++) {
			      time_t t = ts->ats[i];
			      if (increment_overflow_time(&t, leapcorr(sp, t))
				  || (0 < sp->timecnt
				      && t <= sp->ats[sp->timecnt - 1]))
				continue;
			      if (TZ_MAX_TIMES <= sp->timecnt) {
				sp->goahead = false;
				break;
			      }
			      sp->ats[sp->timecnt] = t;
			      sp->types[sp->timecnt] = (sp->typecnt
							+ ts->types[i]);
			      sp->timecnt++;
			    }
			    for (i = 0; i < ts->typecnt; i++)
			      sp->ttis[sp->typecnt++] = ts->ttis[i];
			  }
			}
	}
	if (sp->typecnt == 0)
	  return EINVAL;

	return 0;
}

static int /* [jart] pthread cancelation safe */
localtime_tzloadbody(char const *name, struct state *sp, bool doextend,
		     union local_storage *lsp)
{
	int rc;
	BLOCK_CANCELATION;
	rc = localtime_tzloadbody_(name, sp, doextend, lsp);
	ALLOW_CANCELATION;
	return rc;
}

/* Load tz data from the file named NAME into *SP.  Read extended
   format if DOEXTEND.  Return 0 on success, an errno value on failure.  */
static int
localtime_tzload(char const *name, struct state *sp, bool doextend)
{
#ifdef ALL_STATE
	union local_storage *lsp = malloc(sizeof *lsp);
	if (!lsp) {
		return HAVE_MALLOC_ERRNO ? errno : ENOMEM;
	} else {
		int err = localtime_tzloadbody(name, sp, doextend, lsp);
		free(lsp);
		return err;
	}
#else
	union local_storage ls;
	return localtime_tzloadbody(name, sp, doextend, &ls);
#endif
}

static const int	mon_lengths[2][MONSPERYEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
	DAYSPERNYEAR, DAYSPERLYEAR
};

/* Is C an ASCII digit?  */
static inline bool
is_digit(char c)
{
	return '0' <= c && c <= '9';
}

/*
** Given a pointer into a timezone string, scan until a character that is not
** a valid character in a time zone abbreviation is found.
** Return a pointer to that character.
*/

static nosideeffect const char *
getzname(register const char *strp)
{
	register char	c;

	while ((c = *strp) != '\0' && !is_digit(c) && c != ',' && c != '-' &&
		c != '+')
			++strp;
	return strp;
}

/*
** Given a pointer into an extended timezone string, scan until the ending
** delimiter of the time zone abbreviation is located.
** Return a pointer to the delimiter.
**
** As with getzname above, the legal character set is actually quite
** restricted, with other characters producing undefined results.
** We don't do any checking here; checking is done later in common-case code.
*/

static nosideeffect const char *
getqzname(register const char *strp, const int delim)
{
	register int	c;

	while ((c = *strp) != '\0' && c != delim)
		++strp;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a number from that string.
** Check that the number is within a specified range; if it is not, return
** NULL.
** Otherwise, return a pointer to the first character not part of the number.
*/

static const char *
getnum(register const char *strp, int *const nump, const int min, const int max)
{
	register char	c;
	register int	num;

	if (strp == NULL || !is_digit(c = *strp))
		return NULL;
	num = 0;
	do {
		num = num * 10 + (c - '0');
		if (num > max)
			return NULL;	/* illegal value */
		c = *++strp;
	} while (is_digit(c));
	if (num < min)
		return NULL;		/* illegal value */
	*nump = num;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a number of seconds,
** in hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the number
** of seconds.
*/

static const char *
getsecs(register const char *strp, int_fast32_t *const secsp)
{
	int	num;
	int_fast32_t secsperhour = SECSPERHOUR;

	/*
	** 'HOURSPERDAY * DAYSPERWEEK - 1' allows quasi-POSIX rules like
	** "M10.4.6/26", which does not conform to POSIX,
	** but which specifies the equivalent of
	** "02:00 on the first Sunday on or after 23 Oct".
	*/
	strp = getnum(strp, &num, 0, HOURSPERDAY * DAYSPERWEEK - 1);
	if (strp == NULL)
		return NULL;
	*secsp = num * secsperhour;
	if (*strp == ':') {
		++strp;
		strp = getnum(strp, &num, 0, MINSPERHOUR - 1);
		if (strp == NULL)
			return NULL;
		*secsp += num * SECSPERMIN;
		if (*strp == ':') {
			++strp;
			/* 'SECSPERMIN' allows for leap seconds.  */
			strp = getnum(strp, &num, 0, SECSPERMIN);
			if (strp == NULL)
				return NULL;
			*secsp += num;
		}
	}
	return strp;
}

/*
** Given a pointer into a timezone string, extract an offset, in
** [+-]hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the time.
*/

static const char *
localtime_getoffset(register const char *strp, int_fast32_t *const offsetp)
{
	register bool neg = false;

	if (*strp == '-') {
		neg = true;
		++strp;
	} else if (*strp == '+')
		++strp;
	strp = getsecs(strp, offsetp);
	if (strp == NULL)
		return NULL;		/* illegal time */
	if (neg)
		*offsetp = -*offsetp;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a rule in the form
** date[/time]. See POSIX Base Definitions section 8.3 variable TZ
** for the format of "date" and "time".
** If a valid rule is not found, return NULL.
** Otherwise, return a pointer to the first character not part of the rule.
*/

static const char *
localtime_getrule(const char *strp, register struct rule *const rulep)
{
	if (*strp == 'J') {
		/*
		** Julian day.
		*/
		rulep->r_type = JULIAN_DAY;
		++strp;
		strp = getnum(strp, &rulep->r_day, 1, DAYSPERNYEAR);
	} else if (*strp == 'M') {
		/*
		** Month, week, day.
		*/
		rulep->r_type = MONTH_NTH_DAY_OF_WEEK;
		++strp;
		strp = getnum(strp, &rulep->r_mon, 1, MONSPERYEAR);
		if (strp == NULL)
			return NULL;
		if (*strp++ != '.')
			return NULL;
		strp = getnum(strp, &rulep->r_week, 1, 5);
		if (strp == NULL)
			return NULL;
		if (*strp++ != '.')
			return NULL;
		strp = getnum(strp, &rulep->r_day, 0, DAYSPERWEEK - 1);
	} else if (is_digit(*strp)) {
		/*
		** Day of year.
		*/
		rulep->r_type = DAY_OF_YEAR;
		strp = getnum(strp, &rulep->r_day, 0, DAYSPERLYEAR - 1);
	} else	return NULL;		/* invalid format */
	if (strp == NULL)
		return NULL;
	if (*strp == '/') {
		/*
		** Time specified.
		*/
		++strp;
		strp = localtime_getoffset(strp, &rulep->r_time);
	} else	rulep->r_time = 2 * SECSPERHOUR;	/* default = 2:00:00 */
	return strp;
}

/*
** Given a year, a rule, and the offset from UT at the time that rule takes
** effect, calculate the year-relative time that rule takes effect.
*/

static int_fast32_t
localtime_transtime(const int year, register const struct rule *const rulep,
		    const int_fast32_t offset)
{
	register bool	leapyear;
	register int_fast32_t value;
	register int	i;
	int		d, m1, yy0, yy1, yy2, dow;

	leapyear = isleap(year);
	switch (rulep->r_type) {

	case JULIAN_DAY:
		/*
		** Jn - Julian day, 1 == January 1, 60 == March 1 even in leap
		** years.
		** In non-leap years, or if the day number is 59 or less, just
		** add SECSPERDAY times the day number-1 to the time of
		** January 1, midnight, to get the day.
		*/
		value = (rulep->r_day - 1) * SECSPERDAY;
		if (leapyear && rulep->r_day >= 60)
			value += SECSPERDAY;
		break;

	case DAY_OF_YEAR:
		/*
		** n - day of year.
		** Just add SECSPERDAY times the day number to the time of
		** January 1, midnight, to get the day.
		*/
		value = rulep->r_day * SECSPERDAY;
		break;

	case MONTH_NTH_DAY_OF_WEEK:
		/*
		** Mm.n.d - nth "dth day" of month m.
		*/

		/*
		** Use Zeller's Congruence to get day-of-week of first day of
		** month.
		*/
		m1 = (rulep->r_mon + 9) % 12 + 1;
		yy0 = (rulep->r_mon <= 2) ? (year - 1) : year;
		yy1 = yy0 / 100;
		yy2 = yy0 % 100;
		dow = ((26 * m1 - 2) / 10 +
			1 + yy2 + yy2 / 4 + yy1 / 4 - 2 * yy1) % 7;
		if (dow < 0)
			dow += DAYSPERWEEK;

		/*
		** "dow" is the day-of-week of the first day of the month. Get
		** the day-of-month (zero-origin) of the first "dow" day of the
		** month.
		*/
		d = rulep->r_day - dow;
		if (d < 0)
			d += DAYSPERWEEK;
		for (i = 1; i < rulep->r_week; ++i) {
			if (d + DAYSPERWEEK >=
				mon_lengths[leapyear][rulep->r_mon - 1])
					break;
			d += DAYSPERWEEK;
		}

		/*
		** "d" is the day-of-month (zero-origin) of the day we want.
		*/
		value = d * SECSPERDAY;
		for (i = 0; i < rulep->r_mon - 1; ++i)
			value += mon_lengths[leapyear][i] * SECSPERDAY;
		break;

	default: unreachable();
	}

	/*
	** "value" is the year-relative time of 00:00:00 UT on the day in
	** question. To get the year-relative time of the specified local
	** time on that day, add the transition time and the current offset
	** from UT.
	*/
	return value + rulep->r_time + offset;
}

/*
** Given a POSIX.1-2017-style TZ string, fill in the rule tables as
** appropriate.
*/

static bool
localtime_tzparse(const char *name, struct state *sp, struct state const *basep)
{
	const char *			stdname;
	const char *			dstname;
	int_fast32_t			stdoffset;
	int_fast32_t			dstoffset;
	register char *			cp;
	register bool			load_ok;
	ptrdiff_t stdlen, dstlen, charcnt;
	time_t atlo = TIME_T_MIN, leaplo = TIME_T_MIN;

	stdname = name;
	if (*name == '<') {
	  name++;
	  stdname = name;
	  name = getqzname(name, '>');
	  if (*name != '>')
	    return false;
	  stdlen = name - stdname;
	  name++;
	} else {
	  name = getzname(name);
	  stdlen = name - stdname;
	}
	if (! (0 < stdlen && stdlen <= TZNAME_MAXIMUM))
	  return false;
	name = localtime_getoffset(name, &stdoffset);
	if (name == NULL)
	  return false;
	charcnt = stdlen + 1;
	if (basep) {
	  if (0 < basep->timecnt)
	    atlo = basep->ats[basep->timecnt - 1];
	  load_ok = false;
	  sp->leapcnt = basep->leapcnt;
	  memcpy(sp->lsis, basep->lsis, sp->leapcnt * sizeof *sp->lsis);
	} else {
	  load_ok = localtime_tzload(TZDEFRULES, sp, false) == 0;
	  if (!load_ok)
	    sp->leapcnt = 0;	/* So, we're off a little.  */
	}
	if (0 < sp->leapcnt)
	  leaplo = sp->lsis[sp->leapcnt - 1].ls_trans;
	sp->goback = sp->goahead = false;
	if (*name != '\0') {
		if (*name == '<') {
			dstname = ++name;
			name = getqzname(name, '>');
			if (*name != '>')
			  return false;
			dstlen = name - dstname;
			name++;
		} else {
			dstname = name;
			name = getzname(name);
			dstlen = name - dstname; /* length of DST abbr. */
		}
		if (! (0 < dstlen && dstlen <= TZNAME_MAXIMUM))
		  return false;
		charcnt += dstlen + 1;
		if (*name != '\0' && *name != ',' && *name != ';') {
			name = localtime_getoffset(name, &dstoffset);
			if (name == NULL)
			  return false;
		} else	dstoffset = stdoffset - SECSPERHOUR;
		if (*name == '\0' && !load_ok)
			name = TZDEFRULESTRING;
		if (*name == ',' || *name == ';') {
			struct rule	start;
			struct rule	end;
			register int	year;
			register int	timecnt;
			time_t		janfirst;
			int_fast32_t janoffset = 0;
			int yearbeg, yearlim;

			++name;
			if ((name = localtime_getrule(name, &start)) == NULL)
			  return false;
			if (*name++ != ',')
			  return false;
			if ((name = localtime_getrule(name, &end)) == NULL)
			  return false;
			if (*name != '\0')
			  return false;
			sp->typecnt = 2;	/* standard time and DST */
			/*
			** Two transitions per year, from EPOCH_YEAR forward.
			*/
			init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
			init_ttinfo(&sp->ttis[1], -dstoffset, true, stdlen + 1);
			timecnt = 0;
			janfirst = 0;
			yearbeg = EPOCH_YEAR;

			do {
			  int_fast32_t yearsecs
			    = year_lengths[isleap(yearbeg - 1)] * SECSPERDAY;
			  time_t janfirst1 = janfirst;
			  yearbeg--;
			  if (increment_overflow_time(&janfirst1, -yearsecs)) {
			    janoffset = -yearsecs;
			    break;
			  }
			  janfirst = janfirst1;
			} while (atlo < janfirst
				 && EPOCH_YEAR - YEARSPERREPEAT / 2 < yearbeg);

			while (true) {
			  int_fast32_t yearsecs
			    = year_lengths[isleap(yearbeg)] * SECSPERDAY;
			  int yearbeg1 = yearbeg;
			  time_t janfirst1 = janfirst;
			  if (increment_overflow_time(&janfirst1, yearsecs)
			      || increment_overflow(&yearbeg1, 1)
			      || atlo <= janfirst1)
			    break;
			  yearbeg = yearbeg1;
			  janfirst = janfirst1;
			}

			yearlim = yearbeg;
			if (increment_overflow(&yearlim, years_of_observations))
			  yearlim = INT_MAX;
			for (year = yearbeg; year < yearlim; year++) {
				int_fast32_t
				  starttime = localtime_transtime(year, &start, stdoffset),
				  endtime = localtime_transtime(year, &end, dstoffset);
				int_fast32_t
				  yearsecs = (year_lengths[isleap(year)]
					      * SECSPERDAY);
				bool reversed = endtime < starttime;
				if (reversed) {
					int_fast32_t swap = starttime;
					starttime = endtime;
					endtime = swap;
				}
				if (reversed
				    || (starttime < endtime
					&& endtime - starttime < yearsecs)) {
					if (TZ_MAX_TIMES - 2 < timecnt)
						break;
					sp->ats[timecnt] = janfirst;
					if (! increment_overflow_time
					    (&sp->ats[timecnt],
					     janoffset + starttime)
					    && atlo <= sp->ats[timecnt])
					  sp->types[timecnt++] = !reversed;
					sp->ats[timecnt] = janfirst;
					if (! increment_overflow_time
					    (&sp->ats[timecnt],
					     janoffset + endtime)
					    && atlo <= sp->ats[timecnt]) {
					  sp->types[timecnt++] = reversed;
					}
				}
				if (endtime < leaplo) {
				  yearlim = year;
				  if (increment_overflow(&yearlim,
							 years_of_observations))
				    yearlim = INT_MAX;
				}
				if (increment_overflow_time
				    (&janfirst, janoffset + yearsecs))
					break;
				janoffset = 0;
			}
			sp->timecnt = timecnt;
			if (! timecnt) {
				sp->ttis[0] = sp->ttis[1];
				sp->typecnt = 1;	/* Perpetual DST.  */
			} else if (years_of_observations <= year - yearbeg)
				sp->goback = sp->goahead = true;
		} else {
			register int_fast32_t	theirstdoffset;
			register int_fast32_t	theirdstoffset;
			register int_fast32_t	theiroffset;
			register bool		isdst;
			register int		i;
			register int		j;

			if (*name != '\0')
			  return false;
			/*
			** Initial values of theirstdoffset and theirdstoffset.
			*/
			theirstdoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (!sp->ttis[j].tt_isdst) {
					theirstdoffset =
						- sp->ttis[j].tt_utoff;
					break;
				}
			}
			theirdstoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (sp->ttis[j].tt_isdst) {
					theirdstoffset =
						- sp->ttis[j].tt_utoff;
					break;
				}
			}
			/*
			** Initially we're assumed to be in standard time.
			*/
			isdst = false;
			/*
			** Now juggle transition times and types
			** tracking offsets as you do.
			*/
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				sp->types[i] = sp->ttis[j].tt_isdst;
				if (sp->ttis[j].tt_ttisut) {
					/* No adjustment to transition time */
				} else {
					/*
					** If daylight saving time is in
					** effect, and the transition time was
					** not specified as standard time, add
					** the daylight saving time offset to
					** the transition time; otherwise, add
					** the standard time offset to the
					** transition time.
					*/
					/*
					** Transitions from DST to DDST
					** will effectively disappear since
					** POSIX.1-2017 provides for only one
					** DST offset.
					*/
					if (isdst && !sp->ttis[j].tt_ttisstd) {
						sp->ats[i] += dstoffset -
							theirdstoffset;
					} else {
						sp->ats[i] += stdoffset -
							theirstdoffset;
					}
				}
				theiroffset = -sp->ttis[j].tt_utoff;
				if (sp->ttis[j].tt_isdst)
					theirdstoffset = theiroffset;
				else	theirstdoffset = theiroffset;
			}
			/*
			** Finally, fill in ttis.
			*/
			init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
			init_ttinfo(&sp->ttis[1], -dstoffset, true, stdlen + 1);
			sp->typecnt = 2;
		}
	} else {
		dstlen = 0;
		sp->typecnt = 1;		/* only standard time */
		sp->timecnt = 0;
		init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
	}
	sp->charcnt = charcnt;
	cp = sp->chars;
	memcpy(cp, stdname, stdlen);
	cp += stdlen;
	*cp++ = '\0';
	if (dstlen != 0) {
		memcpy(cp, dstname, dstlen);
		*(cp + dstlen) = '\0';
	}
	return true;
}

static void
localtime_gmtload(struct state *const sp)
{
	if (localtime_tzload(etc_utc, sp, true) != 0)
		localtime_tzparse("UTC0", sp, NULL);
}

/* Initialize *SP to a value appropriate for the TZ setting NAME.
   Return 0 on success, an errno value on failure.  */
static int
localtime_zoneinit(struct state *sp, char const *name)
{
	if (name && ! name[0]) {
		/*
		** User wants it fast rather than right.
		*/
		sp->leapcnt = 0;		/* so, we're off a little */
		sp->timecnt = 0;
		sp->typecnt = 0;
		sp->charcnt = 0;
		sp->goback = sp->goahead = false;
		init_ttinfo(&sp->ttis[0], 0, false, 0);
		strcpy(sp->chars, utc);
		return 0;
	} else {
		int err = localtime_tzload(name, sp, true);
		if (err != 0 && name && name[0] != ':' && localtime_tzparse(name, sp, NULL))
			err = 0;
		if (err == 0)
			err = scrub_abbrs(sp);
		return err;
	}
}

static void
localtime_lclptr_free(void *p)
{
	free(p);
}

static void
localtime_tzset_unlocked(void)
{
	char const *name = getenv("TZ");
	struct state *sp = lclptr;
	int lcl = name ? strlen(name) < sizeof lcl_TZname : -1;
	if (lcl < 0
	    ? lcl_is_set < 0
	    : 0 < lcl_is_set && strcmp(lcl_TZname, name) == 0)
		return;
#ifdef ALL_STATE
	if (! sp) {
		lclptr = sp = malloc(sizeof *lclptr);
		__cxa_atexit(localtime_lclptr_free, sp, 0);
	}
#endif /* defined ALL_STATE */
	if (sp) {
		if (localtime_zoneinit(sp, name) != 0)
			localtime_zoneinit(sp, "");
		if (0 < lcl)
			strcpy(lcl_TZname, name);
	}
	settzname();
	lcl_is_set = lcl;
}

void
tzset(void)
{
	__localtime_lock();
	localtime_tzset_unlocked();
	__localtime_unlock();
}

static void
localtime_gmtptr_free(void *p)
{
	free(p);
}

static void
localtime_gmtcheck(void)
{
	static bool gmt_is_set;
	__localtime_lock();
	if (! gmt_is_set) {
#ifdef ALL_STATE
		gmtptr = malloc(sizeof *gmtptr);
		__cxa_atexit(localtime_gmtptr_free, gmtptr, 0);
#endif
		if (gmtptr)
			localtime_gmtload(gmtptr);
		gmt_is_set = true;
	}
	__localtime_unlock();
}

/*
** The easy way to behave "as if no library function calls" localtime
** is to not call it, so we drop its guts into "localsub", which can be
** freely called. (And no, the PANS doesn't require the above behavior,
** but it *is* desirable.)
**
** If successful and SETNAME is nonzero,
** set the applicable parts of tzname, timezone and altzone;
** however, it's OK to omit this step
** if the timezone is compatible with POSIX.1-2017
** since in that case tzset should have already done this step correctly.
** SETNAME's type is int_fast32_t for compatibility with gmtsub,
** but it is actually a boolean and its value should be 0 or 1.
*/

/*ARGSUSED*/
static struct tm *
localsub(struct state const *sp, time_t const *timep, int_fast32_t setname,
	 struct tm *const tmp)
{
	register const struct ttinfo *	ttisp;
	register int			i;
	register struct tm *		result;
	const time_t			t = *timep;

	if (sp == NULL) {
	  /* Don't bother to set tzname etc.; tzset has already done it.  */
	  return gmtsub(gmtptr, timep, 0, tmp);
	}
	if ((sp->goback && t < sp->ats[0]) ||
		(sp->goahead && t > sp->ats[sp->timecnt - 1])) {
			time_t newt;
			register time_t		seconds;
			register time_t		years;

			if (t < sp->ats[0])
				seconds = sp->ats[0] - t;
			else	seconds = t - sp->ats[sp->timecnt - 1];
			--seconds;

			/* Beware integer overflow, as SECONDS might
			   be close to the maximum time_t.  */
			years = seconds / SECSPERREPEAT * YEARSPERREPEAT;
			seconds = years * AVGSECSPERYEAR;
			years += YEARSPERREPEAT;
			if (t < sp->ats[0])
			  newt = t + seconds + SECSPERREPEAT;
			else
			  newt = t - seconds - SECSPERREPEAT;

			if (newt < sp->ats[0] ||
				newt > sp->ats[sp->timecnt - 1])
					return NULL;	/* "cannot happen" */
			result = localsub(sp, &newt, setname, tmp);
			if (result) {
#if defined ckd_add && defined ckd_sub
				if (t < sp->ats[0]
				    ? ckd_sub(&result->tm_year,
					      result->tm_year, years)
				    : ckd_add(&result->tm_year,
					      result->tm_year, years))
				  return NULL;
#else
				register int_fast64_t newy;

				newy = result->tm_year;
				if (t < sp->ats[0])
					newy -= years;
				else	newy += years;
				if (! (INT_MIN <= newy && newy <= INT_MAX))
					return NULL;
				result->tm_year = newy;
#endif
			}
			return result;
	}
	if (sp->timecnt == 0 || t < sp->ats[0]) {
		i = 0;
	} else {
		register int	lo = 1;
		register int	hi = sp->timecnt;

		while (lo < hi) {
			register int	mid = (lo + hi) >> 1;

			if (t < sp->ats[mid])
				hi = mid;
			else	lo = mid + 1;
		}
		i = sp->types[lo - 1];
	}
	ttisp = &sp->ttis[i];
	/*
	** To get (wrong) behavior that's compatible with System V Release 2.0
	** you'd replace the statement below with
	**	t += ttisp->tt_utoff;
	**	timesub(&t, 0L, sp, tmp);
	*/
	result = timesub(&t, ttisp->tt_utoff, sp, tmp);
	if (result) {
		result->tm_isdst = ttisp->tt_isdst;
#ifdef TM_ZONE
		result->TM_ZONE = (char *) &sp->chars[ttisp->tt_desigidx];
#endif /* defined TM_ZONE */
		if (setname)
			update_tzname_etc(sp, ttisp);
	}
	return result;
}

static struct tm *
localtime_tzset(time_t const *timep, struct tm *tmp, bool setname)
{
	__localtime_lock();
	if (setname || !lcl_is_set)
		localtime_tzset_unlocked();
	tmp = localsub(lclptr, timep, setname, tmp);
	__localtime_unlock();
	return tmp;
}

struct tm *
localtime(const time_t *timep)
{
#if !SUPPORT_C89
	static struct tm tm;
#endif
	return localtime_tzset(timep, &tm, true);
}

struct tm *
localtime_r(const time_t *restrict timep, struct tm *restrict tmp)
{
	return localtime_tzset(timep, tmp, false);
}

/*
** gmtsub is to gmtime as localsub is to localtime.
*/

static struct tm *
gmtsub(ATTRIBUTE_MAYBE_UNUSED struct state const *sp, time_t const *timep,
       int_fast32_t offset, struct tm *tmp)
{
	register struct tm *	result;

	result = timesub(timep, offset, gmtptr, tmp);
#ifdef TM_ZONE
	/*
	** Could get fancy here and deliver something such as
	** "+xx" or "-xx" if offset is non-zero,
	** but this is no time for a treasure hunt.
	*/
	tmp->TM_ZONE = ((char *)
			(offset ? wildabbr : gmtptr ? gmtptr->chars : utc));
#endif /* defined TM_ZONE */
	return result;
}

/*
 * Re-entrant version of gmtime.
 */

struct tm *
gmtime_r(time_t const *restrict timep, struct tm *restrict tmp)
{
	localtime_gmtcheck();
	return gmtsub(gmtptr, timep, 0, tmp);
}

struct tm *
gmtime(const time_t *timep)
{
#if !SUPPORT_C89
	static struct tm tm;
#endif
	return gmtime_r(timep, &tm);
}

/*
** Return the number of leap years through the end of the given year
** where, to make the math easy, the answer for year zero is defined as zero.
*/

static time_t
leaps_thru_end_of_nonneg(time_t y)
{
	return y / 4 - y / 100 + y / 400;
}

static time_t
leaps_thru_end_of(time_t y)
{
	return (y < 0
		? -1 - leaps_thru_end_of_nonneg(-1 - y)
		: leaps_thru_end_of_nonneg(y));
}

static struct tm *
timesub(const time_t *timep, int_fast32_t offset,
	const struct state *sp, struct tm *tmp)
{
	register const struct lsinfo *	lp;
	register time_t			tdays;
	register const int *		ip;
	register int_fast32_t		corr;
	register int			i;
	int_fast32_t idays, rem, dayoff, dayrem;
	time_t y;

	/* If less than SECSPERMIN, the number of seconds since the
	   most recent positive leap second; otherwise, do not add 1
	   to localtime tm_sec because of leap seconds.  */
	time_t secs_since_posleap = SECSPERMIN;

	corr = 0;
	i = (sp == NULL) ? 0 : sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (*timep >= lp->ls_trans) {
			corr = lp->ls_corr;
			if ((i == 0 ? 0 : lp[-1].ls_corr) < corr)
			  secs_since_posleap = *timep - lp->ls_trans;
			break;
		}
	}

	/* Calculate the year, avoiding integer overflow even if
	   time_t is unsigned.  */
	tdays = *timep / SECSPERDAY;
	rem = *timep % SECSPERDAY;
	rem += offset % SECSPERDAY - corr % SECSPERDAY + 3 * SECSPERDAY;
	dayoff = offset / SECSPERDAY - corr / SECSPERDAY + rem / SECSPERDAY - 3;
	rem %= SECSPERDAY;
	/* y = (EPOCH_YEAR
	        + floor((tdays + dayoff) / DAYSPERREPEAT) * YEARSPERREPEAT),
	   sans overflow.  But calculate against 1570 (EPOCH_YEAR -
	   YEARSPERREPEAT) instead of against 1970 so that things work
	   for localtime values before 1970 when time_t is unsigned.  */
	dayrem = tdays % DAYSPERREPEAT;
	dayrem += dayoff % DAYSPERREPEAT;
	y = (EPOCH_YEAR - YEARSPERREPEAT
	     + ((1 + dayoff / DAYSPERREPEAT + dayrem / DAYSPERREPEAT
		 - ((dayrem % DAYSPERREPEAT) < 0)
		 + tdays / DAYSPERREPEAT)
		* YEARSPERREPEAT));
	/* idays = (tdays + dayoff) mod DAYSPERREPEAT, sans overflow.  */
	idays = tdays % DAYSPERREPEAT;
	idays += dayoff % DAYSPERREPEAT + 2 * DAYSPERREPEAT;
	idays %= DAYSPERREPEAT;
	/* Increase Y and decrease IDAYS until IDAYS is in range for Y.  */
	while (year_lengths[isleap(y)] <= idays) {
		int tdelta = idays / DAYSPERLYEAR;
		int_fast32_t ydelta = tdelta + !tdelta;
		time_t newy = y + ydelta;
		register int	leapdays;
		leapdays = leaps_thru_end_of(newy - 1) -
			leaps_thru_end_of(y - 1);
		idays -= ydelta * DAYSPERNYEAR;
		idays -= leapdays;
		y = newy;
	}

#ifdef ckd_add
	if (ckd_add(&tmp->tm_year, y, -TM_YEAR_BASE)) {
	  errno = EOVERFLOW;
	  return NULL;
	}
#else
	if (!TYPE_SIGNED(time_t) && y < TM_YEAR_BASE) {
	  int signed_y = y;
	  tmp->tm_year = signed_y - TM_YEAR_BASE;
	} else if ((!TYPE_SIGNED(time_t) || INT_MIN + TM_YEAR_BASE <= y)
		   && y - TM_YEAR_BASE <= INT_MAX)
	  tmp->tm_year = y - TM_YEAR_BASE;
	else {
	  errno = EOVERFLOW;
	  return NULL;
	}
#endif
	tmp->tm_yday = idays;
	/*
	** The "extra" mods below avoid overflow problems.
	*/
	tmp->tm_wday = (TM_WDAY_BASE
			+ ((tmp->tm_year % DAYSPERWEEK)
			   * (DAYSPERNYEAR % DAYSPERWEEK))
			+ leaps_thru_end_of(y - 1)
			- leaps_thru_end_of(TM_YEAR_BASE - 1)
			+ idays);
	tmp->tm_wday %= DAYSPERWEEK;
	if (tmp->tm_wday < 0)
		tmp->tm_wday += DAYSPERWEEK;
	tmp->tm_hour = rem / SECSPERHOUR;
	rem %= SECSPERHOUR;
	tmp->tm_min = rem / SECSPERMIN;
	tmp->tm_sec = rem % SECSPERMIN;

	/* Use "... ??:??:60" at the end of the localtime minute containing
	   the second just before the positive leap second.  */
	tmp->tm_sec += secs_since_posleap <= tmp->tm_sec;

	ip = mon_lengths[isleap(y)];
	for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
		idays -= ip[tmp->tm_mon];
	tmp->tm_mday = idays + 1;
	tmp->tm_isdst = 0;
#ifdef TM_GMTOFF
	tmp->TM_GMTOFF = offset;
#endif /* defined TM_GMTOFF */
	return tmp;
}

/*
** Adapted from code provided by Robert Elz, who writes:
**	The "best" way to do mktime I think is based on an idea of Bob
**	Kridle's (so its said...) from a long time ago.
**	It does a binary search of the time_t space. Since time_t's are
**	just 32 bits, its a max of 32 iterations (even at 64 bits it
**	would still be very reasonable).
*/

#ifndef WRONG
# define WRONG (-1)
#endif /* !defined WRONG */

/*
** Normalize logic courtesy Paul Eggert.
*/

forceinline bool
increment_overflow(int *ip, int j)
{
#ifdef ckd_add
	return ckd_add(ip, *ip, j);
#else
	register int const	i = *ip;

	/*
	** If i >= 0 there can only be overflow if i + j > INT_MAX
	** or if j > INT_MAX - i; given i >= 0, INT_MAX - i cannot overflow.
	** If i < 0 there can only be overflow if i + j < INT_MIN
	** or if j < INT_MIN - i; given i < 0, INT_MIN - i cannot overflow.
	*/
	if ((i >= 0) ? (j > INT_MAX - i) : (j < INT_MIN - i))
		return true;
	*ip += j;
	return false;
#endif
}

forceinline bool
increment_overflow32(int_fast32_t *const lp, int const m)
{
#ifdef ckd_add
	return ckd_add(lp, *lp, m);
#else
	register int_fast32_t const	l = *lp;

	if ((l >= 0) ? (m > INT_FAST32_MAX - l) : (m < INT_FAST32_MIN - l))
		return true;
	*lp += m;
	return false;
#endif
}

forceinline bool
increment_overflow_time(time_t *tp, int_fast32_t j)
{
#ifdef ckd_add
	return ckd_add(tp, *tp, j);
#else
	/*
	** This is like
	** 'if (! (TIME_T_MIN <= *tp + j && *tp + j <= TIME_T_MAX)) ...',
	** except that it does the right thing even if *tp + j would overflow.
	*/
	if (! (j < 0
	       ? (TYPE_SIGNED(time_t) ? TIME_T_MIN - j <= *tp : -1 - j < *tp)
	       : *tp <= TIME_T_MAX - j))
		return true;
	*tp += j;
	return false;
#endif
}

static bool
normalize_overflow(int *const tensptr, int *const unitsptr, const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow(tensptr, tensdelta);
}

static bool
normalize_overflow32(int_fast32_t *tensptr, int *unitsptr, int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow32(tensptr, tensdelta);
}

static int
tmcomp(register const struct tm *const atmp,
       register const struct tm *const btmp)
{
	register int	result;

	if (atmp->tm_year != btmp->tm_year)
		return atmp->tm_year < btmp->tm_year ? -1 : 1;
	if ((result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
		(result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
		(result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
		(result = (atmp->tm_min - btmp->tm_min)) == 0)
			result = atmp->tm_sec - btmp->tm_sec;
	return result;
}

/* Copy to *DEST from *SRC.  Copy only the members needed for mktime,
   as other members might not be initialized.  */
static void
mktmcpy(struct tm *dest, struct tm const *src)
{
	dest->tm_sec = src->tm_sec;
	dest->tm_min = src->tm_min;
	dest->tm_hour = src->tm_hour;
	dest->tm_mday = src->tm_mday;
	dest->tm_mon = src->tm_mon;
	dest->tm_year = src->tm_year;
	dest->tm_isdst = src->tm_isdst;
#if defined TM_GMTOFF && ! UNINIT_TRAP
	dest->TM_GMTOFF = src->TM_GMTOFF;
#endif
}

static time_t
time2sub(struct tm *const tmp,
	 struct tm *(*funcp)(struct state const *, time_t const *,
			     int_fast32_t, struct tm *),
	 struct state const *sp,
	 const int_fast32_t offset,
	 bool *okayp,
	 bool do_norm_secs)
{
	register int			dir;
	register int			i, j;
	register int			saved_seconds;
	register int_fast32_t		li;
	register time_t			lo;
	register time_t			hi;
	int_fast32_t			y;
	time_t				newt;
	time_t				t;
	struct tm			yourtm, mytm;

	*okayp = false;
	mktmcpy(&yourtm, tmp);

	if (do_norm_secs) {
		if (normalize_overflow(&yourtm.tm_min, &yourtm.tm_sec,
			SECSPERMIN))
				return WRONG;
	}
	if (normalize_overflow(&yourtm.tm_hour, &yourtm.tm_min, MINSPERHOUR))
		return WRONG;
	if (normalize_overflow(&yourtm.tm_mday, &yourtm.tm_hour, HOURSPERDAY))
		return WRONG;
	y = yourtm.tm_year;
	if (normalize_overflow32(&y, &yourtm.tm_mon, MONSPERYEAR))
		return WRONG;
	/*
	** Turn y into an actual year number for now.
	** It is converted back to an offset from TM_YEAR_BASE later.
	*/
	if (increment_overflow32(&y, TM_YEAR_BASE))
		return WRONG;
	while (yourtm.tm_mday <= 0) {
		if (increment_overflow32(&y, -1))
			return WRONG;
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday += year_lengths[isleap(li)];
	}
	while (yourtm.tm_mday > DAYSPERLYEAR) {
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday -= year_lengths[isleap(li)];
		if (increment_overflow32(&y, 1))
			return WRONG;
	}
	for ( ; ; ) {
		i = mon_lengths[isleap(y)][yourtm.tm_mon];
		if (yourtm.tm_mday <= i)
			break;
		yourtm.tm_mday -= i;
		if (++yourtm.tm_mon >= MONSPERYEAR) {
			yourtm.tm_mon = 0;
			if (increment_overflow32(&y, 1))
				return WRONG;
		}
	}
#ifdef ckd_add
	if (ckd_add(&yourtm.tm_year, y, -TM_YEAR_BASE))
	  return WRONG;
#else
	if (increment_overflow32(&y, -TM_YEAR_BASE))
		return WRONG;
	if (! (INT_MIN <= y && y <= INT_MAX))
		return WRONG;
	yourtm.tm_year = y;
#endif
	if (yourtm.tm_sec >= 0 && yourtm.tm_sec < SECSPERMIN)
		saved_seconds = 0;
	else if (yourtm.tm_year < EPOCH_YEAR - TM_YEAR_BASE) {
		/*
		** We can't set tm_sec to 0, because that might push the
		** time below the minimum representable time.
		** Set tm_sec to 59 instead.
		** This assumes that the minimum representable time is
		** not in the same minute that a leap second was deleted from,
		** which is a safer assumption than using 58 would be.
		*/
		if (increment_overflow(&yourtm.tm_sec, 1 - SECSPERMIN))
			return WRONG;
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = SECSPERMIN - 1;
	} else {
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = 0;
	}
	/*
	** Do a binary search (this works whatever time_t's type is).
	*/
	lo = TIME_T_MIN;
	hi = TIME_T_MAX;
	for ( ; ; ) {
		t = lo / 2 + hi / 2;
		if (t < lo)
			t = lo;
		else if (t > hi)
			t = hi;
		if (! funcp(sp, &t, offset, &mytm)) {
			/*
			** Assume that t is too extreme to be represented in
			** a struct tm; arrange things so that it is less
			** extreme on the next pass.
			*/
			dir = (t > 0) ? 1 : -1;
		} else	dir = tmcomp(&mytm, &yourtm);
		if (dir != 0) {
			if (t == lo) {
				if (t == TIME_T_MAX)
					return WRONG;
				++t;
				++lo;
			} else if (t == hi) {
				if (t == TIME_T_MIN)
					return WRONG;
				--t;
				--hi;
			}
			if (lo > hi)
				return WRONG;
			if (dir > 0)
				hi = t;
			else	lo = t;
			continue;
		}
#if defined TM_GMTOFF && ! UNINIT_TRAP
		if (mytm.TM_GMTOFF != yourtm.TM_GMTOFF
		    && (yourtm.TM_GMTOFF < 0
			? (-SECSPERDAY <= yourtm.TM_GMTOFF
			   && (mytm.TM_GMTOFF <=
			       (min(INT_FAST32_MAX, LONG_MAX)
				+ yourtm.TM_GMTOFF)))
			: (yourtm.TM_GMTOFF <= SECSPERDAY
			   && ((max(INT_FAST32_MIN, LONG_MIN)
				+ yourtm.TM_GMTOFF)
			       <= mytm.TM_GMTOFF)))) {
		  /* MYTM matches YOURTM except with the wrong UT offset.
		     YOURTM.TM_GMTOFF is plausible, so try it instead.
		     It's OK if YOURTM.TM_GMTOFF contains uninitialized data,
		     since the guess gets checked.  */
		  time_t altt = t;
		  int_fast32_t diff = mytm.TM_GMTOFF - yourtm.TM_GMTOFF;
		  if (!increment_overflow_time(&altt, diff)) {
		    struct tm alttm;
		    if (funcp(sp, &altt, offset, &alttm)
			&& alttm.tm_isdst == mytm.tm_isdst
			&& alttm.TM_GMTOFF == yourtm.TM_GMTOFF
			&& tmcomp(&alttm, &yourtm) == 0) {
		      t = altt;
		      mytm = alttm;
		    }
		  }
		}
#endif
		if (yourtm.tm_isdst < 0 || mytm.tm_isdst == yourtm.tm_isdst)
			break;
		/*
		** Right time, wrong type.
		** Hunt for right time, right type.
		** It's okay to guess wrong since the guess
		** gets checked.
		*/
		if (sp == NULL)
			return WRONG;
		for (i = sp->typecnt - 1; i >= 0; --i) {
			if (sp->ttis[i].tt_isdst != yourtm.tm_isdst)
				continue;
			for (j = sp->typecnt - 1; j >= 0; --j) {
				if (sp->ttis[j].tt_isdst == yourtm.tm_isdst)
					continue;
				if (ttunspecified(sp, j))
				  continue;
				newt = (t + sp->ttis[j].tt_utoff
					- sp->ttis[i].tt_utoff);
				if (! funcp(sp, &newt, offset, &mytm))
					continue;
				if (tmcomp(&mytm, &yourtm) != 0)
					continue;
				if (mytm.tm_isdst != yourtm.tm_isdst)
					continue;
				/*
				** We have a match.
				*/
				t = newt;
				goto label;
			}
		}
		return WRONG;
	}
label:
	newt = t + saved_seconds;
	if ((newt < t) != (saved_seconds < 0))
		return WRONG;
	t = newt;
	if (funcp(sp, &t, offset, tmp))
		*okayp = true;
	return t;
}

static time_t
time2(struct tm * const	tmp,
      struct tm *(*funcp)(struct state const *, time_t const *,
			  int_fast32_t, struct tm *),
      struct state const *sp,
      const int_fast32_t offset,
      bool *okayp)
{
	time_t	t;

	/*
	** First try without normalization of seconds
	** (in case tm_sec contains a value associated with a leap second).
	** If that fails, try with normalization of seconds.
	*/
	t = time2sub(tmp, funcp, sp, offset, okayp, false);
	return *okayp ? t : time2sub(tmp, funcp, sp, offset, okayp, true);
}

static time_t
time1(struct tm *const tmp,
      struct tm *(*funcp)(struct state const *, time_t const *,
			  int_fast32_t, struct tm *),
      struct state const *sp,
      const int_fast32_t offset)
{
	register time_t			t;
	register int			samei, otheri;
	register int			sameind, otherind;
	register int			i;
	register int			nseen;
	char				seen[TZ_MAX_TYPES];
	unsigned char			types[TZ_MAX_TYPES];
	bool				okay;

	if (tmp == NULL) {
		errno = EINVAL;
		return WRONG;
	}
	if (tmp->tm_isdst > 1)
		tmp->tm_isdst = 1;
	t = time2(tmp, funcp, sp, offset, &okay);
	if (okay)
		return t;
	if (tmp->tm_isdst < 0)
#ifdef PCTS
		/*
		** POSIX Conformance Test Suite code courtesy Grant Sullivan.
		*/
		tmp->tm_isdst = 0;	/* reset to std and try again */
#else
		return t;
#endif /* !defined PCTS */
	/*
	** We're supposed to assume that somebody took a time of one type
	** and did some math on it that yielded a "struct tm" that's bad.
	** We try to divine the type they started from and adjust to the
	** type they need.
	*/
	if (sp == NULL)
		return WRONG;
	for (i = 0; i < sp->typecnt; ++i)
		seen[i] = false;
	nseen = 0;
	for (i = sp->timecnt - 1; i >= 0; --i)
		if (!seen[sp->types[i]] && !ttunspecified(sp, sp->types[i])) {
			seen[sp->types[i]] = true;
			types[nseen++] = sp->types[i];
		}
	for (sameind = 0; sameind < nseen; ++sameind) {
		samei = types[sameind];
		if (sp->ttis[samei].tt_isdst != tmp->tm_isdst)
			continue;
		for (otherind = 0; otherind < nseen; ++otherind) {
			otheri = types[otherind];
			if (sp->ttis[otheri].tt_isdst == tmp->tm_isdst)
				continue;
			tmp->tm_sec += (sp->ttis[otheri].tt_utoff
					- sp->ttis[samei].tt_utoff);
			tmp->tm_isdst = !tmp->tm_isdst;
			t = time2(tmp, funcp, sp, offset, &okay);
			if (okay)
				return t;
			tmp->tm_sec -= (sp->ttis[otheri].tt_utoff
					- sp->ttis[samei].tt_utoff);
			tmp->tm_isdst = !tmp->tm_isdst;
		}
	}
	return WRONG;
}

static time_t
mktime_tzname(struct state *sp, struct tm *tmp, bool setname)
{
	if (sp)
		return time1(tmp, localsub, sp, setname);
	else {
		localtime_gmtcheck();
		return time1(tmp, gmtsub, gmtptr, 0);
	}
}

time_t
mktime(struct tm *tmp)
{
	time_t t;
	__localtime_lock();
	localtime_tzset_unlocked();
	t = mktime_tzname(lclptr, tmp, true);
	__localtime_unlock();
	return t;
}

/* This function is obsolescent and may disapper in future releases.
   Callers can instead use mktime.  */
time_t
timelocal(struct tm *tmp)
{
	if (tmp != NULL)
		tmp->tm_isdst = -1;	/* in case it wasn't initialized */
	return mktime(tmp);
}

/* This function is obsolescent and may disapper in future releases.
   Callers can instead use mktime_z with a fixed-offset zone.  */
time_t
timeoff(struct tm *tmp, long offset)
{
	if (tmp)
		tmp->tm_isdst = 0;
	localtime_gmtcheck();
	return time1(tmp, gmtsub, gmtptr, offset);
}

time_t
timegm(struct tm *tmp)
{
	time_t t;
	struct tm tmcpy;
	mktmcpy(&tmcpy, tmp);
	tmcpy.tm_wday = -1;
	t = timeoff(&tmcpy, 0);
	if (0 <= tmcpy.tm_wday)
		*tmp = tmcpy;
	return t;
}

static int_fast32_t
leapcorr(struct state const *sp, time_t t)
{
	register struct lsinfo const *	lp;
	register int			i;

	i = sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (t >= lp->ls_trans)
			return lp->ls_corr;
	}
	return 0;
}
