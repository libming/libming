# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Button;
use SWF();


1;

__END__

=head1 NAME

SWF::Button - SWF button class

=head1 SYNOPSIS

 use SWF::Button();   # Do not import any constant
 my $button = new SWF::Button();

 #import all constants and functions available to current package
 use SWF::Button qw(:Button);


=head1 DESCRIPTION

Creates buttons for flash movies. By default, SWF::Button will not import anything to your package, i.e. you have to explicitly indicate which constant or function you need. Here is the list of constants and functions that you may import to your package:

    SWFBUTTON_MOUSEUP
    SWFBUTTON_HIT
    SWFBUTTON_DOWN
    SWFBUTTON_OVER
    SWFBUTTON_UP
    SWFBUTTON_MOUSEOVER
    SWFBUTTON_MOUSEOUT
    SWFBUTTON_MOUSEDOWN
    SWFBUTTON_DRAGOUT
    SWFBUTTON_DRAGOVER
    SWFBUTTON_MOUSEUPOUTSIDE
    SWFBUTTON_KEYPRESS
    SWFBUTTON_ONKEYPRESS

For example, if you want to import SWFBUTTON_MOUSEOVER and SWFBUTTON_MOUSEOUT
only, use the following statement:

    use SWF::Button qw(SWFBUTTON_MOUSEOVER SWFBUTTON_MOUSEOUT);

You may use the following shortcut, if you want to import everything

    use SWF::Button ':Button';

=head1 METHODS

=over 4

=item new SWF::Button();

Creates a new Button object.


=item $button->addShape($shape, FLAG);

Adds $shape to the button. Valid FLAGs are:

    SWFBUTTON_HIT
    SWFBUTTON_UP
    SWFBUTTON_DOWN
    SWFBUTTON_OVER


=item $button->setOver($shape);

Shortcut for $button->addShape($shape, SWFBUTTON_OVER);

=item $button->setHit($shape);

Shortcut for $button->addShape($shape, SWFBUTTON_HIT);

=item $button->setUp($shape);

Shortcut for $button->addShape($shape, SWFBUTTON_UP);

=item $button->setDown($shape);

Shortcut for $button->addShape($shape, SWFBUTTON_DOWN);


=item $button->setAction($action [,FLAG]);

Adds $action object (see SWF::Action). Valid FLAGs are:

    SWFBUTTON_MOUSEUP
    SWFBUTTON_MOUSEOVER
    SWFBUTTON_MOUSEOUT
    SWFBUTTON_MOUSEDOWN
    SWFBUTTON_DRAGOVER
    SWFBUTTON_DRAGOUT

SWFBUTTON_MOUSEUP is the default value of FLAG.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Shape, SWF::Movie, .


=cut
