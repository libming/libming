//
// Description:
//    SWFMovie Class
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
//  SWFMovie Class
//	top-level swf movie class
//
//  Notes
//    -	too bad the notion of movie and sprite aren't the same.  Effectively
//	they should be.
//
//    -	we should add coordinate system specification and perspective options
//	to give this more flexibility
//
public class SWFMovie extends SWFObject implements SWFMovieI {

    public SWFMovie ()
	throws SWFException
    { 
	setHandle (nNew()); 
    }

    public void	setScale (float scale)
        { nSetScale (scale); }
    public void	setCubicThreshold (int v)
        { nSetCubicThreshold (v); }
    
    public void	setRate (float rate)
        { nSetRate (handle, rate); }
    public void	setDimension (int width, int height)
        { nSetDimension (handle, width, height); }
    
    public void	setNumberOfFrames (int nframes)
        { nSetNumberOfFrames (handle, nframes); }

    public void useSWFVersion( int version )
	    { nUseSWFVersion( version ); }

    public void	setBackground (int r, int g, int b)
        { nSetBackground (handle, r,g,b); }
    public void	setSoundStream (SWFSoundStream sound_stream)
	
        { nSetSoundStream (handle, sound_stream.getHandle()); }


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

    public void	nextFrame()
        { nNextFrame (handle); }
    public void	labelFrame (String label)
        { nLabelFrame (handle, label); }

    public int setCompression(int level) 
    {
	   return nSetCompression(handle, level);
    }
	    
    public void	save (String file)
        { nSave (handle, file); }
    public byte[] output ()
        { return nOutput (handle); }
    public String toString ()
        { return new String (nOutput (handle)); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nSetScale (float scale);
    protected native void	nSetCubicThreshold (int v);
    
    protected native void	nSetRate (int handle, float rate);
    protected native void	nSetDimension (int handle, int width, int height);
    protected native void	nSetNumberOfFrames (int handle, int nframes);
    protected native void 	nUseSWFVersion(int version);

    protected native void	nSetBackground (int handle, int r, int g, int b);
    protected native void	nSetSoundStream (int handle, int HsoundStream);

    protected native int	nAdd (int handle, int Hobject);
    protected native void	nRemove (int handle, int Hitem);

    protected native void	nNextFrame(int handle);
    protected native void	nLabelFrame (int handle, String label);

    protected native int 	nSetCompression(int handle, int level);
    protected native void	nSave (int handle, String file);
    protected native byte[]	nOutput (int handle);

};



    
