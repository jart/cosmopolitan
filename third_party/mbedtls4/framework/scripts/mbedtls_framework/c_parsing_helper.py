"""Helper functions to parse C code in heavily constrained scenarios.

Currently supported functionality:

* read_function_declarations: read function declarations from a header file.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

### WARNING: the code in this file has not been extensively reviewed yet.
### We do not think it is harmful, but it may be below our normal standards
### for robustness and maintainability.

import re
from typing import Dict, Iterable, Iterator, List, Optional, Tuple


class ArgumentInfo:
    """Information about an argument to an API function."""
    #pylint: disable=too-few-public-methods

    _KEYWORDS = [
        'const', 'register', 'restrict',
        'int', 'long', 'short', 'signed', 'unsigned',
    ]
    _DECLARATION_RE = re.compile(
        r'(?P<type>\w[\w\s*]*?)\s*' +
        r'(?!(?:' + r'|'.join(_KEYWORDS) + r'))(?P<name>\b\w+\b)?' +
        r'\s*(?P<suffix>\[[^][]*\])?\Z',
        re.A | re.S)

    @classmethod
    def normalize_type(cls, typ: str) -> str:
        """Normalize whitespace in a type."""
        typ = re.sub(r'\s+', r' ', typ)
        typ = re.sub(r'\s*\*', r' *', typ)
        return typ

    def __init__(self, decl: str) -> None:
        self.decl = decl.strip()
        m = self._DECLARATION_RE.match(self.decl)
        if not m:
            raise ValueError(self.decl)
        self.type = self.normalize_type(m.group('type')) #type: str
        self.name = m.group('name') #type: Optional[str]
        self.suffix = m.group('suffix') if m.group('suffix') else '' #type: str

    def __str__(self) -> str:
        return self.decl

class FunctionInfo:
    """Information about an API function."""
    #pylint: disable=too-few-public-methods

    # Regex matching the declaration of a function that returns void.
    VOID_RE = re.compile(r'\s*\bvoid\s*\Z', re.A)

    def __init__(self, #pylint: disable=too-many-arguments
                 filename: str,
                 line_number: int,
                 qualifiers: Iterable[str],
                 return_type: str,
                 name: str,
                 arguments: List[str],
                 doc: str = "") -> None:

        self.filename = filename
        self.line_number = line_number
        self.qualifiers = frozenset(qualifiers)
        self.return_type = return_type
        self.name = name
        self.arguments = [ArgumentInfo(arg) for arg in arguments]
        self.doc = doc

    def returns_void(self) -> bool:
        """Whether the function returns void."""
        return bool(self.VOID_RE.search(self.return_type))

    def __str__(self) -> str:
        str_args = [str(a) for a in self.arguments]
        str_text = "{} {} {}({})".format(" ".join(self.qualifiers),
                                         self.return_type, self.name,
                                         ", ".join(str_args)).strip()
        str_text = self._c_wrap_(str_text)
        return self.doc + "\n" + str_text

    @staticmethod
    def _c_wrap_(in_str: str, line_len: int = 80) -> str:
        """Auto-idents function declaration args using opening parenthesis."""
        if len(in_str) >= line_len:
            p_idx = in_str.index("(")
            ident = " "  * p_idx
            padded_comma = ",\n" + ident
            in_str = in_str.replace(",", padded_comma)
        return in_str

# Match one C comment.
# Note that we match both comment types, so things like // in a /*...*/
# comment are handled correctly.
_C_COMMENT_RE = re.compile(r'//(?:[^\n]|\\\n)*|/\*.*?\*/', re.S)
_NOT_NEWLINES_RE = re.compile(r'[^\n]+')

def read_logical_lines(filename: str) -> Iterator[Tuple[int, str]]:
    """Read logical lines from a file.

    Logical lines are one or more physical line, with balanced parentheses.
    """
    with open(filename, encoding='utf-8') as inp:
        content = inp.read()
    # Strip comments, but keep newlines for line numbering
    content = re.sub(_C_COMMENT_RE,
                     lambda m: re.sub(_NOT_NEWLINES_RE, "", m.group(0)),
                     content)
    lines = enumerate(content.splitlines(), 1)
    for line_number, line in lines:
        # Read a logical line, containing balanced parentheses.
        # We assume that parentheses are balanced (this should be ok
        # since comments have been stripped), otherwise there will be
        # a gigantic logical line at the end.
        paren_level = line.count('(') - line.count(')')
        while paren_level > 0:
            _, more = next(lines) #pylint: disable=stop-iteration-return
            paren_level += more.count('(') - more.count(')')
            line += '\n' + more
        yield line_number, line

_C_FUNCTION_DECLARATION_RE = re.compile(
    r'(?P<qualifiers>(?:(?:extern|inline|static)\b\s*)*)'
    r'(?P<return_type>\w[\w\s*]*?)\s*' +
    r'\b(?P<name>\w+)' +
    r'\s*\((?P<arguments>.*)\)\s*;',
    re.A | re.S)

def read_function_declarations(functions: Dict[str, FunctionInfo],
                               filename: str) -> None:

    """Collect function declarations from a C header file."""
    for line_number, line in read_logical_lines(filename):
        m = _C_FUNCTION_DECLARATION_RE.match(line)
        if not m:
            continue
        qualifiers = m.group('qualifiers').split()
        return_type = m.group('return_type')
        name = m.group('name')
        arguments = m.group('arguments').split(',')
        if len(arguments) == 1 and re.match(FunctionInfo.VOID_RE, arguments[0]):
            arguments = []
        # Note: we replace any existing declaration for the same name.
        functions[name] = FunctionInfo(filename, line_number,
                                       qualifiers,
                                       return_type,
                                       name,
                                       arguments)

_C_TYPEDEF_DECLARATION_RE = re.compile(r'typedef (?:struct )?(?P<type>\w+) (?P<name>\w+)')

def read_typedefs(filename: str) -> Dict[str, str]:
    """ Extract type definitions in a {typedef aliased name: original type} dictionary.
    Multi-line typedef struct are not captured. """

    type_decl = {}

    for _, line in read_logical_lines(filename):
        m = _C_TYPEDEF_DECLARATION_RE.match(line)
        if m:
            type_decl[m.group("name")] = m.group("type")
    return type_decl
