//
// Description:
//    SWFGradient Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//




//
//  SWFGradient Interface
//	gradient
//
//  Notes
//    -	this object is not instantiated by the user rather is generated
//	when a gradient fill is specified on a path-based entity.
//
public interface SWFGradientI extends SWFObjectI {
    
  public void addEntry( float ratio, SWFColor clr );
    public void	addEntry (float ratio, int r, int g, int b, int alpha);

};



    
