//
// Description:
//    SWFFill Class
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
//  SWFFill Class
//      general fill container
//
//  Notes
//    -	this object is not instantiated by the user rather is generated
//	when a fill is specified on a path-based entity.
//
public class SWFFill extends SWFObject implements SWFFillI {

    public SWFFill (int handle)
	throws SWFException
    { 
	super (handle); 
    }


    public void	skewX (float skew)
        { nSkewX (handle, skew); }
    public void	skewXTo (float skew)
        { nSkewXTo (handle, skew); }

    public void	skewY (float skew)
        { nSkewY (handle, skew); }
    public void	skewYTo (float skew)
        { nSkewYTo (handle, skew); }

    public void	scaleX (float scale)
        { nScaleX (handle, scale); }
    public void	scaleXTo (float scale)
        { nScaleXTo (handle, scale); }

    public void	scaleY (float scale)
        { nScaleY (handle, scale); }
    public void	scaleYTo (float scale)
        { nScaleYTo (handle, scale); }

    public void	scale (float xscale, float yscale)
        { nScale (handle, xscale, yscale); }
    public void	scale (float scale)
        { nScale (handle, scale, scale); }

    public void	rotate (float degrees)
        { nRotate (handle, degrees); }
    public void	rotateTo (float degrees)
        { nRotateTo (handle, degrees); }

    public void	move (float x, float y)
        { nMove (handle, x, y); }
    public void	moveTo (float x, float y)
        { nMoveTo (handle, x, y); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native methods

    protected native void	nDestroy (int handle);

    protected native void	nSkewX (int handle, float skew);
    protected native void	nSkewXTo (int handle, float skew);

    protected native void	nSkewY (int handle, float skew);
    protected native void	nSkewYTo (int handle, float skew);

    protected native void	nScaleX (int handle, float scale);
    protected native void	nScaleXTo (int handle, float scale);

    protected native void	nScaleY (int handle, float scale);
    protected native void	nScaleYTo (int handle, float scale);

    protected native void	nScale (int handle, float xscale, float yscale);

    protected native void	nRotate (int handle, float degrees);
    protected native void	nRotateTo (int handle, float degrees);

    protected native void	nMove (int handle, float x, float y);
    protected native void	nMoveTo (int handle, float x, float y);
};



    




    

