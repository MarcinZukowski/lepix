#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod_hr1.h"

static unsigned char xlbin[]={
#include "mod_hr1.xlc"
};

//static int dithering=0;
static int with_scr=0;
static int automode=0;

static unsigned char scr0[PICSIZE/8];
static unsigned char scr1[PICSIZE/8];
static unsigned char scr0hdr[] = {0xff, 0xff, 0x60, 0x40, 0x9f, 0x5f};
static unsigned char scr1hdr[] = {0xff, 0xff, 0x60, 0x60, 0x9f, 0x7f};
/*
static int CB[]={0x20,0x40,0x60,0x80,0xa0,0xc0,0x100};

static int lin0_cols[]={
		0x00,0x55,0xff,0x00,0x00,0x55,0xff,
		0x00,0xaa,0xff,0x00,0x00,0xaa,0xff,
		};
static int lin1_cols[]={
		0x00,0x00,0x00,0x55,0xff,0xff,0xff,
		0x00,0x00,0x00,0xaa,0xff,0xff,0xff,
		};
*/

static int colour_border[]={0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0x100};
static int colour_modeF[] ={   0,   0,   0,   0,   1,   1,   1,    1};
static int colour_modeE[] ={   0,   1,   2,   3,   0,   1,   2,    3};

static void usage()
{
	printf( 
	"HR1 options:\n"
	"\t-a <mode> color borders:\n"
		"\t\t0 - fixed\n"
		"\t\t1 - choose by color values\n"
		"\t\t2 - choose by color frequencies\n"
	"\t-c <#color> <hex value> - set atari colour\n"
	"\t-d <mode> use dithering:\n"
		"\t\t0 - no dithering\n"
		"\t\t1 - 'chessboard' dithering\n"
	"\t-S generate .sc0 and .sc1\n"
	);
}

static int parse_arg(int argc, char **argv)
{
	int i=0,nr,val;
	char *opt=argv[i++];
	
	switch(opt[1]) {
		case 'a':
			if (argc<i+1) {
				ERROR("Not enough parameters for -a\n");
			}
			sscanf(argv[i++],"%d", &val);
			if (val<0 && val>2) {
				ERROR("Possible automode values: 0,1,2\n");
			}
			automode=val;
			break ;
		case 'c':
			if (argc<i+2) {
				ERROR("Not enough parameters for -c\n");
			}
			sscanf(argv[i++],"%d", &nr);
			sscanf(argv[i++],"%x", &val);
			if (nr<0 && nr>3) {
				ERROR("Possible colors are 0..5\n");
			}
			xlbin[6+nr]=val;
			break;
		case 'd':
			if (argc<i+1) {
				ERROR("Not enough parameters for -d\n");
			}
			sscanf(argv[i++],"%d", &val);
			if (val<0 && val>1) {
				ERROR("Possible dithering values: 0,1\n");
			}
			automode=val;
			break;
		case 'S':
			with_scr=1;
			break;
		default:
			i=0;
	}
	return i ;
}


static void convert()
{
	int x,y;
	int dualsum=0 ;
	char *s0,*s1;
	memset(scr0, 0, PICSIZE8);
	memset(scr1, 0, PICSIZE8);
	
	INFO("hr1: Converting %s\n", fname);

	for(y=0;y<nlines;y++) {
		for(x=0;x<HSIZE;x++) {
			int c=bmap[y*HSIZE+x];
			int c0,c1; // atari colour

			if (y&1) { // odd lines
				s1=scr0+(y*HSIZE+x)/8;
				s0=scr1+(y*HSIZE+x)/8;
			} else { // even lines
				s0=scr0+(y*HSIZE+x)/8;
				s1=scr1+(y*HSIZE+x)/8;
			}
			dualsum+=c ;
#if 1
			// gr 8
			c0 = (c > 0x80) ;
			// gr 15
			if (x&1) {
				dualsum/=2;
				if (dualsum > 0x80) {
					if (dualsum > 0xc0) 
						c1=3;
					else
						c1=2;
				} else {
					if (dualsum > 0x40)
						c1=1;
					else
						c1=0;
				}
				dualsum=0;
			} else {
				c1 = 0 ;
			}
#else
			for(c0=0; c>colour_border[c0]; c0++) ;
			c0=colour_modeF[c0];
			if (x&1) {
				dualsum/=2;
				for(c1=0; dualsum>colour_border[c1]; c1++);
				c1=colour_modeE[c1];
				dualsum=0;
			} else {
				c1 = 0;
			}
#endif
			*s0 |= c0 << (7-(x&7));
			*s1 |= c1 << (6-(x&6));
		}
	}
}

static void write_files()
{
	FILE *fd;
	char fn[1000];
	int i;

	if (with_scr) {	
		sprintf(fn, "%s.sc0", fname_base);
		INFO("Writing %s\n", fn);
		fd=fopen(fn,"wb");
		fwrite(scr0, 1, PICSIZE8, fd);
		fclose(fd);
		
		sprintf(fn, "%s.sc1", fname_base);
		INFO("Writing %s\n", fn);
		fd=fopen(fn,"wb");
		fwrite(scr1, 1, PICSIZE8, fd);
		fclose(fd);
	}
	
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

modt mod_hr1 = {
	"hr1",
	"320x200, 8 shades of gray",
	parse_arg,
	usage,
	convert,
	write_files
};

modt* init_mod_hr1()
{
	return &mod_hr1;
}
