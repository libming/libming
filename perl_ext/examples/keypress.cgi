#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(Action Shape TextField Button);
use SWF::Constants qw(:Text :Button);

SWF::setScale(1.0);
SWF::setVersion(4);

# Add path to your *.fdb file
my $fdb_file = '../common/_sans.fdb';

# sprite has one letter per frame */
my  $p = new SWF::MovieClip;
$p->add(new SWF::Action("stop();"));

my $chars = "abcdefghijklmnopqrstuvwxyz".
    "1234567890!@#$%^&*()_+-=/[]{}|;:,.<>?`~";

my $f = new SWF::Font($fdb_file);
my $n;
my $i = undef;
for($n=0; $n<length($chars); ++$n)
{
    my $c = substr($chars, $n, 1);

    $p->remove($i) if(defined($i));
    my $t = new SWF::TextField(SWFTEXTFIELD_DRAWBOX);
    $t->setFont($f);
    $t->setHeight(240);
    $t->setBounds(600,240);
    $t->align(SWFTEXTFIELD_ALIGN_CENTER);
    $t->addString($c);
    $i = $p->add($t);
    $p->labelFrame($c);
    $p->nextFrame();
}


# hit region for button - the entire frame 

my $s = new SWF::Shape();
$s->setRightFill($s->addFill(0, 0, 0));
$s->drawLine(600, 0);
$s->drawLine(0, 400);
$s->drawLine(-600, 0);
$s->drawLine(0, -400);


# button checks for pressed key, sends sprite to the right frame */

$b = new SWF::Button();
$b->addShape($s, SWFBUTTON_HIT);

for($n=0; $n<length($chars); ++$n){
    my $c = substr($chars, $n, 1);
    $b->addAction(new SWF::Action("

setTarget('/char');
gotoFrame('$c');

    "), SWFBUTTON_KEYPRESS($c));
  }

my $m = new SWF::Movie();
$m->setDimension(600,400);
$i = $m->add($p);
$i->setName('char');
$i->moveTo(0,80);

$m->add($b);

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}

