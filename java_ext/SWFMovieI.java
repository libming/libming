//
// Description:
//    SWFMovie Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//







//
//  SWFMovie Interface
//	top-level swf movie interface
//
//  Notes
//    -	too bad the notion of movie and sprite aren't the same.  Effectively
//	they should be.
//
//    -	we should add coordinate system specification and perspective options
//	to give this more flexibility
//
public interface SWFMovieI extends SWFObjectI {

    public void			setScale (float scale);
    public void			setCubicThreshold (int v);
    
    public void			setRate (float rate);
    public void			setDimension (int width, int height);
    public void			setNumberOfFrames (int nframes);

    public void			setBackground (int r, int g, int b);
    public void			setSoundStream (SWFSoundStream sound_stream);

    public SWFDisplayItemI	add (SWFObjectI object)			throws SWFException;
    public SWFDisplayItemI	add (SWFObjectI object, int depth)	throws SWFException;
    public void			remove (SWFDisplayItemI item);

    public void			nextFrame();
    public void			labelFrame (String label);

    public int 			setCompression(int level);
    public void			save (String file);
    public byte[]		output ();
};



    
