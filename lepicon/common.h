#ifndef _COMMON_H
#define _COMMON_H

#define HSIZE 320
#define VSIZE 200
#define PICSIZE ((HSIZE)*(VSIZE))
#define PICSIZE8 ((PICSIZE)/8)

#define MAX(x,y) (((x)>=(y))?(x):(y))
#define MIN(x,y) (((x)<=(y))?(x):(y))
#define FREE(x) { if (x) { free(x) ; x=NULL; } }

extern int is_silent ;
extern char *fname, *fname_base;
extern unsigned char bmap[PICSIZE];
extern int nlines;

typedef struct {
	char *id ;
	char *info ;
	int (*parse_arg)(int argc, char **argv);
	void (*usage)();
	void (*convert)();
	void (*write_files)();
} modt ;

void INFO(char *format, ...) ;
void ERROR(char *format, ...) ;

/* Color management */

/* algo: 0-fixed, 1-value range 2-frequency */
extern int CM_border_algo ;
/* algo: 0-none, 1-chessboard 2-progressive 3-floyd-steinberg*/
extern int CM_dither_algo ;

void CM_initialize(int num_colors);
int CM_get_color(int x, int y, int value);

#endif /* _COMMON_H */
