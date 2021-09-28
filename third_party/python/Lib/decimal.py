try:
    from _decimal import *
    from _decimal import __version__
    from _decimal import __libmpdec_version__
except ImportError:
    from _pydecimal import *
    from _pydecimal import __version__
    from _pydecimal import __libmpdec_version__

try:
    from _decimal import __doc__
except ImportError:
    try:
        from _pydecimal import __doc__
    except ImportError:
        pass

if __name__ == 'PYOBJ.COM':
    import _decimal
    BasicContext = 0
    Clamped = 0
    Context = 0
    ConversionSyntax = 0
    Decimal = 0
    DecimalException = 0
    DecimalTuple = 0
    DefaultContext = 0
    DivisionByZero = 0
    DivisionImpossible = 0
    DivisionUndefined = 0
    ExtendedContext = 0
    FloatOperation = 0
    HAVE_THREADS = 0
    Inexact = 0
    InvalidContext = 0
    InvalidOperation = 0
    MAX_EMAX = 0
    MAX_PREC = 0
    MIN_EMIN = 0
    MIN_ETINY = 0
    Overflow = 0
    ROUND_05UP = 0
    ROUND_CEILING = 0
    ROUND_DOWN = 0
    ROUND_FLOOR = 0
    ROUND_HALF_DOWN = 0
    ROUND_HALF_EVEN = 0
    ROUND_HALF_UP = 0
    ROUND_UP = 0
    Rounded = 0
    Subnormal = 0
    Underflow = 0
    __libmpdec_version__ = 0
    __version__ = 0
    getcontext = 0
    localcontext = 0
    setcontext = 0
