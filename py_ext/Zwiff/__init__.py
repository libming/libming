#
# A base hack to allow ming use in Zope.
#

from AccessControl import allow_module, allow_class, allow_type
from AccessControl import ModuleSecurityInfo, ClassSecurityInfo
from Globals import InitializeClass
from thread import allocate_lock

ZMingLock = allocate_lock()

allow_module('ming')

import setFlags

__allow_access_to_unprotected_subobjects__=1

import os

ZWIFF_DEBUG = os.environ.get('ZWIFF_DEBUG',0) and 1

class MingLocker:
   debug = ZWIFF_DEBUG

   def __init__(self):
      if self.debug:
          print "attempting to get lock.. "
          
      ZMingLock.acquire()
      
      if self.debug:
          print "got lock.. "

   def __del__(self):
      if self.debug:
          print "attempting to release lock.. "
   
      ZMingLock.release()

      if self.debug:
          print "lock released.. "

import tempfile
import os

class dumbBuffer:
    __allow_access_to_unprotected_subobjects__=1
    debug = ZWIFF_DEBUG
    
    def __init__(self, ext='', tmpdir=''):
        self.f = None
        if tmpdir:
            tempfile.tempdir=tmpdir
        
        self.tmpfile = tempfile.mktemp()
        if ext:
            self.tmpfile = self.tmpfile + ext
        
    def filename(self):
        return self.tmpfile
        
    def write(self, s):
        f = open(self.tmpfile,'w')
        f.write(s)
        f.close()
    
    def read(self):
        f = open(self.tmpfile)
        s = f.read()
        f.close()
        return s
    
    def __del__(self):
       if not ZWIFF_DEBUG:
          os.unlink(self.tmpfile)
          
        
    
