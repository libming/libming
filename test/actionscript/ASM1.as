// Test mix of asm and V6 syntax
// It is assumed this file will be compiled as target SWF6 or higher

// this is only accepted in AS_V6 start condition
try{};

// mix the asm block ...
asm {
push "1"
push "2"
add
trace 
};

// this is only accepted in AS_V6 start condition
try{};
