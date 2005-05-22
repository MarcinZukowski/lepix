#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "common.h"

int is_silent=0;

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
