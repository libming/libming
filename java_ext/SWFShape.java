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
//    This software is distributed with no warranty of liability, merchantability,
//    or fitness for a specific purpose.
//






//
//  SWFShape Class
//      general polygon / poly-curve region
//
//  Notes
//    - building block for flash objects
//
public class SWFShape extends SWFObject implements SWFShapeI {

    public final static float PI = 3.1415926f;

    // public methods
    
    public SWFShape (int handle)
	throws SWFException
    { 
	super (handle); 
	this.lwidth = 1;
	this.color = SWFColor.identity();
    }

    public SWFShape ()
    	throws SWFException
    { 
	setHandle (nNew()); 
	this.lwidth = 1;
	this.color = SWFColor.identity();
    }

    public float getPenX()
    {
	    return nGetPenX(handle);
    }

    public float getPenY()
    {
	    return nGetPenY(handle);
    }

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
        { iDrawArc (handle, r, startAngle, endAngle); }
    public void	drawCircle (float r)
        { iDrawCircle (handle, r); }

    public void	drawGlyph (SWFFontI font, int c)
	throws SWFException
    { 
	font.eval(); 
	nDrawGlyph (handle, font.getHandle(), c); 
	preserve (font);
    }


    public void	end ()
        { nEnd (handle); }


    // color & line thickness

    public void	setLine (short width, int r, int g, int b, int alpha)
    { 
	lwidth = width;
	color = new SWFColor(r,g,b, alpha);
	nSetLine (handle, width, r,g,b,alpha); 
    }

    public void	setLine (short width, SWFColor color)
    { 
	lwidth = width;
	this.color = color;
	nSetLine (handle, width, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); 
    }

    public void	setLine (short width)
    { 
	lwidth = width;
	setLine (width, color);
    }

    public void	setColor (SWFColor color)
    { 
	this.color = color;
	setLine (lwidth, color);
    }


    // fills

    public SWFFillI addBitmapFill (SWFBitmapI bitmap, int flags)
	throws SWFException
    { 
	bitmap.eval(); 
	preserve (bitmap);
	return new SWFFill (nAddBitmapFill (handle, bitmap.getHandle(), (short)flags)); 
    }

    public SWFFillI addGradientFill (SWFGradientI gradient, int flags)
	throws SWFException
    { 
	gradient.eval(); 
	preserve (gradient);
	return new SWFFill (nAddGradientFill (handle, gradient.getHandle(), (short)flags)); 
    }

    public SWFFillI addSolidFill (int r, int g, int b, int alpha)
	throws SWFException
        { return new SWFFill (nAddSolidFill (handle, r,g,b, alpha)); }
    public SWFFillI addSolidFill (SWFColor color)
	throws SWFException
        { return new SWFFill (nAddSolidFill (handle, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha())); }

    public void	setLeftFill (SWFFillI fill)
	throws SWFException
    { 
	fill.eval();
	preserve (fill);
	nSetLeftFill (handle, fill.getHandle()); 
    }

    public void	setRightFill (SWFFillI fill)
	throws SWFException
    { 
	fill.eval(); 
	preserve (fill);
	nSetRightFill (handle, fill.getHandle()); 
    }


    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }


    // internal stuff

    
    protected void iDrawCircle (int handle, float r)
    {
	final float a = r * (float)Math.tan (PI/8);
	final float b = r * (float)Math.sin (PI/4);

	movePen (r, 0);
	drawCurve (0, -a, b-r, -b+a);
	drawCurve (-b+a, b-r, -a, 0);
	drawCurve (-a, 0, a-b, r-b);
	drawCurve (b-r, b-a, 0, a);
	drawCurve (0,a, r-b, b-a);
	drawCurve (b-a,r-b,a,0);
	drawCurve (a,0,b-a,b-r);
	drawCurve (r-b,a-b,0,-a);
	movePen (-r, 0);
    }


    protected void iDrawArc (int handle, float r, float startAngle, float endAngle)
    {
	nDrawArc (handle,r,startAngle,endAngle);
    }
    
    

    // native stuff


    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nMovePen (int handle, float x, float y);
    protected native void	nMovePenTo (int handle, float x, float y);
    protected native float	nGetPenX(int handle );
    protected native float 	nGetPenY(int handle );

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

    

    // variables

    protected short		lwidth;
    protected SWFColor		color;
};



    

