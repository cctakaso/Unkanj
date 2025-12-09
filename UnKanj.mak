# Microsoft Developer Studio Generated NMAKE File, Based on UnKanj.dsp
!IF "$(CFG)" == ""
CFG=UnKanj - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの UnKanj - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "UnKanj - Win32 Release" && "$(CFG)" != "UnKanj - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "UnKanj.mak" CFG="UnKanj - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "UnKanj - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "UnKanj - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "UnKanj - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\UnKanj.dll"

!ELSE 

ALL : "$(OUTDIR)\UnKanj.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\unkandll.obj"
	-@erase "$(INTDIR)\unkanj.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\UnKanj.dll"
	-@erase "$(OUTDIR)\UnKanj.exp"
	-@erase "$(OUTDIR)\UnKanj.lib"
	-@erase "$(OUTDIR)\UnKanj.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\UnKanj.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnKanj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\UnKanj.pdb" /map:"$(INTDIR)\UnKanj.map"\
 /machine:I386 /def:".\unkanj.def" /out:"$(OUTDIR)\UnKanj.dll"\
 /implib:"$(OUTDIR)\UnKanj.lib" 
DEF_FILE= \
	".\unkanj.def"
LINK32_OBJS= \
	"$(INTDIR)\unkandll.obj" \
	"$(INTDIR)\unkanj.obj"

"$(OUTDIR)\UnKanj.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UnKanj - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\UnKanj.dll"

!ELSE 

ALL : "$(OUTDIR)\UnKanj.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\unkandll.obj"
	-@erase "$(INTDIR)\unkanj.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\UnKanj.dll"
	-@erase "$(OUTDIR)\UnKanj.exp"
	-@erase "$(OUTDIR)\UnKanj.ilk"
	-@erase "$(OUTDIR)\UnKanj.lib"
	-@erase "$(OUTDIR)\UnKanj.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\UnKanj.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnKanj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\UnKanj.pdb" /debug /machine:I386\
 /def:".\unkanj.def" /out:"$(OUTDIR)\UnKanj.dll" /implib:"$(OUTDIR)\UnKanj.lib"\
 /pdbtype:sept 
DEF_FILE= \
	".\unkanj.def"
LINK32_OBJS= \
	"$(INTDIR)\unkandll.obj" \
	"$(INTDIR)\unkanj.obj"

"$(OUTDIR)\UnKanj.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "UnKanj - Win32 Release" || "$(CFG)" == "UnKanj - Win32 Debug"
SOURCE=.\unkandll.cpp

!IF  "$(CFG)" == "UnKanj - Win32 Release"

DEP_CPP_UNKAN=\
	".\pp_unkanj.h"\
	".\swenv.h"\
	".\Unkanj.h"\
	

"$(INTDIR)\unkandll.obj" : $(SOURCE) $(DEP_CPP_UNKAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "UnKanj - Win32 Debug"

DEP_CPP_UNKAN=\
	".\pp_unkanj.h"\
	".\swenv.h"\
	".\Unkanj.h"\
	

"$(INTDIR)\unkandll.obj" : $(SOURCE) $(DEP_CPP_UNKAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\unkanj.cpp

!IF  "$(CFG)" == "UnKanj - Win32 Release"

DEP_CPP_UNKANJ=\
	".\pp_unkanj.h"\
	".\swenv.h"\
	".\Unkanj.h"\
	

"$(INTDIR)\unkanj.obj" : $(SOURCE) $(DEP_CPP_UNKANJ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "UnKanj - Win32 Debug"

DEP_CPP_UNKANJ=\
	".\pp_unkanj.h"\
	".\swenv.h"\
	".\Unkanj.h"\
	

"$(INTDIR)\unkanj.obj" : $(SOURCE) $(DEP_CPP_UNKANJ) "$(INTDIR)"


!ENDIF 


!ENDIF 

