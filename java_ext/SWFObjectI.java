//
// Description:
//    SWFObject Interface
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
//    or fitness for any specific purpose.
//







//
//  SWFObject Interface
//      base object for all swf-entities
//
//  Notes
//    - keeps underlying SWF entity handle and attributes common to
//	all objects
//
//    - eval() allows one to do any processing before an object is used
//	by another (often used for rendering).
//
//    - added offset, allowing functions to embed offset information
//	when creating an object, so that later when adding to an MC,
//	will be moved by offset amount
//
public interface SWFObjectI {

    public int		getHandle();
    public void		setHandle(int handle);

    public void		eval() throws SWFException;

    public Object	getProperty (String name)			throws SWFException;
    public void		setProperty (String name, Object value);

    public float	getFloatProperty (String name)			throws SWFException;
    public void		setFloatProperty (String name, float value);

    public void		setMatrix (SWFMatrix mat);
    public SWFMatrix	getMatrix ();
};
