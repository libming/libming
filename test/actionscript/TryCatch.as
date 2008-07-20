
function test1()
{
 	trace('test1');
	try
	{
 		trace(' try');
	}
	catch (err) // should be skipped
	{
		trace(' catch');
	}
	finally // should be executed
	{
		trace(" finally");
	};
	trace("/test1");
} 

function test2()
{
 	trace('test2');
	try
	{
 		throw new Error();
 		trace(' try');
	}
	catch (err) // should be executed
	{
		trace(' catch: '+err);
	}
	finally // should be executed
	{
		trace(" finally");
	};
	trace("/test2");
} 

function test3()
{
 	trace('test3');
	try
	{
 		trace(' try');
	}
	catch (err) // should be skipped
	{
		trace(' catch');
	};
	trace("/test3");
} 

function test4()
{
 	trace('test4');
	try
	{
 		trace(' try');
	}
	finally // should be executed
	{
		trace(" finally");
	};
	trace("/test4");
} 

function test5()
{
 	trace('test5');
	try
	{
 		trace(' try');
	};
	trace("/test5");
} 

function test6(dothrow)
{
 	trace('test6('+dothrow+')');
	try
	{
		if ( dothrow ) throw 0;
 		trace(' try');
	}
	catch (err) // should be only executed if throwing
	{
		trace(' catch');
	}
	finally // should be executed
	{
		trace(" finally");
	};
	trace("/test6");
} 

test1();
test2();
test3();
test4();
test5();
test6(false);
test6(true);


