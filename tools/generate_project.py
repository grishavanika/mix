import subprocess
from pprint import pprint
from pathlib import Path
import os
from enum import Enum
import argparse

def quote(str):
	return '"' + str + '"'

def quote_spaces(str):
	if ' ' in str:
		return quote(str)
	else:
		return str

def to_cmd_line(args):
	return ' '.join([quote_spaces(arg) for arg in args])

class Compiler(Enum):
	CL = 1
	CLANG = 2
	GCC = 3

class Platform(Enum):
	X64 = 1
	X32 = 2

def enum_str(enum):
	return enum.name.lower()

def enum_from_string(enum, str):
	for c in enum:
		if enum_str(c) == str:
			return c
	raise Exception('{} is not valid {}'.format(str, str(enum)))

def enum_cmd_values(enum):
	return [enum_str(c) for c in enum]

class Configuration:
	def __init__(self, compiler = Compiler.CLANG
		, platform = Platform.X64, vcpkg_path = None
		, install_to = None):
		self.compiler = compiler
		self.platform = platform
		self.vcpkg_path = vcpkg_path
		self.install_to = install_to

	def build_folder_name(self):
		return 'build_{}_{}'.format(enum_str(self.compiler), enum_str(self.platform))

def cmake_win_generator_args(config):
	# TODO: more intelligent configuration

	args = []
	if config.install_to:
		args += ['-DCMAKE_INSTALL_PREFIX={}'.format(config.install_to)]

	if config.compiler == Compiler.GCC:
		args += ['-G', 'MinGW Makefiles']
	else:
		args += ['-G', 'Visual Studio 16 2019']
		if config.platform == Platform.X64:
			args += ['-A', 'x64']
		if config.compiler == Compiler.CLANG:
			args += ['-T', 'LLVM']
		if config.vcpkg_path:
			args += ['-DCMAKE_TOOLCHAIN_FILE={}'.format(config.vcpkg_path)]

	return args

def cmake_generator_args(config):
	if os.name == 'nt':
		return cmake_win_generator_args(config)
	# TODO: switch from GCC to CLANG:
	# export CC=/usr/bin/clang-5.0
	# export CXX=/usr/bin/clang-5.0
	return []

def find_parent_cmake_folder():
	cmake = []
	path = Path.cwd()
	while not cmake:
		path = path.parent
		cmake = [f.name for f in path.iterdir() if f.name == 'CMakeLists.txt']
	return path

def invoke_cmake(args):
	full_cmd = ['cmake'] + args
	print('Invoking CMake:')
	print(to_cmd_line(full_cmd))
	return subprocess.call(full_cmd)

def cmake_generate(config, location, args):
	return invoke_cmake(cmake_generator_args(config)
		+ args + [str(location)])

def make_build_folder(project_folder, config):
	path = project_folder / config.build_folder_name()
	if not path.exists():
		path.mkdir()
	return path

def cmd_line_parser():
	parser = argparse.ArgumentParser(description = 'Generate the project with CMake')
	parser.add_argument('--compiler'
		, choices = enum_cmd_values(Compiler), default = enum_str(Compiler.CLANG))
	parser.add_argument('--platform'
		, choices = enum_cmd_values(Platform), default = enum_str(Platform.X64))
	parser.add_argument('--vcpkg')
	parser.add_argument('--install', default = 'deploy')
	return parser

def configuration_from_args(args):
	return Configuration(
		enum_from_string(Compiler, args.compiler),
		enum_from_string(Platform, args.platform),
		vcpkg_path = args.vcpkg,
		install_to = args.install)

def do_main():
	args = cmd_line_parser().parse_args()
	config = configuration_from_args(args)
	project_folder = find_parent_cmake_folder()
	build_folder = make_build_folder(project_folder, config)
	print('Generating project into {} folder'.format(build_folder))
	os.chdir(str(build_folder))
	cmake_generate(config, project_folder, [])

# TODO: build - cmake --build . --config Debug

if __name__ == "__main__":
	do_main()

