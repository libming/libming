//
// Description:
//    SWFException Class
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
//  SWFException Class
//      exception from swf generation or rendering
//
//  Notes
//    - 
//
public class SWFException extends Exception {

    public SWFException (String err)
	throws SWFException
    { 
	this.err = err; 
    }

    public String toString ()
        { return "SWF: " + err; }


    // variables

    protected String	err;
};
