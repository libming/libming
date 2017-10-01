//
// Description:
//    SWFMovieClip Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
