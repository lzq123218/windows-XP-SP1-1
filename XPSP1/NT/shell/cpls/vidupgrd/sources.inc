TARGETNAME=vidupgrd
TARGETPATH=obj
TARGETTYPE=DYNLINK
TARGETEXT=dll

INCLUDES=$(BASE_INC_PATH)

TARGETLIBS=$(SDK_LIB_PATH)\advapi32.lib     \
           $(SDK_LIB_PATH)\user32.lib       \
           $(SDK_LIB_PATH)\kernel32.lib     \
           $(SDK_LIB_PATH)\gdi32.lib        \
           $(SDK_LIB_PATH)\setupapi.lib     

USE_MSVCRT=1

!IFNDEF MSC_WARNING_LEVEL
MSC_WARNING_LEVEL=/W3
!ENDIF

MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /WX
C_DEFINES=-DWIN32 -DW3 -DUNICODE -D_UNICODE

SOURCES=..\migrate.cpp   \
        ..\deskcmmn.cpp

DLLDEF=$(O)\vidupgrd.def
