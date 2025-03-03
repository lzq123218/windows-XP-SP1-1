#
# Common makefile include for this project
#

# We don't define _ATL_MIN_CRT because we implement the functions ourselves
# (crtfree.h).

# Define _MERGE_PROXYSTUB to merge the proxy/stub code into the DLL
#C_DEFINES       = $(C_DEFINES) -D_MERGE_PROXYSTUB

# Use the NT product version numbers
USE_NT_PRODUCT_VER = 1
                  
# Turn structure validation ON in debug builds
#
!if "$(FREEBUILD)" == "0"
C_DEFINES       = $(C_DEFINES) -DVSTF       
!endif

!ifdef DOWNLEVEL_PLATFORM

C_DEFINES       = $(C_DEFINES) -DDOWNLEVEL_PLATFORM -DDOWNLEVEL

!endif

INCLUDES = $(INCLUDES);$(BASEDIR)\private\inc

!include $(CCSHELL_DIR)\common.inc
SOURCES_USED    =  $(SOURCES_USED) $(CCSHELL_DIR)\common.inc

LINKER_FLAGS    = $(LINKER_FLAGS) -MERGE:.CRT=.data

CONDITIONAL_INCLUDES = $(CONDITIONAL_INCLUDES) \
                    dlldata.c       \
                    shappmgr_p.c    \
                    statreg.h       \
                    statreg.cpp     \
