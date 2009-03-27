#!/usr/bin/php -c.
<?php
$m = new SWFMovie();

ming_setscale(1.0);

$xml = "<rdf:RDF xmlns:rdf=http://www.w3.org/1999/02/22-rdf-syntax-ns#></rdf:RDF>";

$m->addMetadata($xml);

$m->save("test01.swf");
?>
