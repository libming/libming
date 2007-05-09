dnl
dnl    Copyright (C) 2005, 2006 Free Software Foundation, Inc.
dnl
dnl  This program is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 2 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
dnl
dnl

# Configure paths for Ming
# Author: Sandro Santilli <strk@keybit.net>
#
# This macro uses ming-config, which was
# not available as of Ming 0.3beta1
#
#
# Use: 
#	AC_PATH_MING
#
# Provides:
#	MING_VERSION	  - Ming version string (example: "0.4.1" or "0.4.0.beta2")
#	MING_VERSION_CODE - a 8digits number encoding Major, Minor, Patch and Beta numbers.
#	                    examples: 00040002 (0.4.0.beta2) 00040100 (0.4.1)
#	MING_CFLAGS
#	MING_LIBS
#	MAKESWF
#

AC_DEFUN([AC_PATH_MING],
[
	MING_CFLAGS=""
	MING_LIBS=""

	AC_ARG_WITH(ming,[  --with-ming=[<ming-config>]    Path to the ming-config command],
		[
		case "${withval}" in
			yes|no)
				;;
			*) MING_CONFIG=${withval}
				;;
		esac
		], MING_CONFIG="")

	if test x"$MING_CONFIG" = "x"; then
		AC_PATH_PROG(MING_CONFIG, ming-config)
	fi

	if test x"$MING_CONFIG" != "x"; then
		MING_VERSION=`$MING_CONFIG --version`
		major=`echo $MING_VERSION | \
			sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/'`
		minor=`echo $MING_VERSION | \
			sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/'`
		micro=`echo $MING_VERSION | \
			sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/'`
		beta=`echo $MING_VERSION | sed -ne 's/.*beta\([[0-9]]*\).*/\1/p'`
		MING_VERSION_CODE=`printf %2.2d%2.2d%2.2d%2.2d $major $minor $micro $beta`
		MING_CFLAGS=`$MING_CONFIG --cflags`
		MING_LIBS=`$MING_CONFIG --libs`
		MING_PATH=`$MING_CONFIG --bindir`
		AC_PATH_PROG([MAKESWF], [makeswf], , [$MING_PATH:$PATH])
	fi


	AC_SUBST(MING_VERSION_CODE)
	AC_SUBST(MING_VERSION)
	AC_SUBST(MING_CFLAGS)
	AC_SUBST(MING_LIBS)
	AC_SUBST(MAKESWF)
])
