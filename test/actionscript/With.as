// prevent use of legacy functions inside with block

with(mc)
{
	startDrag();
	trace("trace in with");
}
