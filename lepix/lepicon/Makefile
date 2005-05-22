MADS=e:/a8/bin/mads/mads.exe

CFLAGS=-Wall -g

all: lepicon

MODS=mod_hr0 mod_hr1 mod_hr2

MODS_O=$(MODS:%=%.o)

%.OBX: %.asx
	$(MADS) $< //s
#	@echo ""

%.xlc: %.OBX bin2c
	./bin2c $< $@
	
mod_hr0.o: mod_hr0.c mod_hr0.h mod_hr0.xlc
mod_hr1.o: mod_hr1.c mod_hr1.h mod_hr1.xlc
mod_hr2.o: mod_hr2.c mod_hr2.h mod_hr2.xlc

lepicon: lepicon.o common.o $(MODS_O) 

clean:
	rm -f atapcon atapcon.exe bin2c bin2c.exe \
		*.xlc *.OBX *.o *.LST *.TAB
