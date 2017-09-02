import os
import re
import sys
import platform
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

from shutil import copyfile
from shutil import copy

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        src = 'C:/dev/mix/build_msvc/src/py_bindings/mixal_parser_lib/Debug/py_mixal_parse.cp36-win_amd64.pyd'
        if not os.path.exists(extdir):
            os.makedirs(extdir)
        copy(src, extdir)

setup(
    name='py_mixal_parse',
    version='0.0.1',
    author='Orange Line',
    author_email='orane.line@gmail.com',
    description='MIXAL parser',
    long_description='',
    ext_modules=[CMakeExtension('py_mixal_parse')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
