/*
 * SWFVideoStream.class
 * 10.3.2004 Klaus Rechert
 */

public class SWFVideoStream extends SWFObject {

    public SWFVideoStream (String file) 
    throws SWFException
    {
	setHandle(nNewFile (file)); 
    }

    public SWFVideoStream() throws SWFException
    {
	    setHandle(nNew());
    }

    public void setDimension(int width, int height)
    {
	    nSetDimension(handle, width, height);
    }

    public int getNumFrames()
    {
	    return nGetNumFrames(handle);
    }	    

    protected native int nNewFile (String file);
    protected native int nNew ();
    protected native void nSetDimension(int handle, int width, int height);
    protected native int nGetNumFrames(int handle);
};



    
