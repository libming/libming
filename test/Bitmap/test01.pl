#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

use SWF qw(:ALL);

$m = SWF::Movie::newSWFMovieWithVersion(8);

$b = new SWF::Bitmap($mediadir."/image01.png");

$m->addExport($b, "BitmapExport");
$m->writeExports();

$m->save("test01.swf");
