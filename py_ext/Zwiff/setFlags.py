
cn = """
Ming_setCubicThreshold
Ming_setScale
Ming_useSWFVersion
SWFRect
SWFShape
SWFFill
SWFDisplayItem
SWFMovie
SWFSprite
SWFMovieClip
SWFMorph
SWFMorphShape
SWFFont
SWFBitmap
SWFDBLBitmap
JpegBitmap
SWFText
SWFText2
SWFTextField
SWFSound
SWFAction
SWFGradient
SWFButton
"""

import ming

for item in [getattr(ming,x) for x in cn.split()]:
    item.__allow_access_to_unprotected_subobjects__=1

