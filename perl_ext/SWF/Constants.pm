# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 Albrecht Kleine
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
    SWFBLEND_MODE_NULL
    SWFBLEND_MODE_NORMAL
    SWFBLEND_MODE_LAYER
    SWFBLEND_MODE_MULT
    SWFBLEND_MODE_SCREEN
    SWFBLEND_MODE_DARKEN
    SWFBLEND_MODE_LIGHTEN
    SWFBLEND_MODE_ADD
    SWFBLEND_MODE_SUB
    SWFBLEND_MODE_DIFF
    SWFBLEND_MODE_INV
    SWFBLEND_MODE_ALPHA
    SWFBLEND_MODE_ERASE
    SWFBLEND_MODE_OVERLAY
    SWFBLEND_MODE_HARDLIGHT

=item Fill

This tag imports constants most commonly used by SWF::Fill.

    SWFFILL_SOLID
    SWFFILL_GRADIENT
    SWFFILL_LINEAR_GRADIENT
    SWFFILL_RADIAL_GRADIENT
    SWFFILL_FOCAL_GRADIENT
    SWFFILL_BITMAP
    SWFFILL_TILED_BITMAP
    SWFFILL_CLIPPED_BITMAP

available since SWF8:

    SWFFILL_FOCAL_GRADIENT
    SWFFILL_NONSMOOTHED_TILED_BITMAP
    SWFFILL_NONSMOOTHED_CLIPPED_BITMAP

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

This tag imports constants most commonly used by SWF::Video.

    SWFVIDEOSTREAM_MODE_AUTO
    SWFVIDEOSTREAM_MODE_MANUAL

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

= item Shape

This tag imports constants most commonly used by SWF::Shape.

    SWF_SHAPE_USESCALINGSTROKES
    SWF_SHAPE_USENONSCALINGSTROKES
    SWF_LINESTYLE_CAP_ROUND
    SWF_LINESTYLE_CAP_NONE
    SWF_LINESTYLE_CAP_SQUARE
    SWF_LINESTYLE_JOIN_ROUND
    SWF_LINESTYLE_JOIN_BEVEL
    SWF_LINESTYLE_JOIN_MITER
    SWF_LINESTYLE_FLAG_NOHSCALE
    SWF_LINESTYLE_FLAG_NOVSCALE
    SWF_LINESTYLE_FLAG_HINTING
    SWF_LINESTYLE_FLAG_NOCLOSE
    SWF_LINESTYLE_FLAG_ENDCAP_ROUND
    SWF_LINESTYLE_FLAG_ENDCAP_NONE
    SWF_LINESTYLE_FLAG_ENDCAP_SQUARE

=item Filter

This tag imports constants most commonly used by SWF::Filter,
all available since SWF8:

    SWF_FILTER_MODE_INNER
    SWF_FILTER_MODE_KO
    SWF_FILTER_MODE_COMPOSITE
    SWF_FILTER_MODE_ONTOP
    SWF_FILTER_CLAMP
    SWF_FILTER_FLAG_PRESERVE_ALPHA

=item Gradient

This tag imports constants most commonly used by SWF::Gradient,
all available since SWF8:

    SWF_GRADIENT_PAD
    SWF_GRADIENT_REFLECT
    SWF_GRADIENT_REPEAT
    SWF_GRADIENT_NORMAL
    SWF_GRADIENT_LINEAR

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net).
Peter Liscovius
Albrecht Kleine

and see AUTHORS in ming lib distribution (ming.sf.net)

=head1 SEE ALSO

SWF, SWF::Action, SWF::Bitmap, SWF::Button, SWF::Fill, SWF::Gradient, SWF::Movie, SWF::MovieClip, SWF::Shape, 
SWF::Sound, SWF::TextField, SWF::Filter

=cut
