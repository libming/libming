# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Morph;
use SWF ();

$SWF::Morph::VERSION = $SWF::VERSION;

sub getShapes{
    my $self = shift;
    return ($self->getShape1(), $self->getShape2());
}

1;

__END__

=head1 NAME

SWF::Morph - SWF Morph Class

=head1 SYNOPSIS

	use SWF::Morph;
	my $morph = new SWF::Morph();

=head1 DESCRIPTION

SWF::Morph twist from one shape to another.

=head1 METHODS

=over 4

=item new SWF::Morph();

Returns an SWF::Morph object.

=item ($shape1, $shape2) = $morph->getShapes();

Returns two SWF::Shape objects. The first one (shape1) is the morph's starting shape while shape2 is its final shape. You may also use "getShape1()" and "getShape2()" methods to get a handle of these shapes.

=item $shape1 = $morph->getShape1();

Returns the morph's starting shape.

=item $shape2 = $morph->getShape2();

Returns the morph's final shape.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::DisplayItem, SWF::Shape, SWF::Movie, SWF::MovieClip

=cut
