# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Id$

package SWF;
use strict;
use Carp 'croak';

require DynaLoader;
@SWF::ISA = qw(DynaLoader);

$SWF::VERSION = '0.3beta1';

my @EXPORT_OK = qw(Action Bitmap Button Constants DisplayItem Fill Font Gradient Morph Movie Shape Sound Sprite Text TextField);

bootstrap SWF $SWF::VERSION;

sub import{
    my $self = shift;
    my @modules = @_;

    my $package = (caller())[0];
    my @failed;

    foreach my $module (@modules) {
	my $code = "package $package; ";
	if($module eq ':ALL'){
	    map{$code .= "use SWF::$_; "} @EXPORT_OK;
	}else{
	    $code .= "use SWF::$module;";
	}

	eval($code);
	if ($@) {
	    warn $@;
	    push(@failed, $module);
	}
    }
    @failed and croak "could not import qw(" . join(' ', @failed) . ")";
}


1;
__END__


=head1 NAME

SWF: an autoloadable interface module for Ming - a library for generating 
SWF ("Flash") format movies.

=head1 SYNOPSIS

 # Don't import other modules
 use SWF;              
 or use SWF();

 # import all SWF modules
 use SWF qw(:ALL);

 # import SWF::Shape and SWF::Movie only.
 use SWF qw(Shape Movie);


=head1 DESCRIPTION

By default, SWF doesn't import other SWF classes. You may, however, instruct SWF to import all modules by using the following syntax:

            use SWF qw(:ALL);

=head1 METHODS

=over 4

=item SWF::setScale($scale);

Sets scale to $scale.

=item SWF::setVersion($version);

Sets SWF version. Only versions 4 and 5 are currently supported. 

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Bitmap, SWF::Button, SWF::Constants, SWF::DisplayItem, SWF::Fill, SWF::Font, SWF::Gradient, SWF::Morph, SWF::Movie, SWF::Shape, SWF::Sound, SWF::Sprite, SWF::TextField, SWF::Text

=cut
