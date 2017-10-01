//
// Description:
//    SWFColor Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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
