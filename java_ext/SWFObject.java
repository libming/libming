//
// Description:
//    SWFObject Class
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
//  SWFObject Class
//      base object for all swf-entities
//
//  Notes
//    - keeps underlying SWF entity handle, and adjustments
//
//    - dx and dy are kept so that an object creation function can specify a
//	relative offset when being contained by a movie, movie-clip, etc.  This
//	is especially useful if you want to use center coordinates.
//
//    - eval() is provided so that the object can be adjusted or rendered just
//	prior to being added to a parent container (as in MC.add(object))
//
public class SWFObject implements SWFObjectI {

    public SWFObject ()
    {
	initialize();
	this.handle = 0;
	this.dx = 0.0f;
	this.dy = 0.0f;
    }

    public SWFObject (int handle)
    {
	initialize();
	this.handle = handle;
	this.dx = 0.0f;
	this.dy = 0.0f;
    }


    // handle functions

    public int getHandle()
        { return handle; }

    public void	setHandle(int handle)
        { this.handle = handle; }


    // intializer function

    public void eval() throws SWFException
        { }

    // offset functions

    public void setXOffset (float dx)
        { this.dx = dx; }

    public void setYOffset (float dy)
        { this.dy = dy; }

    public void setOffset (float dx, float dy)
        { this.dx = dx; this.dy = dy; }

    public float getXOffset ()
        { return dx; }

    public float getYOffset ()
        { return dy; }

    public boolean shifted ()
        { return (dx != 0.0f) || (dy != 0.0f); }


    // initializer

    public static void initialize ()
    {
	if (initialized) return;
        System.loadLibrary ("jswf");
	initialized = false;
    }


    // variables

    protected int	handle;
    protected float	dx;
    protected float	dy;

    protected static boolean initialized = false;
};
