
/* Set this to the default include path */
#define DEFAULT_FLAGS "-I/usr/share/actionscript -I/usr/local/share/actionscript -I."

/**
 * Use a value of 0 to skip the preprocessing phase (unreccommended)
 */
void makeswf_set_dopreprocess(int value);

/**
 * Set SWF version to the given number
 * !! Make sure you'll save your SWF with that version !!
 */
void makeswf_set_swfversion(int value);

/**
 * Add CPP flags to the default ones
 */
void makeswf_append_cpparg(const char* buf);

/**
 * Return an SWFAction by compiling code in the given sourcefile.
 *
 * Note that the source code can use the preprocessor, unless
 * preprocessing phase is skipped by a call to makeswf_set_dopreprocess(0)
 *
 */
SWFAction makeswf_compile_source(const char* filename);

