//
// Description:
//    SWFText Class
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
//  SWFText Class
//	text region
//
//  Notes
//    -	
//
public class SWFText extends SWFObject implements SWFTextI {
    
    public SWFText ()
	throws SWFException
    { 
	setHandle (nNew()); 
    }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    public void	setFont (SWFFontI font) 
	throws SWFException
    { 
	font.eval(); 
	nSetFont (handle, font.getHandle());
	preserve (font);
    }

    public void	setColor (int r, int g, int b, int alpha)
        { nSetColor (handle, r,g,b, alpha); }
    public void	setColor (SWFColor color)
        { nSetColor (handle, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }

    public void	addString (String text)
        { nAddString (handle, text); }

    public void	setHeight (float height)
        { nSetHeight (handle, height); }
    public void	setSpacing (float spacing)
        { nSetSpacing (handle, spacing); }

    public float getAscent()
        { return nGetAscent (handle); }
    public float getDescent()
        { return nGetDescent (handle); }
    public float getLeading()
        { return nGetLeading (handle); }

    public void	moveTo (float x, float y)
        { nMoveTo (handle, x,y); }



    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nSetFont (int handle, int Hfont);
    protected native void	nSetColor (int handle, int r, int g, int b, int alpha);

    protected native void	nAddString (int handle, String text);

    protected native void	nSetHeight (int handle, float height);
    protected native void	nSetSpacing (int handle, float spacing);

    protected native float	nGetAscent(int handle);
    protected native float	nGetDescent(int handle);
    protected native float	nGetLeading(int handle);

    protected native void	nMoveTo (int handle, float x, float y);

};



    
