#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "mod_hr0.h"
#include "mod_hr1.h"
#include "mod_hr2.h"
#include "mod_gr8.h"

char* 
#include "VERSION"
;

char *fname = NULL ;
char *fname_base = NULL ;
int is_gray=0;
int is_test=0;

unsigned char bmap[PICSIZE];
int nlines=VSIZE;

int argc=0;
char **argv=NULL;

/* mod management */
typedef modt* (mod_init)();
mod_init* mod_inits[] = {
	init_mod_hr0,
	init_mod_hr1,
	init_mod_hr2,
	init_mod_gr8,
} ;
int mod_nr = 2 ;

modt** mods = NULL ;
int mod_cnt = sizeof(mod_inits)/sizeof(mod_init*);

static void 
init_modules() 
{
	int i;
	mods = (modt**) malloc(mod_cnt*sizeof(modt*));
	for(i=0;i<mod_cnt;i++) {
		mods[i]=mod_inits[i]();
	}
}

void info()
{
	INFO("lepicon v.%s (c) 2005 eru/tqa\n", VERSION);
}

void usage()
{
	int i;
	info();
	printf("Usage: %s [options] <file.bmp>\n", argv[0]);
	printf(
	"file.bmp needs to be 320x200, 256 shades of gray BMP file\n"
	"optionally, you can use 64000b gray file with -g switch\n"
	"Options:\n"
	"\t-g assume file is GRAY (no BMP header)\n"
	"\t-s be silent\n"
	"\t-t generate a test picture\n"
	"\t-h help\n"
	"\t-m <mod> choose a module, availabe modules:\n");
	for(i=0;i<mod_cnt;i++) {
		printf("\t\t%s - %s %s\n", mods[i]->id, mods[i]->info,
			(i==mod_nr)?"[current]":"" );
	}
	mods[mod_nr]->usage();
}

void parse_options()
{
	int i=1,j ;
	while(i<argc) {
		char *opt = argv[i++];
		if (opt[0]=='-') {
			/* options */
			switch(opt[1]) {
				case 'g':
					is_gray=1;
					break;
				case 'h':
					usage();
					exit(0);
					break;
				case 'm':
					for(j=0;j<mod_cnt;j++) {
						if (strcasecmp(mods[j]->id, argv[i])==0) {
							mod_nr=j;
							break ;
						}
					}
					if(j==mod_cnt) {
						ERROR("Unknown mode: %s\n", argv[i]);
					}
					i++;
					break; 
				case 's':
					is_silent=1;
					break;
				case 't': {
					int x,y;
					for(y=0;y<200;y++){
						for(x=0;x<320;x++){
							bmap[y*320+x]=(x+y)&0xff;
						}
					}
					fname_base="test";
					is_test=1;
				}
					break ;
				default:
					j = mods[mod_nr]->parse_arg(argc-i+1,argv+i-1);
					if (!j) {
						ERROR("Unknown option: %s, try -h\n",opt);
					}
					i+=j-1;
			}
		} else {
			/* argument */
			fname = opt;
		}
	}
	if (!fname && !is_test) {
		printf("Filename missing\n");
		exit(-1);
	}
}

void read_file()
{
	int rd,i;
	FILE *fd ;
	unsigned char buf[HSIZE];

	memset(bmap, 0, PICSIZE);

	fd=fopen(fname,"rb");
	if (!fd) {
		ERROR("Can't open file %s\n", fname);
	}
	if (!is_gray) {
		rd=fread(bmap, 1, 1078, fd);
		if (rd!=1078 || strncmp(bmap,"BM",2)) {
			ERROR("Not a proper BMP file\n");
		}
	}
	rd=fread(bmap, 1, PICSIZE, fd);
	nlines=rd/HSIZE;
	if (rd-nlines*HSIZE) {
		ERROR("Incorrect file, read %d bytes",rd);
	}
	INFO("Read %d lines\n", nlines);
	fclose(fd);
	
	/* if a bitmap, rotate upside down */
	if (!is_gray) {
		for(i=0;i<VSIZE/2;i++) {
			char *ad0=bmap+i*HSIZE;
			char *ad1=bmap+(VSIZE-1-i)*HSIZE;
			memcpy(buf, ad0, HSIZE);
			memcpy(ad0, ad1, HSIZE);
			memcpy(ad1, buf, HSIZE);
		}
	}
	
	/* prepare file name */
	fname_base = strdup(fname);
	for(i=0;fname_base[i] && fname[i]!='.';i++) ;
	fname_base[i]=0 ;
}

int main(int _argc, char **_argv)
{
	argc = _argc ;
	argv = _argv ;

	init_modules();
	
	parse_options();
	
	info();
	
	if (!is_test) {
		read_file();
	}
	
	mods[mod_nr]->convert();

	mods[mod_nr]->write_files();

	return 0 ;
}
