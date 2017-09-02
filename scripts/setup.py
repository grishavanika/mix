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

class PydFileExtension(Extension):
    def __init__(self, name, pyd_file, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)
        self.pyd_file = pyd_file

class PydFileBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        if not os.path.exists(extdir):
            os.makedirs(extdir)
        copy(ext.pyd_file, extdir)

def list_all_pyds(root):
    # Assume that .pyd file has next format: `module_name.info.pyd`
    info = [(fn.split('.')[0], os.path.join(root, fn))
        for fn in os.listdir(root) if fn.endswith('.pyd')]
    return info

root_dir = os.path.abspath(os.environ['pyds_root'])
#root_dir = 'C:/dev/mix/build_msvc/deploy/bin/Debug/py/'

for module in list_all_pyds(root_dir):
    name = module[0]
    file = module[1]
    setup(
        name=name,
        version='0.0.1',
        author='Orange Line',
        author_email='orane.line@gmail.com',
        long_description='',
        ext_modules=[PydFileExtension(name, file)],
        cmdclass=dict(build_ext=PydFileBuild),
        zip_safe=False
        )


