// Test the 'extends' opcode.
//
// The code below should be equivalent to AS2:
//
//   class BaseClass1 { };
//   class DerivedClass1 extends BaseClass1 { };
//
asm {
        push "DerivedClass1"
        getvariable
        push "BaseClass1"
        getvariable
        extends
};
