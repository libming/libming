#include "perl_swf.h"

// Export stuff. Blatintly copied from Apache::Constants. Thanks Doug ;-)
// TODO: Check AUTOLOAD

static char *SWF_Fill_tag[] = {
     "SWFFILL_SOLID",
     "SWFFILL_GRADIENT",
     "SWFFILL_LINEAR_GRADIENT",
     "SWFFILL_RADIAL_GRADIENT",
     "SWFFILL_BITMAP",
     "SWFFILL_TILED_BITMAP",
     "SWFFILL_CLIPPED_BITMAP",
     NULL,
};

static char *SWF_Button_tag[] = {
     "SWFBUTTON_HIT",
     "SWFBUTTON_DOWN",
     "SWFBUTTON_OVER",
     "SWFBUTTON_UP",
     "SWFBUTTON_MOUSEUPOUTSIDE",
     "SWFBUTTON_DRAGOVER",
     "SWFBUTTON_DRAGOUT",
     "SWFBUTTON_MOUSEUP",
     "SWFBUTTON_MOUSEDOWN",
     "SWFBUTTON_MOUSEOUT",
     "SWFBUTTON_MOUSEOVER",
     "SWFBUTTON_KEYPRESS",
     "SWFBUTTON_ONKEYPRESS",
     NULL,
};


static char *SWF_TextField_tag[] = {
     "SWFTEXTFIELD_NOEDIT",
     "SWFTEXTFIELD_PASSWORD",
     "SWFTEXTFIELD_DRAWBOX",
     "SWFTEXTFIELD_MULTILINE",
     "SWFTEXTFIELD_WORDWRAP",
     "SWFTEXTFIELD_NOSELECT",
     "SWFTEXTFIELD_ALIGN_LEFT",
     "SWFTEXTFIELD_ALIGN_RIGHT",
     "SWFTEXTFIELD_ALIGN_CENTER",
     "SWFTEXTFIELD_ALIGN_JUSTIFY",
     NULL,
};



static char **export_tags(char *tag) {
   switch (*tag) {
   case 'B':
     if(strEQ("Button", tag))
       return SWF_Button_tag;
   case 'F':
     if(strEQ("Fill", tag))
       return SWF_Fill_tag;
   case 'T':
     if(strEQ("Text", tag))
       return SWF_TextField_tag;
   default:
     croak("unknown tag `%s'", tag);
   }
}


void export_cv(SV *class, SV *caller, char *sub) 
{ 
    GV *gv; 
#if 0
    fprintf(stderr, "Here is the result: *%s::%s = \\&%s::%s\n", 
            SvPVX(caller), sub, SvPVX(class), sub); 
#endif 
    gv = gv_fetchpv(form("%_::%s", caller, sub), TRUE, SVt_PVCV); 
    GvCV(gv) = perl_get_cv(form("%_::%s", class, sub), TRUE); 
    GvIMPORTED_CV_on(gv); 
} 

static void my_import(SV *pclass, SV *caller, SV *sv)
{
    char *sym = SvPV(sv,na), **tags;
    int i;

    switch (*sym) {
    case ':':
        ++sym;
        tags = export_tags(sym);
        for(i=0; tags[i]; i++) {
            export_cv(pclass, caller, tags[i]);
        }
        break;
    case '$':
    case '%':
    case '*':
    case '@':
        croak("\"%s\" is not exported by the SWF module", sym);
    case '&':
        ++sym;
    default:
        if(isALPHA(sym[0])) {
            export_cv(pclass, caller, sym);
            break;
        }
        else {
            croak("Can't export symbol: %s", sym);
        }
    }
}
