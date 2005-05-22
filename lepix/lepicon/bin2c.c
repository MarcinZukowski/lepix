#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *fdin, *fdout;
	unsigned char c;
	int i;
	
	if (argc!=3) {
		printf("usage: %s <infile> <outfile>\n", argv[0]);
		exit(-1);
	}

	fdin=fopen(argv[1],"rb");
	if (!fdin) {
		printf("Can't open %s for reading\n", argv[1]);
		exit(-1);
	}

	fdout=fopen(argv[2],"wb");
	if (!fdin) {
		printf("Can't open %s for writing\n", argv[2]);
		exit(-1);
	}
	
	while(fread(&c,1,1,fdin)) {
		fprintf(fdout, "0x%02x, ",c);
		if ((i&7)==7) {
			fprintf(fdout,"\n");
		}
		i++;
	}
	fprintf(fdout,"\n");
	return 0;
}
