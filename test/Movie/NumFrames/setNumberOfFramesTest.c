#include <libming.h>
#include "../../run_test.c";

main()
{
	int failures=0;

	SWFMovie m = newSWFMovie();
	SWFMovie_setNumberOfFrames(m, 2);
	failures += run_test(m, "test01");

	return failures;
}
