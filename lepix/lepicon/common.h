#ifndef _COMMON_H
#define _COMMON_H

#define HSIZE 320
#define VSIZE 200
#define PICSIZE ((HSIZE)*(VSIZE))
#define PICSIZE8 ((PICSIZE)/8)

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

#endif /* _COMMON_H */
