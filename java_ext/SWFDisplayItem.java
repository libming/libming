//
// Description:
//    SWFDisplayItem Class
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
//  SWFDisplayItem Class
//	item/symbol in movie which can be manipulated
//
//  Notes
//    -	one manipulates these items to set the scene and then writes
//	the frame.  This is how animation is accomplished
//
//    -	the user doesn't instantiate this object, it is rather generated
//	by other interfaces.  Hence the constructor will be private
//
public class SWFDisplayItem extends SWFObject implements SWFDisplayItemI {

    public SWFDisplayItem (int handle)
	throws SWFException
    { 
	super (handle); 
    }
    
    public void	rotate (float degrees)
        { nRotate (handle, degrees); }
    public void	rotateTo (float degrees)
        { nRotateTo (handle, degrees); }
    
    public void	move (float x, float y)
        { nMove (handle, x, y); }
    public void	moveTo (float x, float y)
        { nMoveTo (handle, x, y); }

    public void	scale (float scale)
        { nScale (handle, scale, scale); }
    public void	scale (float xscale, float yscale)
        { nScale (handle, xscale, yscale); }
    public void	scaleTo (float scale)
        { nScaleTo (handle, scale, scale); }
    public void	scaleTo (float xscale, float yscale)
        { nScaleTo (handle, xscale, yscale); }

    public void	skewX (float scew)
        { nSkewX (handle, scew); }
    public void	skewXTo (float scew)
        { nSkewXTo (handle, scew); }
    public void	skewY (float scew)
        { nSkewY (handle, scew); }
    public void	skewYTo (float scew)
        { nSkewYTo (handle, scew); }

    public int	getDepth ()
        { return nGetDepth (handle); }
    public void	setDepth (int depth)
        { nSetDepth (handle, depth); }

    public void	remove ()
        { nRemove (handle); }

    public void	setName (String name)
        { nSetName (handle, name); }
    public void	setRatio (float ratio)
        { nSetRatio (handle, ratio); }
    
    public void	addColor (int r, int g, int b, int alpha)
        { nAddColor (handle, r,g,b, alpha); }
    public void	addColor (int r, int g, int b)
        { nAddColor (handle, r,g,b, 0); }
    public void	multColor (float r, float g, float b, float alpha)
        { nMultColor (handle, r,g,b, alpha); }
    public void	multColor (float r, float g, float b)
        { nMultColor (handle, r,g,b, 1f); }


    public void setAlpha (int alpha)
        { multColor (1f,1f,1f,0f); addColor(0,0,0,alpha); }
    public void setColor (int r, int g, int b)
        { multColor (0f,0f,0f,1f); addColor(r,g,b); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native methods

    protected native void	nDestroy (int handle);
    protected native void	nRotate (int handle, float degrees);
    protected native void	nRotateTo (int handle, float degrees);
    
    protected native void	nMove (int handle, float x, float y);
    protected native void	nMoveTo (int handle, float x, float y);

    protected native void	nScale (int handle, float xscale, float yscale);
    protected native void	nScaleTo (int handle, float xscale, float yscale);

    protected native void	nSkewX (int handle, float scew);
    protected native void	nSkewXTo (int handle, float scew);
    protected native void	nSkewY (int handle, float scew);
    protected native void	nSkewYTo (int handle, float scew);

    protected native int	nGetDepth (int handle);
    protected native void	nSetDepth (int handle, int depth);

    protected native void	nRemove (int handle);

    protected native void	nSetName (int handle, String name);
    protected native void	nSetRatio (int handle, float ratio);
    
    protected native void	nAddColor (int handle, int r, int g, int b, int alpha);
    protected native void	nMultColor (int handle, float r, float g, float b, float alpha);
    
};



    
