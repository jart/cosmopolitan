"""This module tests SyntaxErrors.

Here's an example of the sort of thing that is tested.

>>> def f(x):
...     global x
Traceback (most recent call last):
SyntaxError: name 'x' is parameter and global

The tests are all raise SyntaxErrors.  They were created by checking
each C call that raises SyntaxError.  There are several modules that
raise these exceptions-- ast.c, compile.c, future.c, pythonrun.c, and
symtable.c.

The parser itself outlaws a lot of invalid syntax.  None of these
errors are tested here at the moment.  We should add some tests; since
there are infinitely many programs with invalid syntax, we would need
to be judicious in selecting some.

The compiler generates a synthetic module name for code executed by
doctest.  Since all the code comes from the same module, a suffix like
[1] is appended to the module name, As a consequence, changing the
order of tests in this module means renumbering all the errors after
it.  (Maybe we should enable the ellipsis option for these tests.)

In ast.c, syntax errors are raised by calling ast_error().

Errors from set_context():

>>> obj.None = 1
Traceback (most recent call last):
SyntaxError: invalid syntax

>>> None = 1
Traceback (most recent call last):
SyntaxError: can't assign to keyword

>>> f() = 1
Traceback (most recent call last):
SyntaxError: can't assign to function call

>>> del f()
Traceback (most recent call last):
SyntaxError: can't delete function call

>>> a + 1 = 2
Traceback (most recent call last):
SyntaxError: can't assign to operator

>>> (x for x in x) = 1
Traceback (most recent call last):
SyntaxError: can't assign to generator expression

>>> 1 = 1
Traceback (most recent call last):
SyntaxError: can't assign to literal

>>> "abc" = 1
Traceback (most recent call last):
SyntaxError: can't assign to literal

>>> b"" = 1
Traceback (most recent call last):
SyntaxError: can't assign to literal

>>> `1` = 1
Traceback (most recent call last):
SyntaxError: invalid syntax

If the left-hand side of an assignment is a list or tuple, an illegal
expression inside that contain should still cause a syntax error.
This test just checks a couple of cases rather than enumerating all of
them.

>>> (a, "b", c) = (1, 2, 3)
Traceback (most recent call last):
SyntaxError: can't assign to literal

>>> [a, b, c + 1] = [1, 2, 3]
Traceback (most recent call last):
SyntaxError: can't assign to operator

>>> a if 1 else b = 1
Traceback (most recent call last):
SyntaxError: can't assign to conditional expression

From compiler_complex_args():

>>> def f(None=1):
...     pass
Traceback (most recent call last):
SyntaxError: invalid syntax


From ast_for_arguments():

>>> def f(x, y=1, z):
...     pass
Traceback (most recent call last):
SyntaxError: non-default argument follows default argument

>>> def f(x, None):
...     pass
Traceback (most recent call last):
SyntaxError: invalid syntax

>>> def f(*None):
...     pass
Traceback (most recent call last):
SyntaxError: invalid syntax

>>> def f(**None):
...     pass
Traceback (most recent call last):
SyntaxError: invalid syntax


From ast_for_funcdef():

>>> def None(x):
...     pass
Traceback (most recent call last):
SyntaxError: invalid syntax


From ast_for_call():

>>> def f(it, *varargs):
...     return list(it)
>>> L = range(10)
>>> f(x for x in L)
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
>>> f(x for x in L, 1)
Traceback (most recent call last):
SyntaxError: Generator expression must be parenthesized if not sole argument
>>> f(x for x in L, y for y in L)
Traceback (most recent call last):
SyntaxError: Generator expression must be parenthesized if not sole argument
>>> f((x for x in L), 1)
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

>>> f(i0,  i1,  i2,  i3,  i4,  i5,  i6,  i7,  i8,  i9,  i10,  i11,
...   i12,  i13,  i14,  i15,  i16,  i17,  i18,  i19,  i20,  i21,  i22,
...   i23,  i24,  i25,  i26,  i27,  i28,  i29,  i30,  i31,  i32,  i33,
...   i34,  i35,  i36,  i37,  i38,  i39,  i40,  i41,  i42,  i43,  i44,
...   i45,  i46,  i47,  i48,  i49,  i50,  i51,  i52,  i53,  i54,  i55,
...   i56,  i57,  i58,  i59,  i60,  i61,  i62,  i63,  i64,  i65,  i66,
...   i67,  i68,  i69,  i70,  i71,  i72,  i73,  i74,  i75,  i76,  i77,
...   i78,  i79,  i80,  i81,  i82,  i83,  i84,  i85,  i86,  i87,  i88,
...   i89,  i90,  i91,  i92,  i93,  i94,  i95,  i96,  i97,  i98,  i99,
...   i100,  i101,  i102,  i103,  i104,  i105,  i106,  i107,  i108,
...   i109,  i110,  i111,  i112,  i113,  i114,  i115,  i116,  i117,
...   i118,  i119,  i120,  i121,  i122,  i123,  i124,  i125,  i126,
...   i127,  i128,  i129,  i130,  i131,  i132,  i133,  i134,  i135,
...   i136,  i137,  i138,  i139,  i140,  i141,  i142,  i143,  i144,
...   i145,  i146,  i147,  i148,  i149,  i150,  i151,  i152,  i153,
...   i154,  i155,  i156,  i157,  i158,  i159,  i160,  i161,  i162,
...   i163,  i164,  i165,  i166,  i167,  i168,  i169,  i170,  i171,
...   i172,  i173,  i174,  i175,  i176,  i177,  i178,  i179,  i180,
...   i181,  i182,  i183,  i184,  i185,  i186,  i187,  i188,  i189,
...   i190,  i191,  i192,  i193,  i194,  i195,  i196,  i197,  i198,
...   i199,  i200,  i201,  i202,  i203,  i204,  i205,  i206,  i207,
...   i208,  i209,  i210,  i211,  i212,  i213,  i214,  i215,  i216,
...   i217,  i218,  i219,  i220,  i221,  i222,  i223,  i224,  i225,
...   i226,  i227,  i228,  i229,  i230,  i231,  i232,  i233,  i234,
...   i235,  i236,  i237,  i238,  i239,  i240,  i241,  i242,  i243,
...   i244,  i245,  i246,  i247,  i248,  i249,  i250,  i251,  i252,
...   i253,  i254,  i255)
Traceback (most recent call last):
SyntaxError: more than 255 arguments

The actual error cases counts positional arguments, keyword arguments,
and generator expression arguments separately.  This test combines the
three.

>>> f(i0,  i1,  i2,  i3,  i4,  i5,  i6,  i7,  i8,  i9,  i10,  i11,
...   i12,  i13,  i14,  i15,  i16,  i17,  i18,  i19,  i20,  i21,  i22,
...   i23,  i24,  i25,  i26,  i27,  i28,  i29,  i30,  i31,  i32,  i33,
...   i34,  i35,  i36,  i37,  i38,  i39,  i40,  i41,  i42,  i43,  i44,
...   i45,  i46,  i47,  i48,  i49,  i50,  i51,  i52,  i53,  i54,  i55,
...   i56,  i57,  i58,  i59,  i60,  i61,  i62,  i63,  i64,  i65,  i66,
...   i67,  i68,  i69,  i70,  i71,  i72,  i73,  i74,  i75,  i76,  i77,
...   i78,  i79,  i80,  i81,  i82,  i83,  i84,  i85,  i86,  i87,  i88,
...   i89,  i90,  i91,  i92,  i93,  i94,  i95,  i96,  i97,  i98,  i99,
...   i100,  i101,  i102,  i103,  i104,  i105,  i106,  i107,  i108,
...   i109,  i110,  i111,  i112,  i113,  i114,  i115,  i116,  i117,
...   i118,  i119,  i120,  i121,  i122,  i123,  i124,  i125,  i126,
...   i127,  i128,  i129,  i130,  i131,  i132,  i133,  i134,  i135,
...   i136,  i137,  i138,  i139,  i140,  i141,  i142,  i143,  i144,
...   i145,  i146,  i147,  i148,  i149,  i150,  i151,  i152,  i153,
...   i154,  i155,  i156,  i157,  i158,  i159,  i160,  i161,  i162,
...   i163,  i164,  i165,  i166,  i167,  i168,  i169,  i170,  i171,
...   i172,  i173,  i174,  i175,  i176,  i177,  i178,  i179,  i180,
...   i181,  i182,  i183,  i184,  i185,  i186,  i187,  i188,  i189,
...   i190,  i191,  i192,  i193,  i194,  i195,  i196,  i197,  i198,
...   i199,  i200,  i201,  i202,  i203,  i204,  i205,  i206,  i207,
...   i208,  i209,  i210,  i211,  i212,  i213,  i214,  i215,  i216,
...   i217,  i218,  i219,  i220,  i221,  i222,  i223,  i224,  i225,
...   i226,  i227,  i228,  i229,  i230,  i231,  i232,  i233,  i234,
...   i235, i236,  i237,  i238,  i239,  i240,  i241,  i242,  i243,
...   (x for x in i244),  i245,  i246,  i247,  i248,  i249,  i250,  i251,
...    i252=1, i253=1,  i254=1,  i255=1)
Traceback (most recent call last):
SyntaxError: more than 255 arguments

>>> class C:
...     def meth(self, *args):
...         return args
>>> obj = C()
>>> obj.meth(
...   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
...   20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
...   38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
...   56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
...   74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
...   92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
...   108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
...   122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
...   136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
...   150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163,
...   164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177,
...   178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
...   192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
...   206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
...   220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
...   234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247,
...   248, 249, 250, 251, 252, 253, 254)  # doctest: +ELLIPSIS
(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, ..., 252, 253, 254)

>>> f(lambda x: x[0] = 3)
Traceback (most recent call last):
SyntaxError: lambda cannot contain assignment

The grammar accepts any test (basically, any expression) in the
keyword slot of a call site.  Test a few different options.

>>> f(x()=2)
Traceback (most recent call last):
SyntaxError: keyword can't be an expression
>>> f(a or b=1)
Traceback (most recent call last):
SyntaxError: keyword can't be an expression
>>> f(x.y=1)
Traceback (most recent call last):
SyntaxError: keyword can't be an expression


More set_context():

>>> (x for x in x) += 1
Traceback (most recent call last):
SyntaxError: can't assign to generator expression
>>> None += 1
Traceback (most recent call last):
SyntaxError: can't assign to keyword
>>> f() += 1
Traceback (most recent call last):
SyntaxError: can't assign to function call


Test continue in finally in weird combinations.

continue in for loop under finally should be ok.

    >>> def test():
    ...     try:
    ...         pass
    ...     finally:
    ...         for abc in range(10):
    ...             continue
    ...     print(abc)
    >>> test()
    9

Start simple, a continue in a finally should not be allowed.

    >>> def test():
    ...    for abc in range(10):
    ...        try:
    ...            pass
    ...        finally:
    ...            continue
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

This is essentially a continue in a finally which should not be allowed.

    >>> def test():
    ...    for abc in range(10):
    ...        try:
    ...            pass
    ...        finally:
    ...            try:
    ...                continue
    ...            except:
    ...                pass
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

    >>> def foo():
    ...     try:
    ...         pass
    ...     finally:
    ...         continue
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

    >>> def foo():
    ...     for a in ():
    ...       try:
    ...           pass
    ...       finally:
    ...           continue
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

    >>> def foo():
    ...     for a in ():
    ...         try:
    ...             pass
    ...         finally:
    ...             try:
    ...                 continue
    ...             finally:
    ...                 pass
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

    >>> def foo():
    ...  for a in ():
    ...   try: pass
    ...   finally:
    ...    try:
    ...     pass
    ...    except:
    ...     continue
    Traceback (most recent call last):
      ...
    SyntaxError: 'continue' not supported inside 'finally' clause

There is one test for a break that is not in a loop.  The compiler
uses a single data structure to keep track of try-finally and loops,
so we need to be sure that a break is actually inside a loop.  If it
isn't, there should be a syntax error.

   >>> try:
   ...     print(1)
   ...     break
   ...     print(2)
   ... finally:
   ...     print(3)
   Traceback (most recent call last):
     ...
   SyntaxError: 'break' outside loop

This raises a SyntaxError, it used to raise a SystemError.
Context for this change can be found on issue #27514

In 2.5 there was a missing exception and an assert was triggered in a debug
build.  The number of blocks must be greater than CO_MAXBLOCKS.  SF #1565514

   >>> while 1:
   ...  while 2:
   ...   while 3:
   ...    while 4:
   ...     while 5:
   ...      while 6:
   ...       while 8:
   ...        while 9:
   ...         while 10:
   ...          while 11:
   ...           while 12:
   ...            while 13:
   ...             while 14:
   ...              while 15:
   ...               while 16:
   ...                while 17:
   ...                 while 18:
   ...                  while 19:
   ...                   while 20:
   ...                    while 21:
   ...                     while 22:
   ...                      break
   Traceback (most recent call last):
     ...
   SyntaxError: too many statically nested blocks

Misuse of the nonlocal and global statement can lead to a few unique syntax errors.

   >>> def f():
   ...     x = 1
   ...     global x
   Traceback (most recent call last):
     ...
   SyntaxError: name 'x' is assigned to before global declaration

   >>> def f():
   ...     x = 1
   ...     def g():
   ...         print(x)
   ...         nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: name 'x' is used prior to nonlocal declaration

   >>> def f():
   ...     x = 1
   ...     def g():
   ...         x = 2
   ...         nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: name 'x' is assigned to before nonlocal declaration

   >>> def f(x):
   ...     nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: name 'x' is parameter and nonlocal

   >>> def f():
   ...     global x
   ...     nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: name 'x' is nonlocal and global

   >>> def f():
   ...     nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: no binding for nonlocal 'x' found

From SF bug #1705365
   >>> nonlocal x
   Traceback (most recent call last):
     ...
   SyntaxError: nonlocal declaration not allowed at module level

From https://bugs.python.org/issue25973
   >>> class A:
   ...     def f(self):
   ...         nonlocal __x
   Traceback (most recent call last):
     ...
   SyntaxError: no binding for nonlocal '_A__x' found


This tests assignment-context; there was a bug in Python 2.5 where compiling
a complex 'if' (one with 'elif') would fail to notice an invalid suite,
leading to spurious errors.

   >>> if 1:
   ...   x() = 1
   ... elif 1:
   ...   pass
   Traceback (most recent call last):
     ...
   SyntaxError: can't assign to function call

   >>> if 1:
   ...   pass
   ... elif 1:
   ...   x() = 1
   Traceback (most recent call last):
     ...
   SyntaxError: can't assign to function call

   >>> if 1:
   ...   x() = 1
   ... elif 1:
   ...   pass
   ... else:
   ...   pass
   Traceback (most recent call last):
     ...
   SyntaxError: can't assign to function call

   >>> if 1:
   ...   pass
   ... elif 1:
   ...   x() = 1
   ... else:
   ...   pass
   Traceback (most recent call last):
     ...
   SyntaxError: can't assign to function call

   >>> if 1:
   ...   pass
   ... elif 1:
   ...   pass
   ... else:
   ...   x() = 1
   Traceback (most recent call last):
     ...
   SyntaxError: can't assign to function call

Make sure that the old "raise X, Y[, Z]" form is gone:
   >>> raise X, Y
   Traceback (most recent call last):
     ...
   SyntaxError: invalid syntax
   >>> raise X, Y, Z
   Traceback (most recent call last):
     ...
   SyntaxError: invalid syntax


>>> f(a=23, a=234)
Traceback (most recent call last):
   ...
SyntaxError: keyword argument repeated

>>> {1, 2, 3} = 42
Traceback (most recent call last):
SyntaxError: can't assign to literal

Corner-cases that used to fail to raise the correct error:

    >>> def f(*, x=lambda __debug__:0): pass
    Traceback (most recent call last):
    SyntaxError: assignment to keyword

    >>> def f(*args:(lambda __debug__:0)): pass
    Traceback (most recent call last):
    SyntaxError: assignment to keyword

    >>> def f(**kwargs:(lambda __debug__:0)): pass
    Traceback (most recent call last):
    SyntaxError: assignment to keyword

    >>> with (lambda *:0): pass
    Traceback (most recent call last):
    SyntaxError: named arguments must follow bare *

Corner-cases that used to crash:

    >>> def f(**__debug__): pass
    Traceback (most recent call last):
    SyntaxError: assignment to keyword

    >>> def f(*xx, __debug__): pass
    Traceback (most recent call last):
    SyntaxError: assignment to keyword

"""

