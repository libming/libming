//
// Description:
//    SWFTextField Class
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
//  SWFText Class
//	text field
//
//  Notes
//    -	
//
public class SWFTextField extends SWFObject implements SWFTextFieldI {

    public SWFTextField ()
	throws SWFException
    {
	setHandle (nNew());
    }

    protected void finalize()
	throws Throwable
    {
	nDestroy (handle);
	super.finalize();
    }



    // methods

    public void	setBounds (float width, float height)
        { nSetBounds (handle, width, height); }
    public void	setFlags (long flags)
        { nSetFlags (handle, flags); }
    public void setHeight( float height ) 
        { nSetHeight( handle, height ); }
    public void	setAlignment (int alignment)
        { nSetAlignment (handle, alignment); }
    public void	align (int alignment)
        { nSetAlignment (handle, alignment); }

    public void	setFont (SWFFontI font)
	throws SWFException
    { 
	font.eval(); 
	nSetFont (handle, font.getHandle()); 
	preserve (font);
    }

    public void	setColor (int r, int g, int b, int alpha)
        { nSetColor (handle, r,g,b, alpha); }
    public void	setColor (SWFColor color)
        { nSetColor (handle, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }

    public void	addString (String text)
        { nAddString (handle, text); }
    public void	setVariableName (String name)
        { nSetVariableName (handle, name); }

    public void	setLeftMargin (float margin)
        { nSetLeftMargin (handle, margin); }
    public void	setRightMargin (float margin)
        { nSetRightMargin (handle, margin); }
    public void	setIndentation (float indent)
        { nSetIndentation (handle, indent); }
    public void	setLineSpacing (float spacing)
        { nSetLineSpacing (handle, spacing); }
    public void	setLength (int len)
        { nSetLength (handle, len); }


    // native methods

    protected native int	nNew ();
    protected native void	nDestroy (int handle);

    protected native void	nSetFont (int handle, int Hfont);
    protected native void	nSetColor (int handle, int r, int g, int b, int alpha);

    protected native void       nSetHeight( int handle, float height );
    protected native void	nAddString (int handle, String text);

    protected native void	nSetBounds (int handle, float width, float height);
    protected native void	nSetFlags (int handle, long flags);
    protected native void	nSetAlignment (int handle, int alignment);

    protected native void	nSetVariableName (int handle, String name);

    protected native void	nSetLeftMargin (int handle, float margin);
    protected native void	nSetRightMargin (int handle, float margin);
    protected native void	nSetIndentation (int handle, float indent);
    protected native void	nSetLineSpacing (int handle, float spacing);
    protected native void	nSetLength (int handle, int len);

};



    
