#include <libming.h>
#include <run_test.h>

static char *test = "test01";

main()
{
	int failures=0;

	SWFMovie m = newSWFMovie();
	SWFMovie_setDimension(m, 640.00, 480.00);
	failures += run_test(m, "test01");

	return failures;
}
