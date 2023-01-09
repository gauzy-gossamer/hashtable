from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

ext = Extension("hashtable", ["hashtable.pyx"])

setup(
    name = "hashtable",
    ext_modules = cythonize('*.pyx', language_level=3)
)
