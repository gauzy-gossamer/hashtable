from setuptools import setup, Extension


setup(
    name = "hashtable",
    ext_modules = [
        Extension("hashtable", sources=["hashtable.pyx"])
    ]
)
