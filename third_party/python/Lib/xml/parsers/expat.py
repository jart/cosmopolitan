"""Interface to the Expat non-validating XML parser."""

__all__ = [
    'EXPAT_VERSION',
    'ErrorString',
    'ExpatError',
    'ParserCreate',
    'XMLParserType',
    'XML_PARAM_ENTITY_PARSING_ALWAYS',
    'XML_PARAM_ENTITY_PARSING_NEVER',
    'XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE',
    'error',
    'errors',
    'expat_CAPI',
    'features',
    'model',
    'native_encoding',
    'sys',
    'version_info',
]

import sys

from pyexpat import EXPAT_VERSION, ErrorString, ExpatError, ParserCreate, XMLParserType, XML_PARAM_ENTITY_PARSING_ALWAYS, XML_PARAM_ENTITY_PARSING_NEVER, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE, error, errors, expat_CAPI, features, model, native_encoding, version_info

# provide pyexpat submodules as xml.parsers.expat submodules
sys.modules['xml.parsers.expat.model'] = model
sys.modules['xml.parsers.expat.errors'] = errors
