#include <stdio.h>
#include <stdlib.h>

#define print(x)	{fputs(x,stdout);}

void dumpBytes(FILE *f)
{
  int j=0, i, k, b;
  unsigned char buf[16];

  for(;;)
  {
    printf("%03X0: ",j);
    ++j;

    for(i=0; i<16; ++i)
    {
      b = fgetc(f);
      if(b == EOF)
	break;

      printf("%02x ", buf[i] = (unsigned char)b);

      if(i==7) putchar(' ');
    }

    if(b == EOF)
    {
      for(k=i; k<16; ++k)
      {
	if(k==7) putchar(' ');
	print("   ");
      }
    }

    print("   ");

    for(k=0; k<i; ++k)
    {
      if((buf[k] > 31) && (buf[k] < 128))
	putchar(buf[k]);
      else
	putchar('.');

      if(k==7) putchar(' ');
    }

    putchar('\n');

    if(b == EOF)
      break;
  }

  putchar('\n');
}

int main(int argc, char *argv[])
{
  FILE *f;
  int i=0, c;

  if(argc<2 || !(f = fopen(argv[1], "rb")))
  {
    printf("Gimme a file name.\n");
    exit(1);
  }

  dumpBytes(f);

  return 0;
}
