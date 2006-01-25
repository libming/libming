#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(Movie Shape);


SWF::setScale(1.0);

my $s = new SWF::Shape;
$s->setRightFill($s->addFill(0xff, 0, 0));
$s->movePenTo(-500,-500);
$s->drawLineTo(500,-500);
$s->drawLineTo(500,500);
$s->drawLineTo(-500,500);
$s->drawLineTo(-500,-500);

my  $p = new SWF::MovieClip;
my $i = $p->add($s);

for(my $j=0; $j<17; ++$j)
{
    $p->nextFrame();
    $i->rotate(5);
}
$p->nextFrame();

my $m = new SWF::Movie;
$i = $m->add($p);
$i->moveTo(1500,1000);
$i->setName("blah");

$m->setBackground(0xff, 0xff, 0x0);
$m->setDimension(3000,2000);

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
