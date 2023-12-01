/* Declarations for target shuffling support.
Copyright (C) 2022-2022 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

struct dep;
struct goaldep;

void shuffle_set_mode (const char *cmdarg);
const char *shuffle_get_mode (void);
void shuffle_deps_recursive (struct dep* g);

#define shuffle_goaldeps_recursive(_g) do{              \
        shuffle_deps_recursive ((struct dep *)_g);      \
    } while(0)
