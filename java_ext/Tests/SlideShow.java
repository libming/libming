//
// Description:
//    SlideShow Test
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
//  SlideShow Test
//      flip between two slides, first slide alpha transitioned
//
//  Notes
//    - substitute with your own jpgs (and dimensions)
//
public class SlideShow {

    private static SWFShape GetImage (String file, int width, int height)
	throws SWFException
    {
	SWFShape image = new SWFShape ();

	// do fill (red)
	SWFFillI f = image.addBitmapFill (new SWFBitmap (file), SWFFillI.ClippedBitmap);
	image.setRightFill (f);

	// create box
	image.movePenTo (0,0);
	image.drawLineTo (width,0);
	image.drawLineTo (width,height);
	image.drawLineTo (0,height);
	image.drawLineTo (0,0);

	return image;
    }


    public static void main (String[] argv)
        throws Exception
    {
	SWFShape Ia = GetImage ("images/Burberry-h1.jpg", 524, 800);
	SWFShape Ib = GetImage ("images/Burberry-h2.jpg", 524, 800);
  
	// add box to movie clip
	SWFMovieClip clip = new SWFMovieClip ();

	SWFDisplayItemI Da = clip.add(Ia, 1);
	Da.setAlpha (0);
	SWFDisplayItemI Db = clip.add(Ib, 2);
	Db.setAlpha (0);

	for (int i = 0 ; i < 20; i++) {
	    Da.setAlpha ((255/20)*i);
	    clip.nextFrame();
	}

	Db.setAlpha (255);
	for (int i = 0 ; i < 20; i++) clip.nextFrame();

	// create movie
	SWFMovie movie = new SWFMovie();
	movie.setBackground(0xff, 0xff, 0xff);
	movie.setDimension(524,800);

	// add movie clip to main movie and position
	SWFDisplayItemI Citem = movie.add (clip, 1);
	Citem.moveTo(0,0);

	// save to file
	movie.save ("images.swf");
    }
};
