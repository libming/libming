# Building 
Build the java library and the native library, running make in the java 
directory (the current directory) and "native" directory respectively.

The java build will produce a jswf.jar file which can be included in your
classpath.  The native directory creates a libjswf.so file.  This can be
placed anywhere (I typically put it in the same installation directory as
my jar files).


# Using It
To compile or run against this library should do the following (csh):

```sh
   setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:<directory-with-.so-files>

   java or javac -classpath <old-path>:<jar dir>/jswf.jar
```

Make sure you also include libming.so in the directory with your libjswf.so.
Alternatively, add the ming library to your LD_LIBRARY_PATH.


# Notes
The Java class heirarchy and methods closely mirror the PHP and python
interfaces. I've added some additional convenience functions and new
functionality.

Also, have separated the implementation from the interface definition.  This
makes browsing the API easier and also allows for other implementations.
Methods that result SWF objects will return SWF interfaces rather than the 
explicit objects.  For example:

```Java
	SWFMovie movie = new SWFMovie();
	...
	SWFDisplayItemI item = movie.add (shape);
	item.moveTo (23,30);
```
	
The add method returns ```SWFDisplayItemI``` rather than ```SWFDisplayItem```.  No big
deal.  Just be aware that interfaces (identified with an "I" suffix) are
used most of the time.

# Contact
Jonathan Shore <jonathan.shore@gmail.com>
