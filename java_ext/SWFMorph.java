//
// Description:
//    SWFMorph Class
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
//  SWFMorph Class
//	morphing between two objects
//
//  Notes
//    -	one simply draws the two shapes and adds to the movie at a specific
//	frame
//
public class SWFMorph extends SWFObject implements SWFMorphI {

    public SWFMorph () 
	throws SWFException
    { 
	setHandle (nNew()); 
    }
    
    public SWFShapeI getShape1 () throws SWFException
        { return new SWFShape (nGetShape1 (handle)); }
    public SWFShapeI getShape2 () throws SWFException
        { return new SWFShape (nGetShape2 (handle)); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native int	nGetShape1 (int handle);
    protected native int	nGetShape2 (int handle);
};



    
