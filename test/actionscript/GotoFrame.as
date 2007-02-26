// These should get translated to tags
// (GOTOFRAME, GOTOFRAME2, GOTOLABEL)
gotoAndPlay(3);
gotoAndStop(3);
gotoAndPlay(_currentframe);
gotoAndStop(_currentframe);
gotoAndPlay("label1");
gotoAndStop("label1");

// These should get translated to method calls.
this.gotoAndPlay(3);
this.gotoAndStop(3);
this.gotoAndPlay(_currentframe);
this.gotoAndStop(_currentframe);
this.gotoAndPlay("label1");
this.gotoAndStop("label1");

