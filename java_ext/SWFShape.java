//
// Description:
//    SWFShape Class
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warrantee of liability, merchantability,
//    or fitness for a specific purpose.
//



import SWFShapeI;



//
//  SWFShape Class
//      general polygon / poly-curve region
//
//  Notes
//    - building block for flash objects
//
public class SWFShape extends SWFObject implements SWFShapeI {

    // public methods
    
    public SWFShape (int handle)
        { super (handle); }

    public SWFShape ()
        { setHandle (nNew()); }


    public void	movePen (float x, float y)
        { nMovePen (handle, x, y); }
    public void	movePenTo (float x, float y)
        { nMovePenTo (handle, x, y); }


    public void	drawLine (float x, float y)
        { nDrawLine (handle, x, y); }
    public void	drawLineTo (float x, float y)
        { nDrawLineTo (handle, x, y); }
    public void	drawLineToRelative (float dx, float dy)
        { nDrawLineToRelative (handle, dx, dy); }


    public void	drawCurve (float bx, float by, float cx, float cy)
        { nDrawCurve (handle, bx, by, cx, cy); }
    public void	drawCurveTo (float bx, float by, float cx, float cy)
        { nDrawCurveTo (handle, bx, by, cx, cy); }

    public void	drawCurve (float bx, float by, float cx, float cy, float dx, float dy)
        { nDrawCubic (handle, bx, by, cx, cy, dx, dy); }
    public void	drawCurveTo (float bx, float by, float cx, float cy, float dx, float dy)
        { nDrawCubicTo (handle, bx, by, cx, cy, dx, dy); }


    public void	drawArc (float r, float startAngle, float endAngle)
        { nDrawArc (handle, r, startAngle, endAngle); }
    public void	drawCircle (float r)
        { nDrawCircle (handle, r); }
    public void	drawGlyph (SWFFontI font, int c)
	throws SWFException
        { font.eval(); nDrawGlyph (handle, font.getHandle(), c); }


    public void	end ()
        { nEnd (handle); }


    public void	setLine (short width, int r, int g, int b, int alpha)
        { nSetLine (handle, width, r,g,b, alpha); }
    public SWFFillI addBitmapFill (SWFBitmapI bitmap, int flags)
	throws SWFException
        { bitmap.eval(); return new SWFFill (nAddBitmapFill (handle, bitmap.getHandle(), (short)flags)); }
    public SWFFillI addGradientFill (SWFGradientI gradient, int flags)
	throws SWFException
        { gradient.eval(); return new SWFFill (nAddGradientFill (handle, gradient.getHandle(), (short)flags)); }
    public SWFFillI addSolidFill (int r, int g, int b, int alpha)
        { return new SWFFill (nAddSolidFill (handle, r,g,b, alpha)); }

    public void	setLeftFill (SWFFillI fill)
	throws SWFException
        { fill.eval(); nSetLeftFill (handle, fill.getHandle()); }
    public void	setRightFill (SWFFillI fill)
	throws SWFException
        { fill.eval(); nSetRightFill (handle, fill.getHandle()); }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // native stuff


    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nMovePen (int handle, float x, float y);
    protected native void	nMovePenTo (int handle, float x, float y);

    protected native void	nDrawLine (int handle, float x, float y);
    protected native void	nDrawLineTo (int handle, float x, float y);
    protected native void	nDrawLineToRelative (int handle, float dx, float dy);

    protected native void	nDrawCurve (int handle, float bx, float by, float cx, float cy);
    protected native void	nDrawCurveTo (int handle, float bx, float by, float cx, float cy);

    protected native void	nDrawCubic (int handle, float bx, float by, float cx, float cy, float dx, float dy);
    protected native void	nDrawCubicTo (int handle, float bx, float by, float cx, float cy, float dx, float dy);

    protected native void	nDrawArc (int handle, float r, float startAngle, float endAngle);
    protected native void	nDrawCircle (int handle, float r);
    protected native void	nDrawGlyph (int handle, int Hfont, int c);

    protected native void	nEnd (int handle);

    protected native void	nSetLine (int handle, short width, int r, int g, int b, int alpha);
    protected native int	nAddBitmapFill (int handle, int Hbitmap, short flags);
    protected native int	nAddGradientFill (int handle, int Hgradient, short flags);
    protected native int	nAddSolidFill (int handle, int r, int g, int b, int alpha);

    protected native void	nSetLeftFill (int handle, int Hfill);
    protected native void	nSetRightFill (int handle, int Hfill);
};



    

