#include <libming.h>
#include "../../run_test.c";

main()
{
	int failures=0;

	SWFMovie m = newSWFMovie();
	SWFMovie_setRate(m, 24.00);
	failures += run_test(m, "test01");

	return failures;
}
