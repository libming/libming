class A {
	function A() {
		trace("A");
	}
};

class B extends A {
	function B() {
		super();
	}
};

x = new B();
