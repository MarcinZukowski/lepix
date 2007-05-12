MADS=mads

CFLAGS=-Wall -g

all: lepicon

MODS=mod_hr0 mod_hr1 mod_hr2 mod_gr8

MODS_O=$(MODS:%=%.o)

%.obx: %.asx
	$(MADS) $< -s
	
%.xlc: %.obx bin2c
	./bin2c $< $@

mod_hr0.o: mod_hr0.c mod_hr0.h mod_hr0.xlc
mod_hr1.o: mod_hr1.c mod_hr1.h mod_hr1.xlc
mod_hr2.o: mod_hr2.c mod_hr2.h mod_hr2.xlc
mod_gr8.o: mod_gr8.c mod_gr8.h mod_gr8.xlc

lepicon: lepicon.o common.o $(MODS_O) 

clean:
	rm -f \
		lepicon lepicon.exe \
		bin2c bin2c.exe \
		*.xlc *.obx *.o *.LST *.TAB
