//
// Description:
//    SWFFill Interface
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
//  SWFFill Interface
//      general fill container
//
//  Notes
//    -	this object is not instantiated by the user rather is generated
//	when a fill is specified on a path-based entity.
//
public interface SWFFillI extends SWFObjectI {

    // constants
    public final static int	Solid = 0x00; 
    public final static int	Gradient = 0x10; 
    public final static int	LinearGradient = 0x10; 
    public final static int	RadialGradient = 0x12; 
    public final static int	Bitmap = 0x40; 
    public final static int	TiledBitmap = 0x40; 
    public final static int	ClippedBitmap = 0x41; 
    public final static int     NonSmoothedTiledBitmap = 0x42;
    public final static int     NonSmoothedClippedBitmap = 0x43;
    

    // methods

    public void		skewX (float skew);
    public void		skewXTo (float skew);

    public void		skewY (float skew);
    public void		skewYTo (float skew);

    public void		scaleX (float scale);
    public void		scaleXTo (float scale);

    public void		scaleY (float scale);
    public void		scaleYTo (float scale);

    public void		scale (float xscale, float yscale);
    public void		scale (float scale);

    public void		rotate (float degrees);
    public void		rotateTo (float degrees);

    public void		move (float x, float y);
    public void		moveTo (float x, float y);
};



    




    

