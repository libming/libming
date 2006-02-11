The Ch Ming package is a Ch binding to the libming C library.

Ch is an embeddable C/C++ interpreter. Which the Ch Ming package,
C (or C++) programs using Ming C functions can readily run in 
Ch without compilation.


Release Notes
----------------------------------
Ch Ming v1.0.0, May 2nd, 2005
   First release. It has been tested in both Windows, Linux, 
   and Mac OSX 10.3 with fink


System Requirements
--------------------------------------------
(1) Ch Standard or Professional Edition version 5.1.0.12621 or higher.
    Ch is available from http://www.softintegration.com.
(2) It has been tested and works with Ming version 0.3 beta 1 or above. 
    It might work with lower version without guarantee. For Windows, the 
    release version of the Ming library must be built. For Unix, either
    static or shared versions of the Ming library can be used. With 
    shared, the library must be installed.
(3) the dependant ZLIB binary library for Ming is required
(4) the dependant GIF binary library for Ming is optional 
(5) the dependant PNG binary library for Ming is optional

NOTE: (for Windows)
  If the PNG or GIF library are not used, the c/Makefile.win file must
  be modified in order to build under Windows.


Contents
--------------------------
Ch Ming package contains the following directories

(1) Files and utilities used to build Ch Ming package
    using native C compiler and Ch SDK
Makefile      ---  makefile for handing different commands
chfcreate     ---  bare Ming headers for creating .chf files
chfhandmade   ---  Ch function files in .chf for special functions
demos         ---  demos to run in Ch
pkgcreate.ch  ---  A Ch program to update Ch Ming package
license       ---  license file, the same license as MING
readme.txt    ---  this readme file


Installation Instructions:
--------------------------
(1) Run command 
        make 
        make install


Uninstallation Instructions:
--------------------------
(1) Run command
        make uninstall


Run Demo Programs in Directory CHHOME/package/chming/demos:
-------------------------------------------------------------
(1) start ch
(2) type program name such as 'action.c'


Contributors
------------------------------------------------------
Stephen Nestinger   ssn17@yahoo.com

