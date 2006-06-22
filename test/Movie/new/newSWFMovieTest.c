#include <libming.h>
#include "../../run_test.c";

main()
{
	int failures=0;

	/*SWFMovie m = newSWFMovie();*/
	failures += run_test(newSWFMovie(), "test01");
	failures += run_test(newSWFMovieWithVersion(4), "test02");
	failures += run_test(newSWFMovieWithVersion(5), "test03");
	failures += run_test(newSWFMovieWithVersion(6), "test06");
	failures += run_test(newSWFMovieWithVersion(7), "test07");

	return failures;
}
