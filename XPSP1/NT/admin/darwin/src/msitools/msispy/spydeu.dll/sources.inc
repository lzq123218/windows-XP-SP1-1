
TARGETNAME=SpyDeu
!INCLUDE ..\..\..\..\MsiMake.inc
MSISPY_DIR=$(SRC_DIR)\MsiTools\MsiSpy

TARGETTYPE=DYNLINK
SUBSYSTEM_VERSION=4.0
SUBSYSTEM_WINVER=4.0

NTTARGETFILE0=DeuResAll

UMTYPE=console
DLLDEF=
TARGETPATH=$(MSI_BUILD_TARGET)
TARGETLIBS=$(O)\deuall.res

C_DEFINES=$(C_DEFINES) -D_EXE
USE_NOLIBS=1

INCLUDES=
SOURCES=..\spydeu.cpp