import re
import unittest
import warnings

from test import support

class SyntaxTestCase(unittest.TestCase):

    def _check_error(self, code, errtext,
                     filename="<testcase>", mode="exec", subclass=None, lineno=None, offset=None):
        """Check that compiling code raises SyntaxError with errtext.

        errtest is a regular expression that must be present in the
        test of the exception raised.  If subclass is specified it
        is the expected subclass of SyntaxError (e.g. IndentationError).
        """
        try:
            compile(code, filename, mode)
        except SyntaxError as err:
            if subclass and not isinstance(err, subclass):
                self.fail("SyntaxError is not a %s" % subclass.__name__)
            mo = re.search(errtext, str(err))
            if mo is None:
                self.fail("SyntaxError did not contain '%r'" % (errtext,))
            self.assertEqual(err.filename, filename)
            if lineno is not None:
                self.assertEqual(err.lineno, lineno)
            if offset is not None:
                self.assertEqual(err.offset, offset)
        else:
            self.fail("compile() did not raise SyntaxError")

    def test_assign_call(self):
        self._check_error("f() = 1", "assign")

    def test_assign_del(self):
        self._check_error("del f()", "delete")

    def test_global_err_then_warn(self):
        # Bug #763201:  The SyntaxError raised for one global statement
        # shouldn't be clobbered by a SyntaxWarning issued for a later one.
        source = """if 1:
            def error(a):
                global a  # SyntaxError
            def warning():
                b = 1
                global b  # SyntaxWarning
            """
        warnings.filterwarnings(action='ignore', category=SyntaxWarning)
        self._check_error(source, "global")
        warnings.filters.pop(0)

    def test_break_outside_loop(self):
        self._check_error("break", "outside loop")

    def test_unexpected_indent(self):
        self._check_error("foo()\n bar()\n", "unexpected indent",
                          subclass=IndentationError)

    def test_no_indent(self):
        self._check_error("if 1:\nfoo()", "expected an indented block",
                          subclass=IndentationError)

    def test_bad_outdent(self):
        self._check_error("if 1:\n  foo()\n bar()",
                          "unindent does not match .* level",
                          subclass=IndentationError)

    def test_kwargs_last(self):
        self._check_error("int(base=10, '2')",
                          "positional argument follows keyword argument")

    def test_kwargs_last2(self):
        self._check_error("int(**{'base': 10}, '2')",
                          "positional argument follows "
                          "keyword argument unpacking")

    def test_kwargs_last3(self):
        self._check_error("int(**{'base': 10}, *['2'])",
                          "iterable argument unpacking follows "
                          "keyword argument unpacking")

def test_main():
    support.run_unittest(SyntaxTestCase)
    from test import test_syntax
    support.run_doctest(test_syntax, verbosity=True)

if __name__ == "__main__":
    test_main()
