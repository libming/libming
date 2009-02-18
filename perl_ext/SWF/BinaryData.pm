# ====================================================================
# Copyright (c) 2009 Albrecht Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::BinaryData;
use SWF ();

$SWF::BinaryData::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::BinaryData - SWF binary data class

=head1 SYNOPSIS

	use SWF::BinaryData;
	$binarydata = new SWF::BinaryData($data, $length);

=head1 DESCRIPTION

SWF::BinaryData is a helper class useful in ActionScript.

=head1 METHODS

=over

=item new SWF::BinaryData($data, $length)

This constructor method allows to embed arbitrary binary data 
into a SWF file. This data later can be used as a byte array
in AS3 classes. [ToDo: add demo code here !]

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Action, SWF::InitAction, SWF::Movie

=cut
