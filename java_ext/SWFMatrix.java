//
// Description:
//    SWFMatrix Class
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
//  SWFMatrix Interface
//      a rotation, translation matrix
//
//  Notes
//    - 
//
public class SWFMatrix {

    public SWFMatrix ()
    {
	this.rot = 0f;
	this.dx = 0f;
	this.dy = 0f;
	this.xscale = 1.0f;
	this.yscale = 1.0f;
	this.xskew = 0.0f;
	this.yskew = 0.0f;
	this.applied = false;
    }

    
    // accessors
    
    public float getRotation ()
        { return rot; }
    public void setRotation (float deg)
        { rot = deg; }
    
    public float getDx ()
        { return dx; }
    public void setDx (float dx)
        { this.dx = dx; }
    
    public float getDy ()
        { return dy; }
    public void setDy (float dy)
        { this.dy = dy; }
    
    public float getXScale ()
        { return xscale; }
    public void setXScale (float xscale)
        { this.xscale = xscale; }
    
    public float getYScale ()
        { return yscale; }
    public void setYScale (float yscale)
        { this.yscale = yscale; }
    
    public float getXSkew ()
        { return xskew; }
    public void setXSkew (float xskew)
        { this.xskew = xskew; }
    
    public float getYSkew ()
        { return yskew; }
    public void setYSkew (float yskew)
        { this.yskew = yskew; }
    

    // apply methods

    public void apply (SWFDisplayItemI item, boolean force)
    {
	if (applied && !force)
	    return;
	else
	    applied = true;

	if (NE (this.dx, 0.0f) || NE (this.dy, 0.0f))
	    item.move (dx, dy);
	if (NE (this.xscale, 1.0f) || NE (this.yscale, 1.0f))
	    item.scaleTo (xscale, yscale);
	if (NE (this.xskew, 0.0f))
	    item.skewX (xskew);
	if (NE (this.yskew, 0.0f))
	    item.skewY (yskew);
	if (NE (this.rot, 0f))
	    item.rotate (rot);
    }

    
    public void apply (SWFDisplayItemI item)
        { apply (item, false); }

    public boolean applied ()
        { return applied; }



    // class methods

    public static SWFMatrix identity()
        { return identity; }

    private static boolean NE (float a, float b)
        { return Math.abs (a-b) < 1E-5; }


    // variables

    protected float		dx;
    protected float		dy;

    protected float		xscale;
    protected float		yscale;

    protected float		xskew;
    protected float		yskew;

    protected float		rot;
    protected boolean		applied;


    // class variables

    protected static SWFMatrix	identity;

    static {
	identity = new SWFMatrix();
	identity.applied = true;
    }
};
