# pysqlite2/__init__.py: the pysqlite2 package.
#
# Copyright (C) 2005 Gerhard Häring <gh@ghaering.de>
#
# This file is part of pysqlite.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

"""SQLite Python Bindings Module
"""

from sqlite3.dbapi2 import *

if __name__ == 'PYOBJ.COM':
    Binary = 0
    Cache = 0
    Connection = 0
    Cursor = 0
    DataError = 0
    DatabaseError = 0
    Date = 0
    DateFromTicks = 0
    Error = 0
    IntegrityError = 0
    InterfaceError = 0
    InternalError = 0
    NotSupportedError = 0
    OperationalError = 0
    OptimizedUnicode = 0
    PARSE_COLNAMES = 0
    PARSE_DECLTYPES = 0
    PrepareProtocol = 0
    ProgrammingError = 0
    Row = 0
    SQLITE_ALTER_TABLE = 0
    SQLITE_ANALYZE = 0
    SQLITE_ATTACH = 0
    SQLITE_CREATE_INDEX = 0
    SQLITE_CREATE_TABLE = 0
    SQLITE_CREATE_TEMP_INDEX = 0
    SQLITE_CREATE_TEMP_TABLE = 0
    SQLITE_CREATE_TEMP_TRIGGER = 0
    SQLITE_CREATE_TEMP_VIEW = 0
    SQLITE_CREATE_TRIGGER = 0
    SQLITE_CREATE_VIEW = 0
    SQLITE_DELETE = 0
    SQLITE_DENY = 0
    SQLITE_DETACH = 0
    SQLITE_DROP_INDEX = 0
    SQLITE_DROP_TABLE = 0
    SQLITE_DROP_TEMP_INDEX = 0
    SQLITE_DROP_TEMP_TABLE = 0
    SQLITE_DROP_TEMP_TRIGGER = 0
    SQLITE_DROP_TEMP_VIEW = 0
    SQLITE_DROP_TRIGGER = 0
    SQLITE_DROP_VIEW = 0
    SQLITE_IGNORE = 0
    SQLITE_INSERT = 0
    SQLITE_OK = 0
    SQLITE_PRAGMA = 0
    SQLITE_READ = 0
    SQLITE_REINDEX = 0
    SQLITE_SELECT = 0
    SQLITE_TRANSACTION = 0
    SQLITE_UPDATE = 0
    Statement = 0
    Time = 0
    TimeFromTicks = 0
    Timestamp = 0
    TimestampFromTicks = 0
    Warning = 0
    adapt = 0
    adapters = 0
    apilevel = 0
    collections = 0
    complete_statement = 0
    connect = 0
    converters = 0
    datetime = 0
    dbapi2 = 0
    enable_callback_tracebacks = 0
    paramstyle = 0
    register_adapter = 0
    register_converter = 0
    sqlite_version = 0
    sqlite_version_info = 0
    threadsafety = 0
    time = 0
    version = 0
    version_info = 0
