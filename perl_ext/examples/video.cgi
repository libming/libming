#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(:ALL);

my $file = '../common/test.flv';



my $stream=new SWF::VideoStream($file);
$stream->setDimension(200,200);

my $m=new SWF::Movie();
$m->setDimension(400, 300);
$m->setRate(12.0);
$m->add($stream);
for(1..$stream->getNumFrames()) {
	$m->nextFrame();
}

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
