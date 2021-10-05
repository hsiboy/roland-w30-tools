.AUTODEPEND

#		*Translator Definitions*
CC = bcc +A97.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = D:\TC\LIB
INCLUDEPATH = D:\TC\INCLUDE;C:\LIBI


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 a97.obj \
 a97_1.obj

#		*Explicit Rules*
a97.exe: a97.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0s.obj+
a97.obj+
a97_1.obj
a97
		# no map file
emu.lib+
maths.lib+
cs.lib
|


#		*Individual File Dependencies*
a97.obj: a97.cfg a97.c 

a97_1.obj: a97.cfg a97_1.c 

#		*Compiler Configuration File*
a97.cfg: a97.mak
  copy &&|
-2
-ff-
-g255
-v
-O
-Oe
-Ob
-Z
-k-
-r-
-vi-
-I$(INCLUDEPATH)
-L$(LIBPATH)
| a97.cfg


