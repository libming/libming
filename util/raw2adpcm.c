/*
 * raw2adpcm converts a raw sound data to adpcm compressed.
 * This is based on adpcm.cpp found at http://www.openswf.org .
 * The original code is the code posted on news:forums.macromedia.com by Jonathan Gay.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../src/blocks/input.h"
#include "../src/blocks/output.h"
#include "../src/libming.h"

/*
 * ADPCM tables
 */

static const int piIndexTable2[2] =
{
    -1, 2,
};

static const int piIndexTable3[4] =
{
    -1, -1, 2, 4,
};

static const int piIndexTable4[8] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static const int piIndexTable5[16] =
{
 -1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16,
};

static const int* ppiIndexTables[] =
{
    piIndexTable2,
    piIndexTable3,
    piIndexTable4,
    piIndexTable5
};

static const int piStepSizeTable[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void writeADPCMData(short *samples, int stereo, int sample_count, SWFOutput output) {
  int nBits = 4;  /* number of bits per ADPCM sample */
  int iSignBit = 1 << (nBits-1);  /* Sign bit mask */
  const int*  piIndexTable = ppiIndexTables[nBits-2];  /* Select index table to use */

  int iValPred[2] = {0, 0};   /* Predicted output value(s) */
  int iIndex[2]   = {0, 0};   /* Indeces int StepSizeTable */
  int iStep[2]    = {0, 0};   /* Step size */

  int iSampleCount = sample_count;  /* Number of samples. */
  int iChannelCount = 1 + stereo;  /* Number of channels (mono, stereo) */

  short*  psSample = samples;  /* Pointer to start of 16-bit/sample data */

  int i;

  /* Write number of bits per ADPCM sample */
  SWFOutput_writeBits(output, nBits-2, 2);

  for (i=0; i < iSampleCount; i++) {
    if ((i & 0xfff) == 0) {
      int c;
      for (c=0; c<iChannelCount; c++) {
	/* First sample in the block, so no delta */
	short sSample = *psSample++;
	int iDiff;

	/* Write full 16-bit sample */
	SWFOutput_writeSBits(output, (int)sSample, 16);  /* XXX - should be swapped? */
	iValPred[c] = sSample;

	/* Calculate initial index & step */
	if (iSampleCount == 1)  /* XXX - stereo OK? */
	  iDiff = abs(*psSample - sSample);
	else
	  iDiff = abs(*(psSample+1) - sSample);
	for (iIndex[c]=0; iIndex[c] < 88; iIndex[c]++)
	  if (iDiff <= piStepSizeTable[iIndex[c]])
	    break;

	if (iIndex[c] > 63)
	  iIndex[c] = 63;

	iStep[c] = piStepSizeTable[iIndex[c]];

	/* Write initial index into StepSizeTable */
	SWFOutput_writeBits(output, (int)iIndex[c], 6);
      }
    } else {
      int c;
      for (c=0; c<iChannelCount; c++) {
	short sSample = *psSample++;
	int iDiff, iSign, iDelta, iVPDiff;
	int k;

	/* Step 1 - compute difference with previous value */
	iDiff = sSample - iValPred[c];

	/* Make iDiff absolute value */
	iSign = (iDiff < 0) ? iSignBit : 0;
	if (iSign)
	  iDiff = (-iDiff);

	/*
	 * Step 2 - Divide and clamp
	 * Note:
	 * This code *approximately* computes:
         *    iDelta = iDiff*4/iStep;
         *    iVPDiff = (iDelta+0.5)*iStep/4;
         * but in shift step bits are dropped. The net result of this is
         * that even if you have fast mul/div hardware you cannot put it to
         * good use since the fixup would be too expensive.
	 */
	iDelta  = 0;
	iVPDiff = (iStep[c] >> (nBits-1));

	for (k = nBits-2; k >= 0; k--, iStep[c] >>= 1) {
	  if (iDiff >= iStep[c]) {
	    iDelta |= (1<<k);
	    iVPDiff += iStep[c];
	    if (k > 0)
	      iDiff -= iStep[c];
	  }
	}

	/* Step 3 - Update previous value */
	if (iSign)
	  iValPred[c] -= iVPDiff;
	else
	  iValPred[c] += iVPDiff;

	/* Step 4 - Clamp previous value to 16 bits */
	if (iValPred[c] > 32767)
	  iValPred[c] = 32767;
	else if (iValPred[c] < -32768)
	  iValPred[c] = -32768;

	/* Step 5 - Assemble value, update index and step values */
	iIndex[c] += piIndexTable[iDelta];
	iDelta |= iSign; /* Or with iSign *after* indexing */

	/* Clamp StepSizeTable index */
	if (iIndex[c] < 0) iIndex[c] = 0;
	if (iIndex[c] > 88) iIndex[c] = 88;
	iStep[c] = piStepSizeTable[iIndex[c]];

	/* Step 6 - Output value */
	SWFOutput_writeBits(output, iDelta, nBits);
      }
    }
  }
}

int main(int argc, char *argv[])
{
  FILE *fp_in, *fp_out;
  SWFInput input;
  SWFOutput output;
  int bits16;
  int stereo;
  int sample_count;
  short *samples, *i_sample;
  int i;

  if (argc < 3) {
    printf(
"%s converts a raw sound data to adpcm compressed.\n"
"\n"
"Usage:\n"
"%s in out [16bit] [stereo]\n"
"\n"
"in     : the filename of input file, raw sound data\n"
"out    : the filename of output file, ADPCM compressed\n"
"16bit  : bits per sample     0=8bit 1=16bit   default=1=16bit\n"
"stereo : number of channels  0=mono 1=stereo  default=1=stereo\n"
, argv[0], argv[0]);
    exit(0);
  }

  fp_in = fopen(argv[1], "rb");
  if (!fp_in) {
    fprintf(stderr, "file '%s' cannot open.\n", argv[1]);
    exit(1);
  }
  fp_out = fopen(argv[2], "wb");
  if (!fp_out) {
    fprintf(stderr, "file '%s' cannnot open.\n", argv[2]);
    exit(1);
  }
  bits16 = 1;
  if (argc > 3) bits16 = atoi(argv[3]);
  stereo = 1;
  if (argc > 4) stereo = atoi(argv[4]);

  /* read RAW sound data */
  input = newSWFInput_file(fp_in);
  sample_count = SWFInput_length(input) / (1+bits16) / (1+stereo);
  samples = malloc(sizeof(short) * sample_count * (1+stereo));
  i_sample = samples;
  for (i=0; i<sample_count*(1+stereo); i++) {
    if (bits16 == 0) {
      *i_sample = (short)SWFInput_getChar(input);
    } else {
      *i_sample = (short)SWFInput_getSInt16(input);
    }
    i_sample++;
  }
  destroySWFInput(input);
  fclose(fp_in);

  /* create a SWFOutput */
  output = newSWFOutput();

  writeADPCMData(samples, stereo, sample_count, output);

  /* write to file */
  SWFOutput_writeToMethod(output, fileOutputMethod, fp_out);

  fclose(fp_out);

  exit(0);
}
