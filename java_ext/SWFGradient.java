//
// Description:
//    SWFGradientI Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//


//
//  SWFGradient Class
//	gradient
//
//  Notes
//    -	
//
public class SWFGradient extends SWFObject implements SWFGradientI {

    public SWFGradient ()
	throws SWFException
    {
	setHandle (nNew ()); 
    }

    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }

    public void	addEntry (float ratio, int r, int g, int b, int alpha)
    { 
	nAddEntry (handle, ratio, r,g,b, alpha); 
    }

  public void addEntry( float ratio, SWFColor clr )
  {
    addEntry( ratio, clr.getRed(), clr.getGreen(), clr.getBlue(),
              clr.getAlpha() );
  }
    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);
    protected native void	nAddEntry (int handle, float ratio, int r, int g, int b, int alpha);

};



    
