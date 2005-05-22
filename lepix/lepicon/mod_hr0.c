#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod_hr0.h"

static unsigned char xlbin[]={
#include "mod_hr0.xlc"
};

static int dithering=0;
static int with_scr=0;
static int automode=0;

static unsigned char scr0[PICSIZE/8];
static unsigned char scr1[PICSIZE/8];
static unsigned char scr0hdr[] = {0xff, 0xff, 0x60, 0x40, 0x9f, 0x5f};
static unsigned char scr1hdr[] = {0xff, 0xff, 0x60, 0x60, 0x9f, 0x7f};

static int CB[]={0x20,0x40,0x60,0x80,0xa0,0xc0,0x100};

static int lin0_cols[]={
		0x00,0x55,0xff,0x00,0x00,0x55,0xff,
		0x00,0xaa,0xff,0x00,0x00,0xaa,0xff,
		};
static int lin1_cols[]={
		0x00,0x00,0x00,0x55,0xff,0xff,0xff,
		0x00,0x00,0x00,0xaa,0xff,0xff,0xff,
		};

static void usage()
{
	printf( 
	"HR0 options:\n"
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
				ERROR("Possible colors are 0..3\n");
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
			dithering=val;
			break;
		case 'S':
			with_scr=1;
			break;
		default:
			i=0;
	}
	return i ;
}

static void gen_borders()
{
	int minc=256,maxc=0,i;
	int buckets[256];

	if (automode==0) {
		return ;
	}
	
	memset(buckets,0,sizeof(int)*256);
	for(i=0;i<PICSIZE;i++) {
		if (bmap[i]<minc) {
			minc=bmap[i];
		}
		if (bmap[i]>maxc) {
			maxc=bmap[i];
		}
		buckets[bmap[i]]++;
	}
	INFO("Found color range: %2x..%2x\n", minc, maxc);
	if (automode==1) {
		// by color
		INFO("Generating borders by color\n");
		for(i=0;i<6;i++) {
			float step=((float)maxc-minc)/7;
			float val=minc+(step*(i+1));
			CB[i]=val;
		}
	} else {
		// by frequency
		INFO("Generating borders by frequency\n");
		int col=0,step=PICSIZE/7,border=step,sum=0;
		for(i=minc;i<=maxc;i++) {
			sum+=buckets[i];
			if (sum>=border) {
				CB[col++]=i;
				border+=step;
			}
		}
	}
	CB[i]=maxc;
}

static void convert()
{
	int x,y;
	memset(scr0, 0, PICSIZE8);
	memset(scr1, 0, PICSIZE8);
	
	gen_borders();
	
	INFO("hr0: Converting %s %s dithering\n", fname, dithering?"with":"without");
	INFO("Using color borders: ");
	for(x=0;x<7;x++) {
		INFO("%2x ",CB[x]);
	}
	INFO("\n");

	for(y=0;y<nlines;y++) {
		
		for(x=0;x<HSIZE;x++) {
			int c=bmap[y*HSIZE+x];
			int ac,c0,c1; // atari colour
			for(ac=0; c>CB[ac]; ac++) ;
			if (dithering) {
				ac += 7*(y&1);
			} else {
				ac &= ~1;
			}
			if (y&1) {//even lines
				c0=lin0_cols[ac];
				c1=lin1_cols[ac];
			} else {
				c0=lin1_cols[ac];
				c1=lin0_cols[ac];
			}
//			printf("c=%2x ac=%d c0=%2x c1=%2x\n", c, ac, c0, c1);
			scr0[(y*HSIZE+x)/8] |= c0 & (1<<(7-(x&7)));
			scr1[(y*HSIZE+x)/8] |= c1 & (1<<(7-(x&7)));
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
	for(i=0;i<4;i++) {
		INFO("%02x ", xlbin[6+i]);
	}
	INFO("\n");
}

modt mod_hr0 = {
	"hr0",
	"320x200, 4 shades of gray",
	parse_arg,
	usage,
	convert,
	write_files
};

modt* init_mod_hr0()
{
	return &mod_hr0;
}
