
// test for asm push 
asm {
push "dict" // constant 
push 2 // integer
push null // null
push true // boolean
push false // boolean
push 0.0 // double
push r:0 // register
push undefined // undefined

// TODO:
//	- push "string" (hard to do as any string will end up in the dictionary)
};
