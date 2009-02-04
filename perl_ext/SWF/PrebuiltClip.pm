# ====================================================================
# Copyright (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::PrebuiltClip;
use SWF ();

$SWF::PrebuiltClip::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::PrebuiltClip - SWF PrebuiltClip class

=head1 SYNOPSIS

	use SWF::PrebuiltClip;
	my $prebclip = new SWF::PrebuiltClip("prebuilt.swf");
	$displayitem = $movieclip->add($prebclip); 

=head1 DESCRIPTION

SWF::PrebuiltClip is a helper class useful for adding prebuilt clips from SWF-files into movieclips.

=head1 METHODS

=over

=item new SWF::PrebuiltClip($filename);

Creates a SWF::PrebuiltClip object using filename. Use the created object to add that object
into SWF::MovieClip objects using the MovieClip's add() method.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Movie, SWF::MovieClip, SWF::DisplayItem

=cut
