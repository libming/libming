BEGIN { $| = 1; }

use SWF qw(Shape Bitmap);
require 't/config.pl';

my $filename = ming_dir() . "/examples/common/png.dbl";
skip_test() unless (-e $filename);

print "1..1\n"; 

eval{
    my $s = new SWF::Shape();
    my $b = new SWF::Bitmap($filename);
    my $f = $s->addFill($b);
    $s->setRightFill($f);
    
    $s->drawLine(32, 0);
    $s->drawLine(0, 32);
    $s->drawLine(-32, 0);
    $s->drawLine(0, -32);
    
    my $m = new SWF::Movie();
    $m->setDimension(32, 32);
    $m->add($s);
    $m->save('t/png.swf');
};

ok($@);
