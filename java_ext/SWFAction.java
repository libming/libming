//
// Description:
//    SWFAction Class
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
//  SWFAction Class
//	create action sequence
//
//  Notes
//    -	parses and compiles action
//
public class SWFAction extends SWFObject implements SWFActionI {
    
    public SWFAction (String script)
	throws SWFException
    {
	setHandle (nNew (script));
    }

    protected native int nNew (String script);
};



    
