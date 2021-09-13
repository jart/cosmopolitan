"""Faux ``threading`` version using ``dummy_thread`` instead of ``thread``.

The module ``_dummy_threading`` is added to ``sys.modules`` in order
to not have ``threading`` considered imported.  Had ``threading`` been
directly imported it would have made all subsequent imports succeed
regardless of whether ``_thread`` was available which is not desired.

"""
from sys import modules as sys_modules

import _dummy_thread

# Declaring now so as to not have to nest ``try``s to get proper clean-up.
holding_thread = False
holding_threading = False
holding__threading_local = False

try:
    # Could have checked if ``_thread`` was not in sys.modules and gone
    # a different route, but decided to mirror technique used with
    # ``threading`` below.
    if '_thread' in sys_modules:
        held_thread = sys_modules['_thread']
        holding_thread = True
    # Must have some module named ``_thread`` that implements its API
    # in order to initially import ``threading``.
    sys_modules['_thread'] = sys_modules['_dummy_thread']

    if 'threading' in sys_modules:
        # If ``threading`` is already imported, might as well prevent
        # trying to import it more than needed by saving it if it is
        # already imported before deleting it.
        held_threading = sys_modules['threading']
        holding_threading = True
        del sys_modules['threading']

    if '_threading_local' in sys_modules:
        # If ``_threading_local`` is already imported, might as well prevent
        # trying to import it more than needed by saving it if it is
        # already imported before deleting it.
        held__threading_local = sys_modules['_threading_local']
        holding__threading_local = True
        del sys_modules['_threading_local']

    import threading
    # Need a copy of the code kept somewhere...
    sys_modules['_dummy_threading'] = sys_modules['threading']
    del sys_modules['threading']
    sys_modules['_dummy__threading_local'] = sys_modules['_threading_local']
    del sys_modules['_threading_local']
    from _dummy_threading import *
    from _dummy_threading import __all__

finally:
    # Put back ``threading`` if we overwrote earlier

    if holding_threading:
        sys_modules['threading'] = held_threading
        del held_threading
    del holding_threading

    # Put back ``_threading_local`` if we overwrote earlier

    if holding__threading_local:
        sys_modules['_threading_local'] = held__threading_local
        del held__threading_local
    del holding__threading_local

    # Put back ``thread`` if we overwrote, else del the entry we made
    if holding_thread:
        sys_modules['_thread'] = held_thread
        del held_thread
    else:
        del sys_modules['_thread']
    del holding_thread

    del _dummy_thread
    del sys_modules

if __name__ == 'PYOBJ.COM':
    Barrier = 0
    BoundedSemaphore = 0
    BrokenBarrierError = 0
    Condition = 0
    Event = 0
    Lock = 0
    RLock = 0
    Semaphore = 0
    TIMEOUT_MAX = 0
    Thread = 0
    ThreadError = 0
    Timer = 0
    WeakSet = 0
    activeCount = 0
    active_count = 0
    currentThread = 0
    current_thread = 0
    enumerate = 0
    get_ident = 0
    local = 0
    main_thread = 0
    setprofile = 0
    settrace = 0
    stack_size = 0
