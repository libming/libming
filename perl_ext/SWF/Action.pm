# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Action;
use SWF ();

1;

__END__

=head1 NAME

SWF::Action - SWF action class

=head1 SYNOPSIS

 use SWF::Action;
 my $action = new SWF::Action($script);

=head1 DESCRIPTION

SWF::Action compiles ascript into a.

=head1 METHODS

=over 4

=item new SWF::Action($script);

Compiles $script and returns an SWF::Action object.


=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Button, SWF::Shape, SWF::Movie, .

=cut
