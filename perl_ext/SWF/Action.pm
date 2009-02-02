# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 Albrecht Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Action;
use SWF ();

$SWF::Action::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Action - SWF action class

=head1 SYNOPSIS

	use SWF::Action;
	my $action = new SWF::Action($script);

=head1 DESCRIPTION

SWF::Action is an object for storing an action.

=head1 METHODS

=over 4

=item new SWF::Action($script)

Compiles $script and returns an SWF::Action object.
The script is a text string expected, according ActionScript syntax.

=item $result = $actionObject->compile($versionX)

Recompile object using syntax of versionX. 
(Some differences exists between values 4 and 5.)
The result return value will be 0 on successful compiling.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca) and
developers of ming at ming.sourceforge.net                                                                                                                                                                                      

=head1 SEE ALSO

SWF, SWF::Button, SWF::Movie, SWF::MovieClip, SWF::InitAction

=cut
