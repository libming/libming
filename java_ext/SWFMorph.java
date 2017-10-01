//
// Description:
//    SWFMorph Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
