# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Id$

package SWF::Constants;
use SWF();

$SWF::Constants::VERSION = $SWF::VERSION;

1;


=head1 NAME

SWF::Constants - Constants defined in ming

=head1 SYNOPSIS

 use SWF::Constants 'SWFBUTTON_MOUSEUP'; # import SWFBUTTON_MOUSEUP
 use SWF::Constants ':Button';           # import all constants for SWF::Button


=head1 DESCRIPTION

SWF::Constants allows importing constants defined in ming.

=head1 EXPORT TAGS

=over 4

=item Button

This tag imports constants most commonly used by SWF::Button.

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

=item DisplayItem

This tag imports constants most commonly used by SWF::DisplayItem.

    SWFACTION_ONLOAD
    SWFACTION_ENTERFRAME
    SWFACTION_UNLOAD
    SWFACTION_MOUSEMOVE
    SWFACTION_MOUSEDOWN
    SWFACTION_MOUSEUP
    SWFACTION_KEYDOWN
    SWFACTION_KEYUP
    SWFACTION_DATA


=item Fill

This tag imports constants most commonly used by SWF::Fill.

    SWFFILL_SOLID
    SWFFILL_GRADIENT
    SWFFILL_LINEAR_GRADIENT
    SWFFILL_RADIAL_GRADIENT
    SWFFILL_BITMAP
    SWFFILL_TILED_BITMAP
    SWFFILL_CLIPPED_BITMAP

=item Sound

This tag imports constants most commonly used by SWF::Sound.

    SWF_SOUND_NOT_COMPRESSED
    SWF_SOUND_NOT_COMPRESSED_LE
    SWF_SOUND_ADPCM_COMPRESSED
    SWF_SOUND_MP3_COMPRESSED
    SWF_SOUND_NELLY_COMPRESSED

    SWF_SOUND_5KHZ
    SWF_SOUND_11KHZ
    SWF_SOUND_22KHZ
    SWF_SOUND_44KHZ

    SWF_SOUND_8BITS
    SWF_SOUND_16BITS

    SWF_SOUND_MONO
    SWF_SOUND_STEREO

=item Text

This tag imports constants most commonly used by SWF::TextField.

    SWFTEXTFIELD_NOEDIT
    SWFTEXTFIELD_PASSWORD
    SWFTEXTFIELD_DRAWBOX
    SWFTEXTFIELD_MULTILINE
    SWFTEXTFIELD_WORDWRAP
    SWFTEXTFIELD_NOSELECT
    SWFTEXTFIELD_ALIGN_LEFT
    SWFTEXTFIELD_ALIGN_RIGHT
    SWFTEXTFIELD_ALIGN_CENTER
    SWFTEXTFIELD_ALIGN_JUSTIFY
    SWFTEXTFIELD_HTML
    SWFTEXTFIELD_HASLENGTH
    SWFTEXTFIELD_USEFONT
    SWFTEXTFIELD_AUTOSIZE

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net).
Peter Liscovius

and see AUTHORS in ming lib distribution (ming.sf.net)

=head1 SEE ALSO

SWF, SWF::Action, SWF::Bitmap, SWF::Button, SWF::Fill, SWF::Gradient, SWF::Movie, SWF::MovieClip, SWF::Shape, 
SWF::Sound, SWF::TextField

=cut
