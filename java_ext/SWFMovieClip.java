//
// Description:
//    SWFMovieClip Class
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warranty of liability, merchantability,
//    or fitness for a specific purpose.
//







//
//  SWFMovieClip Class
//	movie clip
//
//  Notes
//    -	one simply draws the shapes and adds to the movie at a specific
//	frame
//
public class SWFMovieClip extends SWFObject implements SWFMovieClipI {
    
    public SWFMovieClip ()
	throws SWFException
    { 
	setHandle (nNew()); 
    }

    public void	setFrames (int nframes)
        { nSetFrames (handle, nframes); }
    public void	setNumberOfFrames (int nframes)
        { nSetFrames (handle, nframes); }
    
    public SWFDisplayItemI add (SWFObjectI object)
	throws SWFException
    { 
	object.eval();
	preserve (object);
	SWFDisplayItemI i = 
	    new SWFDisplayItem (nAdd (handle, object.getHandle()));
	object.getMatrix().apply (i);

	return i; 
    }

    public SWFDisplayItemI add (SWFObjectI object, int depth) 
	throws SWFException
    { 
	SWFDisplayItemI i = add (object); 
	i.setDepth(depth); 
	return i; 
    }

    public void	remove (SWFDisplayItemI item)
        { nRemove (handle, item.getHandle()); }

    public void	nextFrame ()
        { nNextFrame (handle); }
    public void	labelFrame (String label)
        { nLabelFrame (handle, label); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nSetFrames (int handle, int nframes);

    protected native int	nAdd (int handle, int Hobject);
    protected native void	nRemove (int handle, int Hobject);

    protected native void	nNextFrame (int handle);
    protected native void	nLabelFrame (int handle, String label);
};



    
