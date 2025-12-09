# Microsoft Developer Studio Generated NMAKE File, Based on UnKanjC.dsp
!IF "$(CFG)" == ""
CFG=UnKanjC - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの UnKanjC - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "UnKanjC - Win32 Release" && "$(CFG)" !=\
 "UnKanjC - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "UnKanjC.mak" CFG="UnKanjC - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "UnKanjC - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "UnKanjC - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "UnKanjC - Win32 Release"

OUTDIR=.\ReleaseC
INTDIR=.\ReleaseC
# Begin Custom Macros
OutDir=.\ReleaseC
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unkanj.exe"

!ELSE 

ALL : "$(OUTDIR)\unkanj.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\unkanj.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\unkanj.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /Fp"$(INTDIR)\UnKanjC.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\
 
CPP_OBJS=.\ReleaseC/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnKanjC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib kernel32.lib user32.lib gdi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\unkanj.pdb" /machine:I386\
 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\unkanj.exe" 
LINK32_OBJS= \
	"$(INTDIR)\unkanj.obj"

"$(OUTDIR)\unkanj.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UnKanjC - Win32 Debug"

OUTDIR=.\DebugC
INTDIR=.\DebugC
# Begin Custom Macros
OutDir=.\DebugC
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unkanj.exe"

!ELSE 

ALL : "$(OUTDIR)\unkanj.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\unkanj.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\unkanj.exe"
	-@erase "$(OUTDIR)\unkanj.map"
	-@erase "$(OUTDIR)\unkanj.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_MBCS" /Fp"$(INTDIR)\UnKanjC.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\DebugC/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnKanjC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\unkanj.pdb" /map:"$(INTDIR)\unkanj.map" /debug /machine:I386\
 /out:"$(OUTDIR)\unkanj.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\unkanj.obj"

"$(OUTDIR)\unkanj.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "UnKanjC - Win32 Release" || "$(CFG)" ==\
 "UnKanjC - Win32 Debug"
SOURCE=.\unkanj.cpp
DEP_CPP_UNKAN=\
	".\pp_unkanj.h"\
	".\swenv.h"\
	".\Unkanj.h"\
	

"$(INTDIR)\unkanj.obj" : $(SOURCE) $(DEP_CPP_UNKAN) "$(INTDIR)"



!ENDIF 

