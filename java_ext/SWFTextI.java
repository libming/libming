//
// Description:
//    SWFText Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
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



    
