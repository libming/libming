//
// Description:
//    Simple Test
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warranty of liability, merchantability,
//    or fitness for a specific purpose.
//


//
//  Simple SWF Test
//      
//
//  Notes
//    - 
//
public class Simple {

    public static void main (String[] argv)
        throws Exception
    {
	SWFShape s = new SWFShape ();

	// do fill (red)
	SWFFillI f = s.addSolidFill (0xff, 0x00, 0x00, 0xff);
	s.setRightFill (f);

	// create box
	s.movePenTo(-500,-500);
	s.drawLineTo(500,-500);
	s.drawLineTo(500,500);
	s.drawLineTo(-500,500);
	s.drawLineTo(-500,-500);
  
	// add box to movie clip
	SWFMovieClip clip = new SWFMovieClip ();
	SWFDisplayItemI Bitem = clip.add(s);
	Bitem.setDepth(1);
	clip.nextFrame();

	// rotate item 15 degrees over 5 frames
	for (int n = 0; n < 5; n++) {
	    Bitem.rotate (-15);
	    clip.nextFrame();
	}

	// create movie
	SWFMovie movie = new SWFMovie();
	movie.setBackground(0xff, 0xff, 0xff);
	movie.setDimension(6000,4000);

	// add movie clip to main movie and position
	SWFDisplayItemI Citem = movie.add (clip);
	Citem.setDepth(1);
	Citem.moveTo(-500,2000);
	Citem.setName("box");

	// actions
	movie.add(new SWFAction ("box.x += 3;"));
	movie.nextFrame();
	movie.add(new SWFAction ("gotoFrame(0); play();"));
	movie.nextFrame();

	// save to file
	movie.save ("simple.swf", -1);
    }
};
