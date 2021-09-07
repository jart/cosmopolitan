#
# Package analogous to 'threading.py' but using processes
#
# multiprocessing/__init__.py
#
# This package is intended to duplicate the functionality (and much of
# the API) of threading.py but uses processes instead of threads.  A
# subpackage 'multiprocessing.dummy' has the same API but is a simple
# wrapper for 'threading'.
#
# Copyright (c) 2006-2008, R Oudkerk
# Licensed to PSF under a Contributor Agreement.
#

import sys
from . import context

#
# Copy stuff from default context
#

Array = context._default_context.Array
AuthenticationError = context._default_context.AuthenticationError
Barrier = context._default_context.Barrier
BoundedSemaphore = context._default_context.BoundedSemaphore
BufferTooShort = context._default_context.BufferTooShort
Condition = context._default_context.Condition
Event = context._default_context.Event
JoinableQueue = context._default_context.JoinableQueue
Lock = context._default_context.Lock
Manager = context._default_context.Manager
Pipe = context._default_context.Pipe
Pool = context._default_context.Pool
Process = context._default_context.Process
ProcessError = context._default_context.ProcessError
Queue = context._default_context.Queue
RLock = context._default_context.RLock
RawArray = context._default_context.RawArray
RawValue = context._default_context.RawValue
Semaphore = context._default_context.Semaphore
SimpleQueue = context._default_context.SimpleQueue
TimeoutError = context._default_context.TimeoutError
Value = context._default_context.Value
active_children = context._default_context.active_children
allow_connection_pickling = context._default_context.allow_connection_pickling
cpu_count = context._default_context.cpu_count
current_process = context._default_context.current_process
freeze_support = context._default_context.freeze_support
get_all_start_methods = context._default_context.get_all_start_methods
get_context = context._default_context.get_context
get_logger = context._default_context.get_logger
get_start_method = context._default_context.get_start_method
log_to_stderr = context._default_context.log_to_stderr
reducer = context._default_context.reducer
set_executable = context._default_context.set_executable
set_forkserver_preload = context._default_context.set_forkserver_preload
set_start_method = context._default_context.set_start_method

#
# XXX These should not really be documented or public.
#

SUBDEBUG = 5
SUBWARNING = 25

#
# Alias for main module -- will be reset by bootstrapping child processes
#

if '__main__' in sys.modules:
    sys.modules['__mp_main__'] = sys.modules['__main__']
