# ====================================================================
# Copyright (c) 2002 by Klaus Rechert. All rights reserved.
#           (c) 2009 Albrecht Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::ButtonRecord;
use SWF();

use strict;

$SWF::ButtonRecord::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::ButtonRecord - SWF Button Helper Class

=head1 SYNOPSIS

	use SWF::ButtonRecord;
	use SWF::Button;
	use SWF::Constants qw(:Button);
	# create button and a shape here..
	# and then:
	$buttonrecord = $button->addCharacter($my_shape, SWFBUTTON_DOWN);
	$buttonrecord->scale(0.5,0.5);
	$buttonrecord->move(50,50);

=head1 DESCRIPTION

Adding a character shape to an object of SWF::Button class will give you
access to an SWF::ButtonRecord object. This is useful for further changes
by using the declared methods below. So SWF::ButtonRecord objects are
a comparable counterpart to SWF::Displayitem objects. Just like them
there is no constructor method available.

=head1 METHODS

=over

=item $buttonRecord->moveTo($x, $y)

Move ButtonRecord to ($x, $y).

=item $buttonRecord->move($x, $y)

Displace ButtonRecord by ($x, $y).

=item $buttonRecord->scaleTo($x ,$y)

Set ButtonRecord scale to $x in the x-direction and $y in the y-direction.

=item $buttonRecord->scale($x ,$y)

Multiply ButtonRecord scale by $x in the x-direction and $y in the y-direction.

=item $buttonRecord->rotateTo($degrees)

Set ButtonRecord rotation to $degrees.

=item $buttonRecord->rotate($degrees)

Rotate ButtonRecord by $degrees.

=item $buttonRecord->skewX($x)

Add $x to the current x-skew.

=item $buttonRecord->skewXTo($x)

Set x-skew to $x. 1.0 is 45-degree forward slant. More is more forward while less is more backward.

=item $buttonRecord->skewY($y)

Add $y to the current y-skew.

=item $buttonRecord->skewYTo($y)

Set y-skew to $y. 1.0 is 45-degree upward slant. More is more upward while less is more downward.

=item $buttonRecord->setDepth($depth)

Set Z-order of ButtonRecord to $depth.

=item $buttonRecord->setBlendMode($mode)

Set an alternative blend mode instead of default alpha blend.
Possible modes are SWFBLEND_MODE_NORMAL, SWFBLEND_MODE_LAYER etc.

=item $buttonRecord->addFilter($filter)

Process the ButtonRecord object thru a prepared filter:
an object of SWF::Filter class, e.g. BlurFilter or DropShadowFilter.
Filters are available since player version 8.

=back

=head1 AUTHOR

developers of ming 
ming.sourceforge.net,
Albrecht Kleine

=head1 SEE ALSO

SWF, SWF::Button, SWF::DisplayItem, SWF::Filter, SWF::Movie, SWF::MovieClip

=cut
