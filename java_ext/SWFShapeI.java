//
// Description:
//    SWFShape Interface
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
//  SWFShape Interface
//      general polygon / poly-curve region
//
//  Notes
//    - building block for flash objects
//
public interface SWFShapeI extends SWFObjectI {
    
    public void		movePen (float x, float y);
    public void		movePenTo (float x, float y);

    public void		drawLine (float x, float y);
    public void		drawLineTo (float x, float y);
    public void		drawLineToRelative (float dx, float dy);

    public void		drawCurve (float bx, float by, float cx, float cy);
    public void		drawCurveTo (float bx, float by, float cx, float cy);

    public void		drawCurve (float bx, float by, float cx, float cy, float dx, float dy);
    public void		drawCurveTo (float bx, float by, float cx, float cy, float dx, float dy);

    public void		drawArc (float r, float startAngle, float endAngle);
    public void		drawCircle (float r);
    public void		drawGlyph (SWFFontI font, int c) throws SWFException;

    public void		end ();

    public void		setLine (short width, int r, int g, int b, int alpha);
    public void		setLine (short width, SWFColor color);

    public void		setLine (short width);
    public void		setColor (SWFColor color);

    public SWFFillI	addBitmapFill (SWFBitmapI bitmap, int flags) throws SWFException;
    public SWFFillI	addGradientFill (SWFGradientI gradient, int flags) throws SWFException;
    public SWFFillI	addSolidFill (int r, int g, int b, int alpha) throws SWFException;
    public SWFFillI	addSolidFill (SWFColor color) throws SWFException;

    public void		setLeftFill (SWFFillI fill) throws SWFException;
    public void		setRightFill (SWFFillI fill) throws SWFException;
};



    

