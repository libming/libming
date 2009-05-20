// Use all supported ASM keywords
//
// NOTE: the resulting SWF makes NO sense !!

asm {
dup
swap
push "string", undefined, null, 12
pop
setregister r:1
callfunction
callmethod
and
or
xor
modulo
add
newadd
lessthan
newlessthan
equals
newequals
inc
dec
enumerate
enumerate2
initobject
initarray
getmember
setmember
shiftleft
shiftright
shiftright2
//varequals // known as token but unhandled
oldadd
subtract
multiply
divide
oldequals
oldlessthan
logicaland
logicalor
not
stringeq
stringlength
substring
getvariable
setvariable
//settargetexpr // known as token, but unhandled
//startdrag // known as token, but unhandled
//stopdrag // known as token, but unhandled
stringlessthan
mblength
mbsubstring
mbord
mbchr
push "66"
branch "10"
branchalways "20"
branchiftrue "30"
push "1", "2", "3"
trace
tonumber
tostring
getproperty
setproperty
//post // known as token, but unhandled
//get // known as token, but unhandled
//end // known as token, but unhandled
implements
extends
fscommand2
new
delete
delete2
};

