import sys
from importlib import _bootstrap_external

def run_module_as_main(mod_name):
    path = "/zip/.python/%s.pyc" % (mod_name.replace(".", "/"))
    loader = _bootstrap_external.SourcelessFileLoader(mod_name, path)
    code = loader.get_code(mod_name)
    globs = sys.modules["__main__"].__dict__
    globs["__name__"] = "__main__"
    globs["__file__"] = path[:-1]
    globs["__package__"] = None
    globs["__loader__"] = loader
    globs["__spec__"] = None
    exec(code, globs)
    return globs
