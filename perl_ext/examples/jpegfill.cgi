#!/usr/bin/perl
use strict;

use SWF qw(:ALL);
SWF::setScale(20.0);

print "Content-type: application/x-shockwave-flash\n\n";

my $s = new SWF::Shape();
my $b = new SWF::Bitmap('../common/flowers.jpg');

my $f = $s->addFill($b);
$s->setRightFill($f);

$s->drawLine(640, 0);
$s->drawLine(0, 480);
$s->drawLine(-640, 0);
$s->drawLine(0, -480);

my $m = new SWF::Movie();
$m->setDimension(640, 480);
$m->add($s);
# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
