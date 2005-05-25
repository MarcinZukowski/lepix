#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "common.h"
#include <string.h>

int is_silent=0;

int CM_border_algo=0;
int CM_dither_algo=0;

static int *color_borders=NULL;
static int num_colors=0;

static int diffusion[PICSIZE];

void CM_initialize(int _num_colors)
{
	int i;
	int buckets[256];
	int minc=255,maxc=0;
	
	if (color_borders) {
		free(color_borders);
	}
	num_colors=_num_colors;
	color_borders = (int*) malloc(sizeof(int)*(num_colors+1));
	color_borders[0]=0;
	
	/* clear error diffusion */
	memset(diffusion, 0, sizeof(int)*PICSIZE);
	
	/* clear color histogram */
	memset(buckets, 0, sizeof(int)*256);
	
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
	switch(CM_border_algo) {
		case 0:
			INFO("Generating fixed palette\n");
			for(i=1;i<=num_colors;i++) {
				color_borders[i]=(i)*256/num_colors;
			}
			break ;
		case 1:
			INFO("Generating borders by color range\n");
			for(i=1;i<=num_colors;i++) {
				color_borders[i]=(i)*(maxc-minc+1)/num_colors + minc ;
			}
			break ;
		case 2:
			INFO("Generating borders by frequency\n");
			int col=1,step=PICSIZE/7,border=step,sum=0;
			for(i=minc;i<=maxc;i++) {
				sum+=buckets[i];
				if (sum>=border) {
					color_borders[col++]=i;
					border+=step;
				}
			}
			break;
		
		default:
			ERROR("color_borders_generate: Unknown algo\n");
	}
	color_borders[num_colors]=256;
	
	INFO("Color borders: ");
	for(i=0;i<=num_colors;i++) {
		INFO("%2x ",color_borders[i]);
	}
	INFO("\n");
}

static int reminder=0;

int CM_get_color(int x, int y, int value)
{
	int ret;
	if (x==0) {
		reminder=0;
	}
	switch(CM_dither_algo) {
		case 0:
			/* no dithering */
			for(ret=1; value>color_borders[ret]; ret++);
			ret--;
			break ;
		case 1:
			/* chessboard */
			for(ret=1; value>color_borders[ret]; ret++);
			if (ret!=num_colors && 
					value > (color_borders[ret]+color_borders[ret-1])/2) {
				// dithering
				ret += (x+y)&1;
			}
			ret--;
			break ;
		case 2:
			/* eru - progressive */
			value+=reminder;
			value=MIN(value,255);
			for(ret=1; value>color_borders[ret]; ret++);
			if (ret!=num_colors && 
					value > (color_borders[ret]+color_borders[ret-1])/2) {
				// dithering
				reminder=value-((color_borders[ret]+color_borders[ret-1])/2);
				ret += (x+y)&1;
			} else {
				reminder=value-color_borders[ret-1];
			}
			if (reminder<0) {
				ERROR("value=%d ret=%d cb[ret-1]=%d reminder=%d < 0", 
					value, ret, color_borders[ret-1], reminder);
			}
			ret--;
			break ;
		case 3:
			/* floyd-steinberg */
			value += diffusion[y*HSIZE+x] / 16;
			value=MIN(value,255);
			for(ret=1; value>color_borders[ret]; ret++);
			reminder = value-color_borders[ret-1];
			if (reminder<0) {
				ERROR("value=%d ret=%d cb[ret-1]=%d reminder=%d < 0", 
					value, ret, color_borders[ret-1], reminder);
			}
			if (x<HSIZE-1) {
					diffusion[(y+0)*HSIZE+x+1] += reminder * 7 ;
			}
			if (y<VSIZE-1) {
				if (x>0) {
					diffusion[(y+1)*HSIZE+x-1] += reminder * 3 ;
				}
				diffusion[(y+1)*HSIZE+x-0] += reminder * 5 ;
				if (x<HSIZE-1) {
					diffusion[(y+1)*HSIZE+x+1] += reminder * 1 ;
				}
			}
			ret--;
			break ;
		default:
			ERROR("find_color: unknown dithering\n");
	}
	return ret ;
}

void INFO(char *format, ...)
{
	if (!is_silent) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
}

void ERROR(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	fprintf(stderr,"ERROR:");
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(-1);
}
