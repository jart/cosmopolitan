# Thank you Alejandro Zeise for collecting this information
# https://gist.github.com/alejzeis/ad5827eb14b5c22109ba652a1a267af5

SUPPORTED = set((
  'Africa/Algiers',
  'Africa/Cairo',
  'Africa/Johannesburg',
  'Africa/Lagos',
  'Africa/Nairobi',
  'America/Anchorage',
  'America/Argentina/Buenos_Aires',
  'America/Bogota',
  'America/Chicago',
  'America/Denver',
  'America/Lima',
  'America/Los_Angeles',
  'America/Mexico_City',
  'America/New_York',
  'America/Phoenix',
  'America/Santiago',
  'America/Sao_Paulo',
  'Asia/Bangkok',
  'Asia/Dhaka',
  'Asia/Dubai',
  'Asia/Hong_Kong',
  'Asia/Jakarta',
  'Asia/Jerusalem',
  'Asia/Kabul',
  'Asia/Karachi',
  'Asia/Kolkata',
  'Asia/Manila',
  'Asia/Seoul',
  'Asia/Shanghai',
  'Asia/Singapore',
  'Asia/Taipei',
  'Asia/Tehran',
  'Asia/Tokyo',
  'Australia/Adelaide',
  'Australia/Brisbane',
  'Australia/Melbourne',
  'Australia/Perth',
  'Australia/Sydney',
  'CET',
  'CST6CDT',
  'EET',
  'EST',
  'EST5EDT',
  'Etc/GMT',
  'Etc/GMT+1',
  'Etc/GMT+10',
  'Etc/GMT+11',
  'Etc/GMT+12',
  'Etc/GMT+2',
  'Etc/GMT+3',
  'Etc/GMT+4',
  'Etc/GMT+5',
  'Etc/GMT+6',
  'Etc/GMT+7',
  'Etc/GMT+8',
  'Etc/GMT+9',
  'Etc/GMT-1',
  'Etc/GMT-10',
  'Etc/GMT-11',
  'Etc/GMT-12',
  'Etc/GMT-13',
  'Etc/GMT-14',
  'Etc/GMT-2',
  'Etc/GMT-3',
  'Etc/GMT-4',
  'Etc/GMT-5',
  'Etc/GMT-6',
  'Etc/GMT-7',
  'Etc/GMT-8',
  'Etc/GMT-9',
  'Etc/UTC',
  'Europe/Brussels',
  'Europe/Bucharest',
  'Europe/Dublin',
  'Europe/Istanbul',
  'GMT',
  'HST',
  'MET',
  'MST',
  'MST7MDT',
  'PST8PDT',
  'Pacific/Auckland',
  'Pacific/Fiji',
  'Pacific/Guam',
  'Pacific/Honolulu',
  'Pacific/Port_Moresby',
  'WET',
))

import re
import os
import subprocess

NAMES = set()
ZONES = set()
SUPERFLUOUS = set()
TABLE1 = []
TABLE2 = []

with open("/home/jart/scratch/windows-timezone-mappings.csv") as f:
  for line in f:
    line = line.strip()
    if not line:
      break
    name, what, zone = line.split(',')
    if name in NAMES:
      continue
    ZZ = zone.split()  # has superfluous zones
    ZZ = [z for z in ZZ if z in SUPPORTED] + [z for z in ZZ if z not in SUPPORTED]
    zone = ZZ[0]
    rest = ZZ[1:]
    NAMES.add(name)
    ZONES.add(ZZ[0])
    SUPERFLUOUS |= set(rest)
    os.environ['TZ'] = zone
    p = subprocess.Popen(['date', '+%z@%Z'], stdout=subprocess.PIPE)
    z, Z = p.stdout.read().decode('utf-8').strip().split('@')
    print("%-35s %-5s %-10s %-10s %-30s" % (name, what, z, Z, zone))
    if zone in SUPPORTED:
      TABLE1.append((name, zone, z, Z))
    else:
      TABLE2.append((name, zone, z, Z))

print()
TABLE1.sort()
TABLE2.sort()
for k, v, z, Z in TABLE1:
  print('{"%s", "%s"}, // %s %s' % (k, v, z, Z))
print('#ifdef EMBED_EVERY_TIME_ZONE')
for k, v, z, Z in TABLE2:
  print('{"%s", "%s"}, // %s %s' % (k, v, z, Z))
print('#endif')

# print()
# TABLE.sort(key=lambda x: (int(x[2]), x[1]))
# for k, v, z, Z in TABLE:
#   if re.search(r'[A-Z]', Z):
#     Z = ' ' + Z
#   else:
#     Z = ''
#   print('__static_yoink("usr/share/zoneinfo/%s"); // %s%s (%s)' % (v, z, Z, k))
# print('#ifdef EMBED_EVERY_TIME_ZONE')
# print('#endif')

# print()
# SUPERFLUOUS -= ZONES
# for z in SUPERFLUOUS:
#   print(z)
