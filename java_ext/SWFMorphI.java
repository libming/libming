//
// Description:
//    SWFMorph Interface
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
//  SWFMorph Interface
//	morphing between two objects
//
//  Notes
//    -	one simply draws the two shapes and adds to the movie at a specific
//	frame
//
public interface SWFMorphI extends SWFObjectI {
    
    public SWFShapeI	getShape1 ()	throws SWFException;
    public SWFShapeI	getShape2 ()	throws SWFException;
};



    
