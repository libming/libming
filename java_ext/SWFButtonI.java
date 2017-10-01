//
// Description:
//    SWFButton Interface
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//






//
//  SWFButton Interface
//	button (region with actions)
//
//  Notes
//    -	
//
public interface SWFButtonI extends SWFObjectI {
    
    // states

    public final static int ButtonHit = (1<<3);
    public final static int ButtonDown = (1<<2);
    public final static int ButtonOver = (1<<1);
    public final static int ButtonUp = (1<<0);
    
    // action triggers

    public final static int MouseUpOutside = (1<<6);
    public final static int DragOver = (1<<5);
    public final static int DragOut = (1<<4);
    public final static int MouseUp = (1<<3);
    public final static int MouseDown = (1<<2);
    public final static int MouseOut = (1<<1);
    public final static int MouseOver = (1<<0);


    // methods

    public void		setUp (SWFShapeI shape)				throws SWFException;
    public void		setDown (SWFShapeI shape)			throws SWFException;
    public void		setOver (SWFShapeI shape)			throws SWFException;
    public void		setHit (SWFShapeI shape)			throws SWFException;

    public void		addShape (int which, SWFShapeI shape)		throws SWFException;
    public void		addAction (SWFActionI action, int trigger)	throws SWFException;
};



    
