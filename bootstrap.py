#!/usr/bin/env python

import os
import sys
import fnmatch

import argparse
import itertools

import ninja_syntax

objdir = 'obj'

def flags(*args):
    return ' '.join(itertools.chain(*args))

def object_file(f):
    (root, ext) = os.path.splitext(f)
    return os.path.join(objdir, root + '.o')

def generate_files_from(directory, glob):
    for root, directories, files in os.walk(directory):
        for f in files:
            if fnmatch.fnmatch(f, glob):
                yield os.path.join(root, f)

def files_from(directory, glob):
    return list(generate_files_from(directory, glob))

# command line stuff
parser = argparse.ArgumentParser(usage='%(prog)s [options...]')
parser.add_argument('--debug', action='store_true', help='compile with debug flags')
parser.add_argument('--ci', action='store_true', help=argparse.SUPPRESS)
parser.add_argument('--cxx', metavar='<compiler>', help='compiler name to use (default: gcc)', default='gcc')
parser.add_argument('--luajit', action='store_true', help='link luajit instead of regular lua')
args = parser.parse_args()

BUILD_FILENAME = 'build.ninja'

compiler = args.cxx
include = ['-Iinclude', '-I./ext/greatest']
depends = []
libdirs = []
ldflags = ['-lm']
cxxflags = ['-Wall', '-Wextra', '-pedantic', '-pedantic-errors', '-std=c11']

if sys.platform == 'win32':
    project.libraries = ['mingw32']

if args.ci:
    ldflags.extend(['-llua5.2'])
    include.extend(['-I/usr/include/lua5.2', '-I./lua-5.2.2/src'])
else:
    if args.luajit:
        include.extend(['-I/usr/include/luajit-2.0'])
        ldflags.extend(['-lluajit-5.1'])
    else:
        ldflags.extend(['-llua'])

def warning(string):
    print('warning: {}'.format(string))

# # configuration
# if 'g++' not in args.cxx:
#     warning('compiler not explicitly supported: {}'.format(args.cxx))

if args.debug:
    cxxflags.extend(['-g', '-O0', '-DDEBUG'])
else:
    cxxflags.extend(['-DNDEBUG', '-O3'])

if args.cxx == 'clang++':
    cxxflags.extend(['-Wno-constexpr-not-const', '-Wno-unused-value', '-Wno-mismatched-tags'])

### Build our ninja file
ninja = ninja_syntax.Writer(open('build.ninja', 'w'))

# Variables
ninja.variable('ninja_required_version', '1.3')
ninja.variable('ar', 'ar')
ninja.variable('cxx', compiler)
ninja.variable('cxxflags', flags(cxxflags + include + libdirs + depends))
ninja.variable('ldflags', flags(ldflags))
ninja.newline()

# Rules
ninja.rule('bootstrap', command = ' '.join(['python'] + sys.argv), generator = True)
ninja.rule('compile', command = '$cxx -fpic -MMD -MF $out.d -c $cxxflags -Werror $in -o $out',
deps = 'gcc', depfile = '$out.d',
description = 'Compiling $in to $out')
ninja.rule('link', command = '$cxx $cxxflags $in -o $out $ldflags', description = 'Creating $out')
ninja.rule('dynlink', command = '$cxx -shared $cxxflags $in -o $out $ldflags', description = 'Creating $out')
ninja.rule('ar', command = 'rm -f $out && $ar crs $out $in', description = 'AR $out')
ninja.newline()

# Builds

ninja.build('build.ninja', 'bootstrap', implicit = 'bootstrap.py')

libObjs = []
for f in files_from('src/', '**.c'):
    obj = object_file(f)
    libObjs.append(obj)
    ninja.build(obj, 'compile', inputs = f)

unitTestObjs = []
for f in files_from('test/src/unit', '*.c'):
    obj = object_file(f)
    unitTestObjs.append(obj)
    ninja.build(obj, 'compile', inputs = f)

scriptTestObjs = []
for f in files_from('test/src/wrap', '*.c'):
    obj = object_file(f)
    scriptTestObjs.append(obj)
    ninja.build(obj, 'compile', inputs = f)

ninja.newline()

ninja.build('./lib/libbarrage.so', 'dynlink', inputs = libObjs)
ninja.newline()
ninja.build('./test/bin/btest', 'link', inputs = libObjs + unitTestObjs)
ninja.newline()
ninja.build('./test/bin/scriptTest', 'link', inputs = libObjs + scriptTestObjs)
