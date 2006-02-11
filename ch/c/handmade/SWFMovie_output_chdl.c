#include <ming.h>
#include <ch.h>

static ChInterp_t interp;
static void *fileOutputMethod_funptr;
static void fileOutputMethod_funarg(byte b, void *data);

EXPORTCH int SWFMovie_output_chdl(void *varg) {
    va_list ap;
    SWFMovie movie;
    SWFByteOutputMethod method;
    void *data;
  //  int level;
    int retval;

    Ch_VaStart(interp, ap, varg);
    movie = Ch_VaArg(interp, ap, SWFMovie);
    method = Ch_VaArg(interp, ap, SWFByteOutputMethod);
    if(method != NULL)
    {
       fileOutputMethod_funptr = (void *)method;
       method = (SWFByteOutputMethod)fileOutputMethod_funarg;
    }
    data = Ch_VaArg(interp, ap, void *);
    /* level = Ch_VaArg(interp, ap, int); */
    retval = SWFMovie_output(movie, method, data); //, level);
    Ch_VaEnd(interp, ap);
    return retval;
}

static void fileOutputMethod_funarg(byte b, void *data)
{
    Ch_CallFuncByAddr(interp, fileOutputMethod_funptr, NULL, b, data);
}
