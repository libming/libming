#!/usr/bin/php -c.

<? 

$m = new SWFMovie();
$tf = new SWFTextField();

$font = new SWFBrowserFont("_sans");

$tf->setFont($font);
$tf->setColor(0, 0, 0);
$tf->setHeight(20);
$tf->addString("abc");

$m->add($tf);
$m->nextFrame();
$m->save("test01.swf");

?>
