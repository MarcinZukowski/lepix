RM=E:/cygwin/bin/rm
MV=E:/cygwin/bin/mv
LS=E:/cygwin/bin/ls
CAT=E:/cygwin/bin/cat
MADS=E:/a8/bin/mads_err.bat
MADS_FLAGS=/l /t /m:e:\a8\bin\mads\Przyklady\xasm_macro.asm

SRCS=lepix.asx dl.asx dli.asx vbl.asx cursor.asx utils.asx zoom.asx \
	joy.asx keyboard.asx \
	io.asx menu.asx \
	mod_cin.asx mod_mic.asx mod_inp.asx mod_hip.asx

TABS=$(SRCS:%.asx=%.tab)
LSTS=$(SRCS:%.asx=%.lst)

default: lepix.xex

lepix.xex: $(SRCS)
	$(MADS) $< /o:$@ $(MADS_FLAGS)

run: lepix.xex
	E:\a8\bin\a800win-4.0beta\Atari800Win.exe -run $<

clean:
	$(RM) -f lepix.xex \
		$(TABS) $(LSTS) \
		*.tab *.TAB *.lst *.LST *.lab *.LAB \
		*.MACRO*

