from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

import numpy

ext = Extension("hashtable", ["hashtable.pyx"],
    include_dirs = [numpy.get_include()])

setup(
    name = "hashtable",
    ext_modules = cythonize('*.pyx'),
    include_dirs = [numpy.get_include()]
)
#cythonize('*.pyx')
