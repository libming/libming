//
// Description:
//    SWFBitmap Class
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



import java.io.ByteArrayOutputStream;
import java.io.IOException;





//
//  SWFBitmap Class
//	bitmap in jpg or dbl format
//
//  Notes
//    -	should make this work with PNG transparently
//
public class SWFBitmap extends SWFObject implements SWFBitmapI {
    
    public SWFBitmap (String filename)
	throws SWFException
    {
	if (filename.endsWith (".dbl") || filename.endsWith (".DBL"))
	    setHandle (nNewDblBitmap (filename));
	else
	    setHandle (nNewJpegBitmap (filename));
    }
    

    public SWFBitmap (String filename, String alphamask)
	throws SWFException
    {
	setHandle (nNewJpegWithAlpha (filename, alphamask));
    }


    protected void finalize()
	throws Throwable
    {
	// nDestroy (handle);
	super.finalize();
    }


    public int getWidth ()
	{ return nGetWidth (handle); }
    public int getHeight ()
	{ return nGetHeight (handle); }

	
    // native methods

    protected native int	nNewDblBitmap (String filename);	
    protected native int	nNewJpegBitmap (String filename);	
    protected native int	nNewDataBitmap (byte[] data);	
    protected native int	nNewJpegWithAlpha (String filename, String alpha);
    protected native void	nDestroy (int handle);	

    protected native int	nGetWidth (int handle);
    protected native int	nGetHeight (int handle);

};



