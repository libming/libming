//
// Description:
//    SWFMovieClip Interface
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
//  SWFMovieClip Interface
//	movie clip
//
//  Notes
//    -	one simply draws the shapes and adds to the movie at a specific
//	frame
//
public interface SWFMovieClipI extends SWFObjectI {
    
    public void			setFrames (int nframes);
    
    public SWFDisplayItemI	add (SWFObjectI object)			throws SWFException;
    public SWFDisplayItemI	add (SWFObjectI object, int depth)	throws SWFException;
    public void			remove (SWFDisplayItemI item);

    public void			nextFrame ();
    public void			labelFrame (String label);
};



    
