#!/bin/sh

VERSION="0.2"

rm -f lepicon-$VERSION.zip lepicon-src-$VERSION.tgz lepicon

make clean
make
strip lepicon.exe
tar czvf lepicon-$VERSION.tgz \
	lepicon.exe \
	
tar czvf lepicon-src-$VERSION.tgz \
	lepicon.c bin2c.c Makefile \
	common.c common.h \
	mod_hr0.asx mod_hr0.c mod_hr0.h \
	mod_hr1.asx mod_hr1.c mod_hr1.h \
	mod_hr2.asx mod_hr2.c mod_hr2.h \
	
	