//
// Description:
//    SWFTextField Interface
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
//	text field
//
//  Notes
//    -	
//
public interface SWFTextFieldI extends SWFObjectI {
    
    // alignment constants

    public final static int AlignLeft = 0;
    public final static int AlignRight = 1;
    public final static int AlignCenter = 2;
    public final static int AlignJustify = 3;
    
    // miscellaneous constants (flags)

    public final static int HasLength = (1<<1);
    public final static int NoEdit = (1<<3);
    public final static int Password = (1<<4);
    public final static int Multiline = (1<<5);
    public final static int WordWrap = (1<<6);
    public final static int DrawBox = (1<<11);
    public final static int NoSelect = (1<<12);
    public final static int Html = (1<<9);


    // methods

    public void		setBounds (float width, float height);
    public void		setFlags (long flags);
    public void         setHeight(float height );
    public void		setAlignment (int alignment);
    public void		align (int alignment);

    public void		setFont (SWFFontI font) throws SWFException;
    public void		setColor (int r, int g, int b, int alpha);
    public void		setColor (SWFColor color);

    public void		addString (String text);
    public void		setVariableName (String name);

    public void		setLeftMargin (float margin);
    public void		setRightMargin (float margin);
    public void		setIndentation (float indent);
    public void		setLineSpacing (float spacing);
    public void		setLength (int len);


};



    
