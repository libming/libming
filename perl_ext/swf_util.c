/* ====================================================================
 * Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================

 * $Author$
 * $Id$
 *
 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"

static HV *swf_refcnt_hv = Nullhv;

void swf_util_cleanup(void)
{
  hv_undef(swf_refcnt_hv);
  SvREFCNT_dec((SV*)swf_refcnt_hv);
  swf_refcnt_hv = Nullhv;
}

void swf_stash_refcnt_inc(SV *sv_key, SV *sv_value)
{
    AV *av_value = Nullav;
    STRLEN klen;
    char *key;
    int store = 0;
    dTHR;

    key = SvPV(sv_key, klen);

    if(swf_refcnt_hv == Nullhv){
	swf_refcnt_hv = newHV();
    }
    else if(hv_exists(swf_refcnt_hv, key, klen)) {
	SV *entry = *hv_fetch(swf_refcnt_hv, key, klen, FALSE);
	if(entry && SvTRUE(entry) && SvROK(entry)) 
	    av_value = (AV*)SvRV(entry);
    }

    if(av_value == Nullav){
      av_value= newAV();
      store = 1;
    }

    av_push (av_value, sv_value);
    SvREFCNT_inc(sv_value);
    if(av_value && store)
	hv_store(swf_refcnt_hv, key, klen, (SV*)newRV((SV*)av_value), FALSE);
}

void swf_stash_refcnt_dec(SV *sv_key)
{
  STRLEN klen;
  char *key;
  I32 i;
  I32 len;
  SV *sv_value;

  if(!swf_refcnt_hv) return;

  key = SvPV(sv_key, klen);
  if(hv_exists(swf_refcnt_hv, key, klen)) {
    SV *entry = *hv_fetch(swf_refcnt_hv, key, klen, FALSE);
    AV *av;
    if(!SvTRUE(entry) && !SvROK(entry))
      return;
    
    av = (AV*)SvRV(entry);
    len = av_len(av);
    for(i=0; i<=len; i++) {
      //sv_value = *av_fetch(av, i, FALSE); 
      sv_value = av_pop(av);
      SvREFCNT_dec(sv_value);
    }
    (void)hv_delete(swf_refcnt_hv, key, klen, G_DISCARD);
    av_undef(av);
    SvREFCNT_dec((SV*)av);

    if(hv_iterinit(swf_refcnt_hv) == 0)
      swf_util_cleanup();

  }
}


