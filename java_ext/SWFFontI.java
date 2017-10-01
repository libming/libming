//
// Description:
//    SWFFont Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//







//
//  SWFFont Interface
//	font specification & attributes
//
//  Notes
//    -	
//
public interface SWFFontI extends SWFObjectI {
    
    // public SWFont (String font);

    public float	getStringWidth (String string);
    public float	getAscent ();
    public float	getDescent ();
    public float	getLeading ();
};



    
