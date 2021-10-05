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
OBJECTS=dis.obj

#
# which is the 'goal' file
#
TARGET=dis.exe

#
# the great library
#
LIBFILE=

#
# currently used C compiler
#
CCOMP=cl

#
# options for the C compiler
#
CFLAGS=-c -AH -G2 -Gs -FPa -Zi

#
# currently used disembler
#
ACOMP=masm

#
# options for the disembler
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
	echo $(TARGET),$*,$(LIBFILE); >> lnk
	$(LINKER) $(LFLAGS) @lnk
	del lnk

#
# and maybe the target has created here ...
#
