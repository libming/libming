//
// Description:
//    SWFSound Class
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
//  SWFSound Class
//	create mp3 based sound stream
//
//  Notes
//    -	no methods at the moment other than a constructor
//
public class SWFSound extends SWFObject implements SWFSoundI {
    
    public SWFSound (String file, byte flags)
	throws SWFException
    { 
	setHandle (nNew (file)); 
    }

    protected native int nNew (String file);
};



    
