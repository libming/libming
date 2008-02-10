
var f1 = function()
{
	return "anonymous function() works";
}; // we need a semicolon to end a statement !

// // 'Function' is not recognized, try using 'new Function' instead
// var f2 = Function()
// {
// };

function test()
{
	return "function <name>() works";
} // no need to terminate with a semicolon

// // 'Function' is a reserved word
// Function test()
// {
// 	// I don't think this should work 
// } // no need to terminate with a semicolon

trace(test instanceOf Function);

// 'function' is a reserved word
// trace(test instanceOf function);

// This is the one that failed with tknip patch for AS2 support
function getName():Object { return this.name; }

var f1:String = function():String
{
	return "anonymous function() works";
}; // we need a semicolon to end a statement !

function foo(x:Number, y:Object)
{
	;
}

function bar()
{
	return new this;
}
