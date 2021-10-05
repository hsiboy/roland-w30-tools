#
# how to make .obj from .c files
#
.c.obj :
	$(CCOMP) $(CFLAGS) $*.c

#
# how to make .obj from .asm files
#
.asm.obj :
	$(ACOMP) $(AFLAGS) $*;

#
# which are the object files for this project
#
OBJECTS=w30ibm.obj disk.obj

#
# which is the 'goal' file
#
TARGET=w30ibm.exe

#
# currently used C compiler
#
CCOMP=cl

#
# options for the C compiler
#
CFLAGS=-c -AH -G2 -Gs -FPa -Zi

#
# currently used assembler
#
ACOMP=masm

#
# options for the assembler
#
AFLAGS=-Mx

#
# currently used linker
#
LINKER=link

#
# options for the linker
#
LFLAGS=/code

#
# from which files the target depends and how to make the target
#
$(TARGET) : $(OBJECTS)
	del *.~??
	echo $(OBJECTS) > lnk
	echo $(TARGET); >> lnk
	$(LINKER) $(LFLAGS) @lnk

#
# and maybe the target has created here ...
#
