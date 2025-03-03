W32_SB=1

MAJORCOMP=windows
MINORCOMP=console

TARGETNAME=conlibk
TARGETPATH=$(_OBJ_DIR)
PASS1_PUBLISH=\
    {$(DYNLINK_LIB)=$(WINDOWS_LIB_DEST)\$(DYNLINK_LIBDIR)}
TARGETTYPE=LIBRARY

INCLUDES=..;                    \
   ..\..\inc;                   \
   $(WINCORE_PATH)\w32inc;      \
   $(WINCORE_PATH)\w32inc\$(O); \
   $(BASE_INC_PATH)

!IFNDEF MSC_WARNING_LEVEL
MSC_WARNING_LEVEL=/W3
!ENDIF
MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /WX

C_DEFINES=$(C_DEFINES) -D_KERNEL32_

PRECOMPILED_INCLUDE=..\precomp.h
PRECOMPILED_PCH=precomp.pch
PRECOMPILED_OBJ=precomp.obj
