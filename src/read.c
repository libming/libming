
static int buffer;
static int bufbits = 0; /* # of bits in buffer */

static void byteAlign()
{
  if(bufbits > 0)
  {
    bufbits = 0;
    buffer = 0;
  }
}

static int readBits(byte **p, int number)
{
  byte *ptr = *p;
  int ret = buffer;

  if(number == bufbits)
  {
    ret = buffer;
    bufbits = 0;
    buffer = 0;
  }
  else if(number > bufbits)
  {
    number -= bufbits;

    while(number>8)
    {
      ret <<= 8;
      ret += *ptr++;
      number -= 8;
    }

    buffer = *ptr++;

    if(number>0)
    {
      ret <<= number;
      bufbits = 8-number;
      ret += buffer >> (8-number);
      buffer &= (1<<bufbits)-1;
    }
  }
  else
  {
    ret = buffer >> (bufbits-number);
    bufbits -= number;
    buffer &= (1<<bufbits)-1;
  }

  *p = ptr;
  return ret;
}

static int readSBits(byte **p, int number)
{
  int num = readBits(p, number);

  if(num & (1<<(number-1)))
    return num - (1<<number);
  else
    return num;
}
