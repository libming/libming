//
// Description:
//    SWFText Interface
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
//  SWFText Interface
//	text region
//
//  Notes
//    -	
//
public interface SWFTextI extends SWFObjectI {
    
    public void		setFont (SWFFontI font) throws SWFException;
    public void		setColor (int r, int g, int b, int alpha);
    public void		setColor (SWFColor color);

    public void		addString (String text);

    public void		setHeight (float height);
    public void		setSpacing (float spacing);

    public float	getAscent();
    public float	getDescent();
    public float	getLeading();

    public void		moveTo (float x, float y);
};



    
