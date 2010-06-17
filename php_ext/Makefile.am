######################################################################
#
#    Ming, an SWF output library
#
#    Copyright (C) 2010  Sandro Santilli <strk@keybit.net>
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
######################################################################

EXTRA_DIST = ming.c php_ming.h README.PHP4 \
	README.PHP5 ming.dsp

pkglib_LTLIBRARIES = ming.la
ming_la_SOURCES = ming.c php_ming.h
ming_la_LIBADD =  $(top_builddir)/src/libming.la 
# TODO: check if -no-undefined should be added
ming_la_LDFLAGS = -module -avoid-version
# TODO: drop conditionals from ming.c and let it bound to current code
ming_la_CFLAGS = `php-config --includes` \
	-DCOMPILE_DL_MING \
	-DHAVE_MING=1 \
	-DHAVE_NEW_MING=1 \
	-DHAVE_SWFVIDEOSTREAM_NEXTFRAME=1 \
	-DHAVE_SWFVIDEOSTREAM_SEEK=1 \
	-DHAVE_SWFVIDEOSTREAM_SETFRAMEMODE \
	-UHAVE_CONFIG_H

install: all
	$(INSTALL) -d "$(DESTDIR)"/`php-config --extension-dir`
	$(INSTALL) -m 755 .libs/ming.so "$(DESTDIR)"/`php-config --extension-dir`
