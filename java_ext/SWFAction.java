//
// Description:
//    SWFAction Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
