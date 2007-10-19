o = new Object ();
o.x = "hi";

// two cases 
// handled differntly by the compiler
for (var x in o) {
  continue;
};

// 2. without VAR
for (y in o) {
  continue;
};

// ENUMERATE2
for(x in foo["bar"])
{
	;
}
