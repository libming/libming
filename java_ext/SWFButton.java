//
// Description:
//    SWFButton Class
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
//  SWFButton Class
//	button (region with actions)
//
//  Notes
//    -	
//
public class SWFButton extends SWFObject implements SWFButtonI {
    
    public SWFButton ()
	throws SWFException
    {
	setHandle (nNew());
    }

    protected void finalize()
	throws Throwable
    {
	// nDestroy (handle);
	super.finalize();
    }



    // methods

    public void	setUp (SWFShapeI shape)
	throws SWFException
        { shape.eval(); addShape (SWFButton.ButtonUp, shape); }
    public void	setDown (SWFShapeI shape)
	throws SWFException
        { shape.eval(); addShape (SWFButton.ButtonDown, shape); }
    public void	setOver (SWFShapeI shape)
	throws SWFException
        { shape.eval(); addShape (SWFButton.ButtonOver, shape); }
    public void	setHit (SWFShapeI shape)
	throws SWFException
        { shape.eval(); addShape (SWFButton.ButtonHit, shape); }

    public void	addShape (int which, SWFShapeI shape)
	throws SWFException
        { shape.eval(); nAddShape (handle, shape.getHandle(), which); }

    public void	addAction (SWFActionI action, int trigger)
	throws SWFException
        { action.eval(); nAddAction (handle, action.getHandle(), trigger); }


    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nAddShape (int handle, int shape, int which);
    protected native void	nAddAction (int handle, int action, int trigger);

};



    
