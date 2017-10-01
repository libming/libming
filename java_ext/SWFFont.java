//
// Description:
//    SWFFont Class
//
// Authors:
//    Jonathan Shore <jonathan.shore@gmail.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 Jonathan Shore
//







//
//  SWFFont Class
//	font specification & attributes
//
//  Notes
//    -	
//
public class SWFFont extends SWFObject implements SWFFontI {
    
    public SWFFont (String font)
	throws SWFException
    {
	if (font.endsWith (".fdb")) {
	    setHandle (nNewFileFont (font));
	    browserfont = false;
	} else {
	    setHandle (nNewBrowserFont (font));
	    browserfont = true;
	}
    }

    protected void finalize()
	throws Throwable
    {
	if (browserfont)
	    nDestroyBrowserFont (handle);
	else
	    nDestroyFileFont (handle);
	super.finalize();
    }


    public float getStringWidth (String string)
        { return nGetStringWidth (handle, string); }
    public float getAscent ()
        { return nGetAscent (handle); }
    public float getDescent ()
        { return nGetDescent (handle); }
    public float getLeading ()
        { return nGetLeading (handle); }

    
    // native methods

    protected native int	nNewFileFont (String font);
    protected native int	nNewBrowserFont (String font);
    protected native void	nDestroyFileFont (int handle);
    protected native void	nDestroyBrowserFont (int handle);

    protected native float	nGetStringWidth (int handle, String string);
    protected native float	nGetAscent (int handle);
    protected native float	nGetDescent (int handle);
    protected native float	nGetLeading (int handle);


    // variables

    private boolean		browserfont;
};



    
