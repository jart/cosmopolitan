"""A minimal subset of the locale module used at interpreter startup
(imported by the _io module), in order to reduce startup time.

Don't import directly from third-party code; use the `locale` module instead!
"""

import sys
import _locale

def getpreferredencoding(do_setlocale=True):
    assert not do_setlocale
    result = _locale.nl_langinfo(_locale.CODESET)
    if not result and sys.platform in ('darwin', 'cosmo'):
        # nl_langinfo can return an empty string
        # when the setting has an invalid value.
        # Default to UTF-8 in that case because
        # UTF-8 is the default charset on OSX and
        # returning nothing will crash the
        # interpreter.
        result = 'UTF-8'
    return result
