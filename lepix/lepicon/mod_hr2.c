#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod_hr2.h"

static unsigned char xlbin[]={
#include "mod_hr2.xlc"
};

static unsigned char scr0[PICSIZE/8];
static unsigned char scr1[PICSIZE/8];
static unsigned char scr0hdr[] = {0xff, 0xff, 0x60, 0x40, 0x9f, 0x5f};
static unsigned char scr1hdr[] = {0xff, 0xff, 0x60, 0x60, 0x9f, 0x7f};

static unsigned char colorsets[] = {
	2, 4, 0, 2, 4, 6,
	4, 8, 0, 4, 8, 12,
} ;

static void usage()
{
	printf( 
	"HR2 options:\n"
	"\t-B <algorithm> - border generation algorithm:\n"
	"\t\t0 - fixed [default]\n"
	"\t\t1 - by value range\n"
	"\t\t2 - by value frequency\n"
	"\t-c <#color> <hex value> - set atari colour\n"
	"\t-C <#colorset> - atari colorset:\n"
	"\t\t0 - darker, less flickering [default]\n"
	"\t\t1 - brighter, more flickering\n"
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
			if (nr<0 || nr>5) {
				ERROR("Possible colors are 0..5\n");
			}
			xlbin[6+nr]=val;
			break;
		case 'C':
			if (argc<i+1) {
				ERROR("Not enough parameters for -C\n");
			}
			sscanf(argv[i++],"%d", &val);
			if (val<0 || val>1) {
				ERROR("Possible colorsets are 0,1\n");
			}
			for(nr=0;nr<6;nr++) {
				xlbin[6+nr]=colorsets[6*val+nr];
			}
			break ;
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
	char *s0,*s1;
	memset(scr0, 0, PICSIZE8);
	memset(scr1, 0, PICSIZE8);
	int colors[] = {
		//	col15	col8a	col8b  
			0,		0,		0,		// 00 !
			0,		0,		1,		// 01 !
			1,		0,		0,		// 02 . == 11
			1,		0,		1,		// 03 . == 12
			1,		1,		1,		// 04 . == 22
			0,		1,		0,		// 10 !
			1,		0,		0,		// 11 !
			1,		0,		1,		// 12 !
			1,		1,		1,		// 13 . == 22
			2,		0,		1,		// 14 . == 23
			1,		0,		0,		// 20 . == 11
			1,		1,		0,		// 21 !
			1,		1,		1,		// 22 ! or 2 0 0
			2,		0,		1,		// 23 !
			2,		1,		1,		// 24 . == 33
			1,		1,		0,		// 30 . == 21
			1,		1,		1,		// 31 . == 22
			2,		1,		0,		// 32 !
			2,		1,		1,		// 33 !
			3,		0,		1,		// 34 !
			1,		1,		1,		// 40 . == 22
			2,		1,		0,		// 41 . == 32
			2,		1,		1,		// 42 . == 33
			3,		1,		0,		// 43 !
			3,		1,		1,		// 44 !
	};
	
	CM_initialize(5);
	
	INFO("hr2: Converting %s\n", fname);
	
	for(y=0;y<nlines;y++) {
		for(x=0;x<HSIZE;x+=2) {
			int b0=bmap[y*HSIZE+x];
			int b1=bmap[y*HSIZE+x+1];
			int c0,c1;
			int ac0,ac1,ac2;
			int ind;
			
			c0 = CM_get_color(x,y,b0);
			c1 = CM_get_color(x+1,y,b1);

//			for(c0=0; b0>color_borders[c0]; c0++);
//			for(c1=0; b1>color_borders[c1]; c1++);
			ind=5*c0+c1;

			if (y&1) { // odd lines
				s1=scr0+(y*HSIZE+x)/8;
				s0=scr1+(y*HSIZE+x)/8;
			} else { // even lines
				s0=scr0+(y*HSIZE+x)/8;
				s1=scr1+(y*HSIZE+x)/8;
			}
			
			ac0=colors[3*ind+0];
			ac1=colors[3*ind+1];
			ac2=colors[3*ind+2];
			
			*s1 |= ac0 << (6-(x&6)); //gr 15
			*s0 |= ac1 << (7-(x&7)) | ac2<< (7-(x&7)-1);
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
	fwrite(scr1hdr, 1, sizeof(scr1hdr), fd);
	fwrite(scr1, 1, PICSIZE8, fd);
	fwrite(xlbin, 1, sizeof(xlbin), fd);
	fclose(fd);
	INFO("Atari colors used: ");
	for(i=0;i<6;i++) {
		INFO("%02x ", xlbin[6+i]);
	}
	INFO("\n");
}

modt mod_hr2 = {
	"hr2",
	"320x200, 5 shades of gray",
	parse_arg,
	usage,
	convert,
	write_files
};

modt* init_mod_hr2()
{
	return &mod_hr2;
}
