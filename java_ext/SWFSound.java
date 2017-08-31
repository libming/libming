//
// Description:
//    SWFSound Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
