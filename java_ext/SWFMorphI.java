//
// Description:
//    SWFMorph Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
