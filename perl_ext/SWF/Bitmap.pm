# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Bitmap;
use SWF();

use strict;

1;

__END__

=head1 NAME

SWF::Bitmap - Bitmap class

=head1 SYNOPSIS

 use SWF::Bitmap;
 my $bitmap = new SWF::Bitmap();


=head1 DESCRIPTION

SWF::Bitmap enables you to add JPG and DBL files.

=head1 METHODS

=over 4

=item new SWF::Bitmap();

Creates a new Bitmap object.

=item $bitmap->getWidth();

Return width of bitmap in pixels.

=item $bitmap->getHeight();

Return height of bitmap in pixels.

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Button, SWF::Movie.


=cut
