//
// Description:
//    SWFException Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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
