//
// Description:
//    SWFGradient Interface
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



    
