#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod_gr8.h"

static unsigned char xlbin[]={
#include "mod_gr8.xlc"
};

static unsigned char scr0[PICSIZE/8];
static unsigned char scr0hdr[] = {0xff, 0xff, 0x60, 0x40, 0x9f, 0x5f};


static void usage()
{
	printf( 
	"HR2 options:\n"
	"\t-B <algorithm> - border generation algorithm:\n"
	"\t\t0 - fixed [default]\n"
	"\t\t1 - by value range\n"
	"\t\t2 - by value frequency\n"
	"\t-c <#color> <hex value> - set atari colour\n"
	"\t-D <algorithm> - use dithering algorithm:\n"
	"\t\t0 - none [default]\n"
	"\t\t1 - chessboard\n"
	"\t\t2 - progressive (experimental)\n"
	"\t\t3 - Floyd-Steinberg\n"
	);
}

static int parse_arg(int argc, char **argv)
{
	int i=0,nr,val;
	char *opt=argv[i++];
	
	switch(opt[1]) {
		case 'B':
			if (argc<i+1) {
				ERROR("Not enough parameters for -B\n");
			}
			sscanf(argv[i++],"%d", &val);
			if (val<0 || val>2) {
				ERROR("Possible border generation algorithms are 0,1,2\n");
			}
			CM_border_algo=val;
			break ;
		case 'c':
			if (argc<i+2) {
				ERROR("Not enough parameters for -c\n");
			}
			sscanf(argv[i++],"%d", &nr);
			sscanf(argv[i++],"%x", &val);
			if (nr<0 || nr>1) {
				ERROR("Possible colors are 0..1\n");
			}
			xlbin[6+nr]=val;
			break;
		case 'D':
			if (argc<i+1) {
				ERROR("Not enough parameters for -D\n");
			}
			sscanf(argv[i++],"%d", &val);
			if (val<0 || val>3) {
				ERROR("Possible dithering algorithms are 0,1,2,3\n");
			}
			CM_dither_algo=val;
			break ;
		default:
			i=0;
	}
	return i ;
}

static void convert()
{
	int x,y;
	char *s0;
	memset(scr0, 0, PICSIZE8);
	
	CM_initialize(2);
	
	INFO("hr1: Converting %s\n", fname);
	
	for(y=0;y<nlines;y++) {
		for(x=0;x<HSIZE;x++) {
			int b0=bmap[y*HSIZE+x];
			int c0;
			
			c0 = CM_get_color(x,y,b0);

			s0 = scr0+(y*HSIZE+x)/8;
			
			*s0 |= c0 << (7-(x&7)) ;
		}
	}
}

static void write_files()
{
	FILE *fd;
	char fn[1000];
	int i;

	sprintf(fn, "%s.xex", fname_base);
	INFO("Writing %s\n", fn);
	fd=fopen(fn,"wb");
	fwrite(scr0hdr, 1, sizeof(scr0hdr), fd);
	fwrite(scr0, 1, PICSIZE8, fd);
	fwrite(xlbin, 1, sizeof(xlbin), fd);
	fclose(fd);
	INFO("Atari colors used: ");
	for(i=0;i<2;i++) {
		INFO("%02x ", xlbin[6+i]);
	}
	INFO("\n");
}

modt mod_gr8 = {
	"gr8",
	"320x200, 2 shades of gray",
	parse_arg,
	usage,
	convert,
	write_files
};

modt* init_mod_gr8()
{
	return &mod_gr8;
}
