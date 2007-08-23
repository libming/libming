
function test()
{
	try {
 		throw new Error();
	}
	catch (err)
	{
		trace(err);	
	}
	finally {
		trace("done");
	};
} 


