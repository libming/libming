# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::TextField;
use SWF ();

1;

__END__

=head1 NAME

SWF::TextField - SWF TextField class

=head1 SYNOPSIS

 use SWF::TextField();   # Do not import any constant
 my $testfield = new SWF::TextField();

 #import all constants and functions available to current package
 use SWF::TextField qw(:Text);


=head1 DESCRIPTION

Creates a TextField for flash movies. By default, SWF::TextField will not import anything to your package, i.e. you have to explicitly indicate which constants you need. Here is the list of constants that you may import to your package:

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

For example, if you want to import SWFTEXTFIELD_HTM aonly, use the following statement:

    use SWF::TextFiels qw(SWFTEXTFIELD_HTM);

You may use the following shortcut, if you want to import everything

    use SWF::TextField ':SWFTEXTFIELD_HTM';

=head1 METHODS

=item XXXXXXXXXXXXXXXXXXXXXX add methods

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Shape, SWF::Movie, .


=cut
