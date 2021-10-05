# Microsoft Visual C++ generated build script - Do not modify

PROJ = WGET
DEBUG = 1
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\SOURCES\W30\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = WGET.C      
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /D_DOS
CFLAGS_R_DEXE = /nologo /W3 /FR /G2 /DNDEBUG /Gs /Ox /AM /D_DOS
LFLAGS_D_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /CO /STACK:5120
LFLAGS_R_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /STACK:5120
LIBS_D_DEXE = oldnames mlibce
LIBS_R_DEXE = oldnames mlibce
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = WGET.SBR \
		W30_UTIL.SBR \
		DO_WDIR.SBR


WGET_DEP = c:\sources\w30\w30.h


W30_UTIL_DEP = 

DO_WDIR_DEP = c:\sources\w30\w30.h


all:	$(PROJ).EXE $(PROJ).BSC

WGET.OBJ:	WGET.C $(WGET_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c WGET.C

W30_UTIL.OBJ:	W30_UTIL.C $(W30_UTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c W30_UTIL.C

DO_WDIR.OBJ:	DO_WDIR.C $(DO_WDIR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DO_WDIR.C

$(PROJ).EXE::	WGET.OBJ W30_UTIL.OBJ DO_WDIR.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
WGET.OBJ +
W30_UTIL.OBJ +
DO_WDIR.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
