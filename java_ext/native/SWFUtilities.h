//
// Description:
//    SWFUtilities Implementation
//
// Authors:
//    Jonathan Shore <jshore@e-shuppan.com>
//    Based on php wrapper developed by <dave@opaque.net>
//
// Copyright:
//    Copyright 2001 E-Publishing Group Inc.  Permission is granted to use or
//    modify this code provided that the original copyright notice is included.
//
//    This software is distributed with no warrantee of liability, merchantability,
//    or fitness for a specific purpose.
//


#include <stdlib.h>
 


//
//  StringStream Class
//	implement a MING output stream suitable for java
//
//  Notes
//    -	
//
class StringStream {
    public:

	StringStream ()
	{
		buffer = NULL;
		len = 0;
		blen = 0;
		
	}

	~StringStream ()
	{
	    if (buffer) free (buffer);
	}

	
	void add (byte b)
	{
	    if (blen > len)
		buffer [len++] = b;
	    else if (blen == 0) {
		buffer = (byte*)malloc (8192);
		blen = 8192;
		buffer [len++] = b;
	    } else {
		buffer = (byte*)realloc ((void*)buffer, 2*blen);
		blen = 2*blen;
		buffer [len++] = b;
	    }
	}


	int length ()
	{
	    return len;
	}


	byte* getBytes ()
	{
	    return buffer;
	}
		
	
	static void hook (byte b, void* data)
	{
	    ((StringStream*)data)->add (b);
	}



    private:

	int	len;
	int	blen;
	byte*	buffer;
};


		
