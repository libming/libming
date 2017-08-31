//
// Description:
//    SWFBitmap Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//


//
//  SWFBitmap Interface
//	bitmap in jpg or dbl format
//
//  Notes
//    -	should make this work with PNG transparently
//
public interface SWFBitmapI extends SWFObjectI {
    
    // public SWFBitmap (String filename, String alphamask);

    public int getWidth ();
    public int getHeight ();
};



    
