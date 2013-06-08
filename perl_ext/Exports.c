/* ====================================================================
 * Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Id$
 */

#include "perl_swf.h"

static char *SWF_Fill_tag[] = {
     "SWFFILL_SOLID",
     "SWFFILL_GRADIENT",
     "SWFFILL_LINEAR_GRADIENT",
     "SWFFILL_RADIAL_GRADIENT",
     "SWFFILL_FOCAL_GRADIENT",
     "SWFFILL_BITMAP",
     "SWFFILL_TILED_BITMAP",
     "SWFFILL_CLIPPED_BITMAP",
     "SWFFILL_NONSMOOTHED_TILED_BITMAP",
     "SWFFILL_NONSMOOTHED_CLIPPED_BITMAP",
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

static char *SWF_Sound_tag[] = {
     "SWF_SOUND_NOT_COMPRESSED",
     "SWF_SOUND_ADPCM_COMPRESSED",
     "SWF_SOUND_MP3_COMPRESSED",
     "SWF_SOUND_NOT_COMPRESSED_LE",
     "SWF_SOUND_NELLY_COMPRESSED",
     "SWF_SOUND_5KHZ",
     "SWF_SOUND_11KHZ",
     "SWF_SOUND_22KHZ",
     "SWF_SOUND_44KHZ",
     "SWF_SOUND_8BITS",
     "SWF_SOUND_16BITS",
     "SWF_SOUND_MONO",
     "SWF_SOUND_STEREO",
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
     "SWFTEXTFIELD_HTML",
     "SWFTEXTFIELD_HASLENGTH",
     "SWFTEXTFIELD_USEFONT",
     "SWFTEXTFIELD_AUTOSIZE",
     NULL,
};


static char *SWF_DisplayItem_tag[] = {
     "SWFACTION_ONLOAD",
     "SWFACTION_ENTERFRAME",
     "SWFACTION_UNLOAD",
     "SWFACTION_MOUSEMOVE",
     "SWFACTION_MOUSEDOWN",
     "SWFACTION_MOUSEUP",
     "SWFACTION_KEYDOWN",
     "SWFACTION_KEYUP",
     "SWFACTION_DATA",
     "SWFBLEND_MODE_NULL",
     "SWFBLEND_MODE_NORMAL",
     "SWFBLEND_MODE_LAYER",
     "SWFBLEND_MODE_MULT",
     "SWFBLEND_MODE_SCREEN",
     "SWFBLEND_MODE_DARKEN",
     "SWFBLEND_MODE_LIGHTEN",
     "SWFBLEND_MODE_ADD",
     "SWFBLEND_MODE_SUB",
     "SWFBLEND_MODE_DIFF",
     "SWFBLEND_MODE_INV",
     "SWFBLEND_MODE_ALPHA",
     "SWFBLEND_MODE_ERASE",
     "SWFBLEND_MODE_OVERLAY",
     "SWFBLEND_MODE_HARDLIGHT",
     NULL,
};

static char *SWF_Filter_tag[] = {
     "SWF_FILTER_MODE_INNER",
     "SWF_FILTER_MODE_KO",
     "SWF_FILTER_MODE_COMPOSITE",
     "SWF_FILTER_MODE_ONTOP",
     "SWF_FILTER_CLAMP",
     "SWF_FILTER_FLAG_PRESERVE_ALPHA",
     NULL,
};

static char *SWF_Gradient_tag[] = {
     "SWF_GRADIENT_PAD",
     "SWF_GRADIENT_REFLECT",
     "SWF_GRADIENT_REPEAT",
     "SWF_GRADIENT_NORMAL",
     "SWF_GRADIENT_LINEAR",
     NULL,
};

static char *SWF_Shape_tag[] = {
     "SWF_SHAPE_USESCALINGSTROKES",
     "SWF_SHAPE_USENONSCALINGSTROKES",
     "SWF_LINESTYLE_CAP_ROUND",
     "SWF_LINESTYLE_CAP_NONE",
     "SWF_LINESTYLE_CAP_SQUARE",
     "SWF_LINESTYLE_JOIN_ROUND",
     "SWF_LINESTYLE_JOIN_BEVEL",
     "SWF_LINESTYLE_JOIN_MITER",
     "SWF_LINESTYLE_FLAG_NOHSCALE",
     "SWF_LINESTYLE_FLAG_NOVSCALE",
     "SWF_LINESTYLE_FLAG_HINTING",
     "SWF_LINESTYLE_FLAG_NOCLOSE",
     "SWF_LINESTYLE_FLAG_ENDCAP_ROUND",
     "SWF_LINESTYLE_FLAG_ENDCAP_NONE",
     "SWF_LINESTYLE_FLAG_ENDCAP_SQUARE",
     NULL,
};

static char *SWF_Video_tag[] = {
     "SWFVIDEOSTREAM_MODE_AUTO",
     "SWFVIDEOSTREAM_MODE_MANUAL",
     NULL,
};


static char **export_tags(char *tag) {
   switch (*tag) {
   case 'B':
     if(strEQ("Button", tag))
       return SWF_Button_tag;
   case 'D':
     if(strEQ("DisplayItem", tag))
       return SWF_DisplayItem_tag;
   case 'F':
     if(strEQ("Fill", tag))
       return SWF_Fill_tag;
     if(strEQ("Filter", tag))
       return SWF_Filter_tag;
   case 'G':
     if(strEQ("Gradient", tag))
       return SWF_Gradient_tag;       
   case 'S':
     if(strEQ("Sound", tag))
       return SWF_Sound_tag;
     if(strEQ("Shape", tag))
       return SWF_Shape_tag;
   case 'T':
     if(strEQ("Text", tag))
       return SWF_TextField_tag;
   case 'V':
     if(strEQ("Video", tag))
       return SWF_Video_tag;
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
    gv = gv_fetchpv(form("%s::%s",SvPVX( caller), sub), TRUE, SVt_PVCV); 
    GvCV_set(gv, perl_get_cv(form("%s::%s", SvPVX(class), sub), TRUE));
    GvIMPORTED_CV_on(gv); 
    GvMULTI_on(gv);
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
