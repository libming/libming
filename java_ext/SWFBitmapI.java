//
// Description:
//    SWFBitmap Interface
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warrantee of liability, merchantability,
//    or fitness for a specific purpose.
//



import SWFObjectI;


//
//  SWFBitmap Interface
//	bitmap in jpg or dbl format
//
//  Notes
//    -	should make this work with PNG transparently
//
public interface SWFBitmapI extends SWFObjectI {
    
    // public SWFBitmap (String filename, String alphamask);

    public float	getWidth ();
    public float	getHeight ();
};



    
