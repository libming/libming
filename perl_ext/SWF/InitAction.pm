# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::InitAction;
use SWF ();

$SWF::InitAction::VERSION = $SWF::VERSION;
1;

__END__

=head1 NAME

SWF::InitAction - SWF init action class

=head1 SYNOPSIS

	use SWF::InitAction;
	$initaction = new SWF::InitAction( $action );

=head1 DESCRIPTION

SWF::InitAction is an action helper class.

=head1 METHODS

=over

=item new SWF::InitAction( $action )

This method creates an InitAction object and defines an empty 
MovieClip which is not placed. Parameter $action is an object 
of SWF::Action class.  [ToDo: add demo code here!]

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Action, SWF::MovieClip

=cut
