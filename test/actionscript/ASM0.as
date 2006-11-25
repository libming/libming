// Simple test for asm {} blocks
asm {
push "1"
push "2"
add
trace // should print 12 (string addition)
};
