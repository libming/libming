//
// Description:
//    SWFSound Class
//
// Authors:
// Klaus Rechert


//
//  SWFSoundStream Class
//	create mp3 based sound stream
//
//  Notes
//    -	no methods at the moment other than a constructor
//
public class SWFSoundStream extends SWFObject {

    public SWFSoundStream (String file) 
    throws SWFException
    {
	setHandle(nNew (file)); 
    }

    protected native int nNew (String file);
};



    
