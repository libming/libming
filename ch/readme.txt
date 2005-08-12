The Ch Ming package is a Ch binding to the libming C library.

Ch is an embeddable C/C++ interpreter. Which the Ch Ming package,
C (or C++) programs using Ming C functions can readily run in 
Ch without compilation.


Release Notes
----------------------------------
Ch Ming v1.0.0, May 2nd, 2005
   First release. It has been tested in both Windows, Linux, and Mac OSX 10.3 with fink

System Requirements
--------------------------------------------
(1) Ch Standard or Professional Edition version 5.0.1.12211 or higher.
    Ch is available from http://www.softintegration.com.
(2) It has been tested and works with Ming version 0.3 beta 1 or above. It might
    work with lower version without guarantee.
(3) the dependant ZLIB binary library for Ming is also required
(4) the dependant GIF binary library for Ming is also required
(5) the dependant PNG binary library for Ming is also required


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
pkginstall.ch ---  A Ch program to install Ch Ming package
license       ---  license file, the same license as MING

(2) chming - Ch Ming package
chming/demos       ---  Ming demo programs in C readily to run in Ch
chming/bin         ---  Ming dynamical library and commands
chming/dl          ---  dynamically loaded library
chming/include     ---  header files
chming/lib         ---  function files

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

How to update Ch Ming package for newer versions of Ming:
-------------------------------------------------------------
(1) If Ch has not been installed in your computer,
    install Ch from http://www.softintegration.com.
(2) Run
        make 
(3) install the newly created Ch Ming package by command
        make install 
(4) Questions specific to using Ch SDK to update Ch Ming can be posted in 
    either Ming discussion group or Ch discussion group at
    http://www.softintegration.com.  

Contributors
------------------------------------------------------
Stephen Nestinger   ssn17@yahoo.com

