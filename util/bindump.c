#include <stdio.h>
#include <stdlib.h>

#define print(x)	{fputs(x,stdout);}

void dumpBytes(FILE *f)
{
  int count=0, j, i, k, b;
  unsigned char buf[4];

  for(;;)
  {
    printf("%04X: ", count);
    count += 4;

    for(i=0; i<4; ++i)
    {
      b = fgetc(f);
      if(b == EOF)
	break;

      printf("%02x ", buf[i] = (unsigned char)b);
    }

    if(b == EOF)
    {
      for(k=i; k<4; ++k)
      {
	print("   ");
      }
    }

    print("   ");

    for(k=0; k<i; ++k)
    {
      b = buf[k];
      for(j=0; j<8; j++)
      {
	if(b&0x80)
	  putchar('1');
	else
	  putchar('0');

	b <<= 1;
      }

      putchar(' ');
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

  if(argc<2 || !(f = fopen(argv[1], "rb")))
  {
    printf("Gimme a file name.\n");
    exit(1);
  }

  dumpBytes(f);

  return 0;
}
