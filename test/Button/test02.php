#!/usr/bin/php -c.
<?php
	$m = new SWFMovie();
	
	$s = new SWFShape();
	$s->setLine(1, 255, 0, 0,255);
	$s->setRightFill(255,255,0,255);
	$s->drawLine(100,0);
	$s->drawLine(0,40);
	$s->drawLineTo(0,0);
	
	$b = new SWFButton();
	$br1 = $b->addCharacter($s, SWFBUTTON_HIT|SWFBUTTON_UP|SWFBUTTON_OVER|SWFBUTTON_DOWN);

	$br2 = $b->addCharacter($s, SWFBUTTON_OVER|SWFBUTTON_DOWN);
	$br2->rotate(10);
	$br2->move(20,0);

	$br3 = $b->addCharacter($s, SWFBUTTON_DOWN);
	$br3->rotate(20);
	$br3->move(40,0);

	$d = $m->add($b);
	$d->moveTo(50, 100);
	
	$m->addExport($b, "ButtonExport");
	$m->writeExports();

	$m->nextFrame;
	$m->save("test02.swf");
?>
	
	

