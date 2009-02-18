# ====================================================================
# Copyright (c) 2008 Klaus Rechert. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Character;
use SWF();

use strict;

$SWF::Character::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Character - SWF Character Class

=head1 SYNOPSIS

	use SWF::Character;
	$character=$displayItem->getCharacter();
	print $character->getWidth();

=head1 DESCRIPTION

SWF::Character is a helper class, objects are available
thru SWF::DisplayItem's getCharacter() and SWF::Movie's 
importCharacter() methods, so there is no constructor here.

=head1 METHODS

=over

=item $width = $character->getWidth()

This method returrns the character's width in px. 
If a character has no bounds -1 is returned.

=item $height = $character->getHeight()

This method returrns the character's height in px. 
If a character has no bounds -1 is returned.

=back

=head1 AUTHOR

	developers of ming 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::DisplayItem

=cut
