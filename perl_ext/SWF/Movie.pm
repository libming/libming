# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Movie;
use SWF();

$SWF::Movie::VERSION = $SWF::VERSION;

use strict;

sub streamMp3{
    my $movie = shift;
    my $filename = shift;
    $movie->setSoundStream(new SWF::Sound($filename));
}


1;
