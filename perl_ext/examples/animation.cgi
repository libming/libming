#!/usr/bin/perl
use strict;

# Were you looking for thw worst perl script?
# Congradulations! you found it.

use SWF (:ALL);

my $string = "ming!";
my $dir = '/home/soheil/src/ming-0.0.9/examples/common/';

print "Content-type: application/x-shockwave-flash\n\n";

my $loc = {};
my $i1 = { r      => 0xff, 
	   g      => 0x33, 
	   b      => 0x33, 
	   a      => 0xff, 
	   rot    => 900, 
           x      => 1200, 
	   y      => 800,
	   scale  => 0.03, 
	   string => $string,
	  };

my $i2 = { r      => 0x00, 
	   g      => 0x33,
	   b      => 0xff, 
	   a      => 0x7f, 
	   rot    => -560, 
           x      => 1200, 
	   y      => 800,
	   scale  => 0.04, 
	   string => $string,
	  };

my $i3 = { r      => 0xff, 
	   g      => 0xff,
	   b      => 0xff, 
	   a      => 0x9f, 
	   rot    => 180, 
           x      => 1200, 
	   y      => 800,
	   scale  => 0.001, 
	   string => $string,
	  };



my  $f = new SWF::Font($dir."test.fdb");
my $m = new SWF::Movie();
$m->setRate(24.0);
$m->setDimension(2400, 1600);
$m->setBackground(0xff, 0xff, 0xff);



sub text{
    my $j = shift;

    #print STDERR "Enetrnig TEXT\n";

    my $t = new SWF::Text();
    $t->setFont($f);
    $t->setColor($j->{r}, $j->{g}, $j->{b}, $j->{a});
    $t->setHeight(960);
    $t->moveTo(-($t->getWidth($string))/2, 220); 
    $t->addString($string);

    my $i = $m->add($t);
    $j->{x} = $j->{x};
    $j->{y} = $j->{y};
    $j->{rot} = $j->{rot};
    $j->{s} = $j->{scale};
    $i->rotateTo($j->{rot});
    $i->scale($j->{scale}, $j->{scale});

    $j->{item} = $i;
#    $j->{text} = $t;
    return $j;
}

sub step{
    my $j = shift;
    #print STDERR "Enetrnig STEP\n";
    my $oldrot = $j->{rot};
    $j->{rot} = 19*($j->{rot})/20;
    $j->{x} = (19*$j->{x} + 1200)/20;
    $j->{y} = (19*$j->{y} + 800)/20;
    $j->{s} = (19*$j->{s} + 1.0)/20;

    my $i = $j->{item};
    $i->rotateTo($j->{rot});
    $i->scaleTo($j->{s}, $j->{s});
    $i->moveTo($j->{x}, $j->{y});
    return $j;
}


$i1 = text($i1);
$i2 = text($i2);
$i3 = text($i3);

for(my $i=1; $i<=100; ++$i){
    $i1 = step($i1);
    $i2 = step($i2);
    $i3 = step($i3);
    $m->nextFrame();
}

$m->output();
