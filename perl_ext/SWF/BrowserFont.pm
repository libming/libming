# ====================================================================
# Copyright (c) 2008 by Klaus Rechert. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::BrowserFont;

use SWF();

$SWF::BrowserFont::VERSION = $SWF::VERSION;


1;
=head1 NAME

SWF::BrowserFont - SWF BrowserFont class

=head1 SYNOPSIS

	use SWF::BrowserFont;
	$font = new SWF::BrowserFont('_sans');

=head1 DESCRIPTION

Browser built-in font object for later usage in TextField objects.
Not available for Text objects. Using BrowserFont objects will save
memory in output file. Consider that not every font might be available
in any environment.

=head1 METHODS 

=over

=item $bf = new SWF::BrowserFont($name)

Creates a BrowserFont object, name selects a built-in font, like '_sans'
or '_typewriter'.

=back

=head1 AUTHOR

developers of ming
ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Font, SWF::Text, SWF::TextField

=cut
