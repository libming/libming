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




import java.util.Hashtable;
import java.util.Vector;




//
//  SWFObject Class
//      base object for all swf-entities
//
//  Notes
//    - keeps underlying SWF entity handle, and adjustments
//
//    - a translation / rotation / skew matrix is kept so that objects can
//	be created with specific offset, rotation, etc
//
//    - eval() is provided so that the object can be adjusted or rendered just
//	prior to being added to a parent container (as in MC.add(object))
//
public class SWFObject implements SWFObjectI {

    public SWFObject ()
	throws SWFException
    {
	initialize();
	this.handle = 0;
	this.matrix = null;
	this.props = null;
	this.preserve = null;
    }

    public SWFObject (int handle)
	throws SWFException
    {
	initialize();
	this.handle = handle;
	this.matrix = null;
	this.props = null;
	this.preserve = null;
    }


    // handle functions

    public int getHandle()
        { return handle; }

    public void	setHandle(int handle)
        { this.handle = handle; }


    // intializer function

    public void eval() throws SWFException
        { }


    // matrix functions

    public void setMatrix (SWFMatrix matrix)
    { 
	this.matrix = matrix; 
    }

    public SWFMatrix getMatrix ()
    {
	if (matrix == null)
	    return SWFMatrix.identity();
	else
	    return matrix;
    }


    // property functions

    public Object getProperty (String name)
	throws SWFException
    {
	if (props == null)
	    throw new SWFException ("SWFObject::getProperty: no properties, trying: " + name);
	else
	    return props.get (name);
    }

    public void setProperty (String name, Object value)
    {
	if (props == null)
	    props = new Hashtable();

	props.put (name, value);
    }

    
    public float getFloatProperty (String name)
	throws SWFException
    {
	Object o = getProperty (name);
	if (o == null)
	    throw new SWFException ("SWFObject::getProperty: unknown property: " + name);
	else
	    return ((Float)o).floatValue();
    }

    
    public void setFloatProperty (String name, float v)
    {
	setProperty (name, new Float (v));
    }
	


    // initializer & GC related

    public static synchronized void initialize ()
	throws SWFException
    {
	if (initialized) 
	    return;
	else
	    initialized = true;

	try
            { System.loadLibrary ("jswf"); }

	catch (UnsatisfiedLinkError e)
	{
	    System.out.println(e.toString());
	    String msg = e.getMessage();
	    if (msg.indexOf ("already loaded") < 0)
		throw new SWFException ("native loading: " + msg);
	}
    }


    public void preserve (SWFObjectI obj)
    {
	if (preserve == null)
	    preserve = new Vector();
	preserve.add (obj);
    }


    // variables

    protected int		handle;
    protected SWFMatrix		matrix;
    protected Hashtable		props;
    protected Vector		preserve;

    protected static boolean	initialized = false;
};
