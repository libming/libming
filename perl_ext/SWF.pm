# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

package SWF;

use strict;
use vars qw($VERSION @ISA);

require DynaLoader;
@ISA = qw(DynaLoader);

$VERSION = '0.01b';

bootstrap SWF $VERSION;

1;
__END__
