//
// Description:
//    SWFColor Class
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warranty of liability, merchantability,
//    or fitness for any specific purpose.
//



//
//  SWFColor Interface
//      color & alpha
//
//  Notes
//    - 
//
public class SWFColor {

    public SWFColor (int r, int g, int b, int alpha)
    {
	this.r = r;
	this.g = g;
	this.b = b;
	this.alpha = alpha;
    }

    public SWFColor (int r, int g, int b)
    {
	this.r = r;
	this.g = g;
	this.b = b;
	this.alpha = 0xff;
    }

    public SWFColor ()
    {
	this.r = 0x00;
	this.g = 0x00;
	this.b = 0x00;
	this.alpha = 0xff;
    }

    
    // accessors
    
    public int getRed ()
        { return r; }
    public void setRed (int r)
        { this.r = r; }
    
    public int getGreen ()
        { return g; }
    public void setGreen (int g)
        { this.g = g; }
    
    public int getBlue ()
        { return b; }
    public void setBlue (int b)
        { this.b = b; }
    
    public int getAlpha ()
        { return alpha; }
    public void setAlpha (int alpha)
        { this.alpha = alpha; }


    public static SWFColor identity()
        { return identity; }
    
    
    // variables

    protected int	r;
    protected int	g;
    protected int	b;
    protected int	alpha;


    // static stuff

    protected static SWFColor	identity = new SWFColor();
};
