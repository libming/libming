//
// Description:
//    SWFDisplayItem Interface
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
//  SWFDisplayItem Interface
//	item/symbol in movie which can be manipulated
//
//  Notes
//    -	one manipulates these items to set the scene and then writes
//	the frame.  This is how animation is accomplished
//
//    -	the user doesn't instantiate this object, it is rather generated
//	by other interfaces.  Hence the constructor will be private
//
public interface SWFDisplayItemI extends SWFObjectI {
    
    public void		rotate (float degrees);
    public void		rotateTo (float degrees);
    
    public void		move (float x, float y);
    public void		moveTo (float x, float y);

    public void		scale (float scale);
    public void		scaleTo (float scale);
    public void		scaleTo (float xscale, float yscale);

    public void		skewX (float scew);
    public void		skewXTo (float scew);
    public void		skewY (float scew);
    public void		skewYTo (float scew);

    public int		getDepth ();
    public void		setDepth (int depth);

    public void		remove ();

    public void		setName (String name);
    public void		setRatio (float ratio);
    
    public void		addColor (int r, int g, int b, int alpha);
    public void		addColor (int r, int g, int b);

    public void		multColor (float r, float g, float b, float alpha);
    public void		multColor (float r, float g, float b);

    public void		setAlpha (int alpha);
    public void		setColor (int r, int g, int b);
};



    
