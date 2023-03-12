"""The asyncio package, tracking PEP 3156."""

import sys

# The selectors module is in the stdlib in Python 3.4 but not in 3.3.
# Do this first, so the other submodules can use "from . import selectors".
# Prefer asyncio/selectors.py over the stdlib one, as ours may be newer.
import selectors  # Will also be exported.


# This relies on each of the submodules having an __all__ variable.
from .base_events import *
from .coroutines import *
from .events import *
from .futures import *
from .locks import *
from .protocols import *
from .queues import *
from .streams import *
from .subprocess import *
from .tasks import *
from .transports import *

__all__ = (base_events.__all__ +
           coroutines.__all__ +
           events.__all__ +
           futures.__all__ +
           locks.__all__ +
           protocols.__all__ +
           queues.__all__ +
           streams.__all__ +
           subprocess.__all__ +
           tasks.__all__ +
           transports.__all__)

from .unix_events import *  # pragma: no cover
__all__ += unix_events.__all__
