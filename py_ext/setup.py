#
# you can use this with the fink distro of ming.. or one installed in /usr/local.. or the result of a build in the
# parent source directory.
#

from distutils.core import setup, Extension

import os
curdir = os.getcwd()

print curdir

setup(name = "mingc", version = "0.0.3a",
      py_modules = ['ming', 'mingc'],
      ext_modules = [Extension("_mingc", ["ming_wrap.c"],
                               include_dirs=['/usr/local/include',os.path.join(os.path.join(curdir, '..'),'src')],
                               library_dirs=['/usr/local/lib','..'],
                               libraries=['ming','z'])])

